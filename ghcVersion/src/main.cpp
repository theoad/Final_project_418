#include <iostream>
#include <ctime>
#include <string>
#include "CycleTimer.h"
#include "Halide.h"
#include "halide_image_io.h"
#include "rsa.h"
#include "cryptlib.h"
#include "osrng.h"
#define CIPHER_SIZE 192

////////////////////////////////////////////////
// Random number generator
CryptoPP::AutoSeededRandomPool rng;

////////////////////////////////////////////////
// Encryption and decryption schemes
CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor;
CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor;

////////////////////////////////////////////////
// Cipher text space
char* cipherSpace;

extern "C" uint8_t ByteEncrypt (uint8_t pixel_val, int index) {
	static const int SECRET_SIZE = 1;
	CryptoPP::SecByteBlock plaintext( SECRET_SIZE );
	memset( plaintext, pixel_val, SECRET_SIZE );	

	// Now that there is a concrete object, we can validate
	assert( 0 != encryptor.FixedMaxPlaintextLength() );
	assert( plaintext.size() <= encryptor.FixedMaxPlaintextLength() );

	// Create cipher text space
	size_t ecl = encryptor.CiphertextLength( plaintext.size() );
	assert( 0 != ecl );
	CryptoPP::SecByteBlock ciphertext( ecl );

	encryptor.Encrypt( rng, plaintext, plaintext.size(), ciphertext );

	memcpy(&cipherSpace[index * CIPHER_SIZE], ciphertext.data(), ciphertext.size());
	uint8_t* res = reinterpret_cast<uint8_t*>(ciphertext.data());
	return *res;
}

extern "C" uint8_t ByteDecrypt (uint8_t pixel_val, int index) {
	CryptoPP::SecByteBlock ciphertext( CIPHER_SIZE );
	memcpy( ciphertext, &cipherSpace[index * CIPHER_SIZE], CIPHER_SIZE );	

	// Now that there is a concrete object, we can check sizes
	assert( 0 != decryptor.FixedCiphertextLength() );
	assert( ciphertext.size() <= decryptor.FixedCiphertextLength() );

	// Create recovered text space
	size_t dpl = decryptor.MaxPlaintextLength( ciphertext.size() );
	assert( 0 != dpl );
	CryptoPP::SecByteBlock recovered( dpl );

	CryptoPP::DecodingResult result = decryptor.Decrypt( rng, ciphertext, ciphertext.size(), recovered );

	// More sanity checks
	assert( result.isValidCoding );        
	assert( result.messageLength <= decryptor.MaxPlaintextLength( ciphertext.size() ) );

	// At this point, we can set the size of the recovered
	// data. Until decryption occurs (successfully), we
	// only know its maximum size
	recovered.resize( result.messageLength );

	uint8_t* res = reinterpret_cast<uint8_t*>(recovered.data());
	return *res;
}


HalideExtern_2 (uint8_t, ByteEncrypt, uint8_t, int);
HalideExtern_2 (uint8_t, ByteDecrypt, uint8_t, int);

int main(int argc, char **argv) {

	std::cout << "Generating RSA public and private keys..." << std::endl;

	////////////////////////////////////////////////
	// Generate keys
	CryptoPP::InvertibleRSAFunction params;

	params.GenerateRandomWithKeySize(rng, 1536);

	CryptoPP::RSA::PrivateKey privateKey(params);
	CryptoPP::RSA::PublicKey publicKey(params);

	std::cout << "Encryption..." << std::endl;
	std::string encryption_schedule = "serial";
	std::cout << "Enter the schedule for encryption:\n-row\n-vectorized_row\n-tile\n-vectorized_tile\n-vectorized\n-serial" << std::endl;
	std::cin >> encryption_schedule;

	////////////////////////////////////////////////
	// Encryptor machine from the pubic key
	encryptor = CryptoPP::RSAES_OAEP_SHA_Encryptor ( publicKey );
	
	////////////////////////////////////////////////
	// Load input image
	std::cout << "loading image..." << std::endl;
	Halide::Buffer<uint8_t> input = Halide::Tools::load_image("images/rgb.png");

	//cipher space allocation
	cipherSpace = new char[input.height() * input.width() * input.channels() * CIPHER_SIZE];
	int width = input.width();
	int depth = input.channels();

	Halide::Var x, y, c;
	Halide::Var x_outer,x_inner,y_outer,y_inner,tile_index;
	std::cout << "creating halide encryption pipeline..." << std::endl;
	Halide::Func Image_Encryptor;
	Image_Encryptor(x, y, c) = ByteEncrypt(input(x, y, c), (y * width + x) * depth + c)  ;

	std::cout << "encrypting image..." << std::endl;

	////////////////////////////////////////////////
	// Schedule
	////////////////////////////////////////////////
	// Choose the schedule here among
	// * "row"
	// * "vectorized row"
	// * "tile"
	// * "vectorized tile"
	if(encryption_schedule=="tile")
	{
		Image_Encryptor.tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64);
	  	Image_Encryptor.fuse(x_outer, y_outer, tile_index);
	  	Image_Encryptor.parallel(tile_index);
	}
	else if(encryption_schedule=="vectorized_tile")
	{
		Image_Encryptor.tile(x,y,x_outer,y_outer,x_inner,y_inner,64,64);
		Image_Encryptor.fuse(x_outer, y_outer, tile_index);
		Image_Encryptor.parallel(tile_index);
		//Tile again and vectorize
		Halide::Var x_inside_out,y_inside_out,x_vector,y_pairs;
		Image_Encryptor.tile(x_inner, y_inner, x_inside_out, y_inside_out, x_vector, y_pairs, 4, 2);
		Image_Encryptor.vectorize(x_vector);
		Image_Encryptor.unroll(y_pairs);
	}
	else if(encryption_schedule=="row")
	{
		Image_Encryptor.parallel(y);
	}
	else if(encryption_schedule=="vectorized_row")
	{
		Image_Encryptor.parallel(y);
		Image_Encryptor.vectorize(x,4);
	}
	else if(encryption_schedule=="vectorized")
	{
		Image_Encryptor.vectorize(x, 8);
	}
	else
	{
	}

	////////////////////////////////////////////////
	// Encrypt
	double startTime = CycleTimer::currentSeconds();
	Halide::Buffer<uint8_t> encrypted_image = Image_Encryptor.realize(input.width(), input.height(), input.channels());
	double endTime = CycleTimer::currentSeconds();

	std::cout << "image encrypted ! check the output directory to see the cipher" << std::endl;
	std::cout << "[serial encryption]:\t\t[" << (endTime - startTime) << "] s\n" << std::endl;

	Halide::Tools::save_image(encrypted_image, "out/cipher.png");

	std::cout << "Decryption..." << std::endl;
	std::string decryption_schedule = "serial";
	std::cout << "Enter the schedule for decryption:\n-row\n-vectorized_row\n-tile\n-vectorized_tile\n-vectorized\n-serial" << std::endl;
	std::cin >> decryption_schedule;

	////////////////////////////////////////////////
	// Decryptor machine from the private key (trapdoor)
	decryptor = CryptoPP::RSAES_OAEP_SHA_Decryptor ( privateKey );

	std::cout << "creating halide decryption pipeline..." << std::endl;
	Halide::Func Image_Decryptor;
	Image_Decryptor(x, y, c) = ByteDecrypt(encrypted_image(x, y, c),(y * width + x) * depth + c);

	std::cout << "decrypting image..." << std::endl;

	////////////////////////////////////////////////
	// Schedule
	////////////////////////////////////////////////
	// Choose the schedule here among
	// * "row"
	// * "vectorized row"
	// * "tile"
	// * "vectorized tile"
	if(decryption_schedule=="tile")
	{
		Image_Decryptor.tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64);
	  	Image_Decryptor.fuse(x_outer, y_outer, tile_index);
	  	Image_Decryptor.parallel(tile_index);
	}
	else if(decryption_schedule=="vectorized_tile")
	{
		Image_Decryptor.tile(x,y,x_outer,y_outer,x_inner,y_inner,64,64);
		Image_Decryptor.fuse(x_outer, y_outer, tile_index);
		Image_Decryptor.parallel(tile_index);
		//Tile again and vectorize
		Halide::Var x_inside_out,y_inside_out,x_vector,y_pairs;
		Image_Decryptor.tile(x_inner, y_inner, x_inside_out, y_inside_out, x_vector, y_pairs, 4, 2);
		Image_Decryptor.vectorize(x_vector);
		Image_Decryptor.unroll(y_pairs);
	}
	else if(decryption_schedule=="row")
	{
		Image_Decryptor.parallel(y);
	}
	else if(decryption_schedule=="vectorized_row")
	{
		Image_Decryptor.parallel(y);
		Image_Decryptor.vectorize(x,4);
	}
	else if(decryption_schedule=="vectorized")
	{
		Image_Decryptor.vectorize(x, 8);
	}
	else
	{
	}


	////////////////////////////////////////////////
	// Decrypt
	startTime = CycleTimer::currentSeconds();
	Halide::Buffer<uint8_t> decrypted_image = Image_Decryptor.realize(encrypted_image.width(), encrypted_image.height(), encrypted_image.channels());
	endTime = CycleTimer::currentSeconds();
	
	std::cout << "image decrypted ! check the output directory to see the recovered plain text" << std::endl;
	std::cout << "[serial decryption]:\t\t[" << (endTime - startTime) << "] s\n" << std::endl;

	Halide::Tools::save_image(decrypted_image, "out/recovered.png");

	delete [] cipherSpace;
	return 0;
}
