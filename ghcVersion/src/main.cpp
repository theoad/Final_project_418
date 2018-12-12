#include <iostream>
#include "Halide.h"
#include "halide_image_io.h"
#include "rsa.h"
#include "cryptlib.h"
#include "osrng.h"
int main(int argc, char **argv) {

	////////////////////////////////////////////////
	// Generate keys
	CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 3072);

	CryptoPP::RSA::PrivateKey privateKey(params);
	CryptoPP::RSA::PublicKey publicKey(params);

	////////////////////////////////////////////////
	// Load input image
	Halide::Buffer<unsigned char> input = Halide::Tools::load_image("images/rgb.png");
	Halide::Buffer<unsigned char> output;
	Halide::Var x, y, c, plain, cipher;
	Halide::Func Image_Encryptor;

	////////////////////////////////////////////////
	// Encryption
	CryptoPP::RSAES_OAEP_SHA_Encryptor e(publicKey);

	Image_Encryptor(plain, cipher, y) = CryptoPP::ss1(reinterpret_cast<unsigned char*>(plain(0, y, 0)), plain.witdh()*plain.channels(), 
		true, new PK_EncryptorFilter(rng, e, new StringSink(reinterpret_cast<unsigned char*>(cipher(0, y, 0))))); 

	Image_Encryptor.realize(input, output, input.height());

	Halide::Tools::save_image(output, "out/encrypt.png");

	// string plain="RSA Encryption", cipher, recovered;

	// ////////////////////////////////////////////////
	// // Encryption
	// RSAES_OAEP_SHA_Encryptor e(publicKey);

	// StringSource ss1(plain, true,
	//     new PK_EncryptorFilter(rng, e,
	//         new StringSink(cipher)
	//    ) // PK_EncryptorFilter
	// ); // StringSource

	// ////////////////////////////////////////////////
	// // Decryption
	// RSAES_OAEP_SHA_Decryptor d(privateKey);

	// StringSource ss2(cipher, true,
	//     new PK_DecryptorFilter(rng, d,
	//         new StringSink(recovered)
	//    ) // PK_DecryptorFilter
	// ); // StringSource

	// cout << "Recovered plain text" << endl;

	
	

	// Encrypt_Machine(x, y, c) = value; //defining

	// //debug:
	// Encrypt_Machine.trace_stores();  //tell halide to print on each evaluation when running
	// Encrypt_Machine.print_loop_nest(); //tell halide to print pseudo code of how the loops are arranged 
	// //print, print_when

	// //scheduling primitive:
	// Encrypt_Machine.parallel(y);  //row are computed in parallel with task queue. 
	// 							  //Can control the nb of threads using HL_NUM_THREADS
	// Encrypt_Machine.reorder(y, x, c); //first argument = innermost loop 

	// Halide::Buffer<uint8_t> output = Encrypt_Machine.realize(input.width(), input.height(), input.channels());

	// Halide::Func gradient("gradient"); //defining a stage
	// Halide::Var x("x"), y("y"); //defining variables
	// gradient (x, y) = x + y; //creating the stage
	// Halide::Buffer<int32_t> output = gradient.realize(8, 8); //realising
	// gradient.compile_to_lowered_stmt("gradient.html", {}, Halide::HTML);
	return 0;
}
