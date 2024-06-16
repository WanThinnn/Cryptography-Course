// g++ -g3 -ggdb -O0 -DDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread
// g++ -g -O2 -DNDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread

// header part
#ifdef _WIN32
#include <windows.h>
#endif

#include <cstdlib>
#include <locale>
#include <cctype>

// cryptopp
#include "cryptopp\osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include "cryptopp\cryptlib.h"
using CryptoPP::Exception;

#include "cryptopp\hex.h"
using CryptoPP::HexDecoder;
using CryptoPP::HexEncoder;

#include "cryptopp\base64.h"
using CryptoPP::Base64Decoder;
using CryptoPP::Base64Encoder;

#include "cryptopp\filters.h"
using CryptoPP::StreamTransformationFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;

#include "cryptopp\des.h"
using CryptoPP::DES;

#include "cryptopp\modes.h"
using CryptoPP::CBC_Mode;

#include "cryptopp\secblock.h"
using CryptoPP::SecByteBlock;

// cpp

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <cstdlib>
using std::exit;

// main

int main(int argc, char *argv[])
{
	// main part
	// Set locale to support UTF-8
	#ifdef __linux__
	std::locale::global(std::locale("C.utf8"));
	#endif

	#ifdef _WIN32
	// Set console code page to UTF-8 on Windows C.utf8, CP_UTF8
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	#endif
	AutoSeededRandomPool prng;

	SecByteBlock key(DES::DEFAULT_KEYLENGTH);
	prng.GenerateBlock(key, key.size());

	CryptoPP::byte iv[DES::BLOCKSIZE];
	prng.GenerateBlock(iv, sizeof(iv));

	string plain = "22521385 - Lai Quan Thien - Lab 1 - Task 2";
	string cipher, encoded, recovered;

	/*********************************\
	\*********************************/

	// cout << "key length: " << DES::DEFAULT_KEYLENGTH << endl;
	// cout << "block size: " << DES::BLOCKSIZE << endl;

	// Pretty print key
	encoded.clear();
	StringSource(key, key.size(), true,
				 new HexEncoder(
					 new StringSink(encoded)) // HexEncoder
	);										  // StringSource
	cout << "key(hex): " << encoded << endl;

	// Pretty print iv
	encoded.clear();
	StringSource(iv, sizeof(iv), true,
				 new HexEncoder(
					 new StringSink(encoded)) // HexEncoder
	);										  // StringSource
	cout << "iv(hex): " << encoded << endl;

	// Pretty print key
	encoded.clear();
	StringSource(key, key.size(), true,
				 new Base64Encoder(
					 new StringSink(encoded)) // Base64Encoder
	);										  // StringSource
	cout << "key(base64): " << encoded << endl;

	// Pretty print iv
	encoded.clear();
	StringSource(iv, sizeof(iv), true,
				 new Base64Encoder(
					 new StringSink(encoded)) // Base64Encoder
	);										  // StringSource
	cout << "iv(base64): " << encoded << endl;

	/*********************************\
	\*********************************/

	try
	{
		cout << "plain text: " << plain << endl;

		CBC_Mode<DES>::Encryption e;
		e.SetKeyWithIV(key, key.size(), iv);

		// The StreamTransformationFilter adds padding
		//  as required. ECB and CBC Mode must be padded
		//  to the block size of the cipher.

		StringSource(plain, true,										   // input of pipeline
					 new StreamTransformationFilter(e,					   // mutator
													new StringSink(cipher) // output of pipeline
													)					   // StreamTransformationFilter
		);																   // StringSource
	}

	catch (const CryptoPP::Exception &e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	/*********************************\
	\*********************************/

	// Pretty print
	encoded.clear();
	StringSource(cipher, true,
				 new HexEncoder(
					 new StringSink(encoded)) // HexEncoder
	);										  // StringSource
	cout << "cipher text(hex): " << encoded << endl;

	// Pretty print
	encoded.clear();
	StringSource(cipher, true,
				 new Base64Encoder(			  // mutator
					 new StringSink(encoded)) // Base64Encoder
	);										  // StringSource
	cout << "cipher text(base64): " << encoded << endl;
	/*********************************\
	\*********************************/

	try
	{
		CBC_Mode<DES>::Decryption d;
		d.SetKeyWithIV(key, key.size(), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(cipher, true,
					   new StreamTransformationFilter(d,
													  new StringSink(recovered)) // StreamTransformationFilter
		);																		 // StringSource

		cout << "recovered text: " << recovered << endl;
	}
	catch (const CryptoPP::Exception &e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	/*********************************\
	\*********************************/

	return 0;
}
