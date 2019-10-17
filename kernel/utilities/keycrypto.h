//----------------------------------------------------------
// Name.........: KeyCrypto class
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Provides a wrapper for crypto functions, keys, validation, verifications
//			saving, generating, loading, and reading of encrypted files.
// Comments.....: Before altering this class, read up on Crypto++ and cryptography. 
// 			Documentation is sparse so be careful.
// Revisions....:
//===========================================================

#ifndef __KEYCRYPTO_H__
#define __KEYCRYPTO_H__

#include <cstdlib>
#include <cstdio>
#include <string>
#include <keyczar/keyczar.h>
#include <keyczar/keyczar_tool/keyczar_tool.h>
#include <keyczar/rw/keyset_file_reader.h>

#include <cassert>
/*
//Crypto++ libraries.
#include "base64.h" //Base64 encoding/decoding
#include "hex.h" //Hex encoding/decoding
#include "rsa.h" //RSA keys, encoding and decoding.
#include "cryptlib.h" //Interface for Crypto++ (also includes ByteQueues, BufferedTransformations and SecByteBlock).
#include "files.h" //FileSink, FileSource.
#include "osrng.h" //Secure BGs
#include "pssr.h" //PSSR
#include "filters.h" //SignerFilter, SignerVerificationFilter
#include "sha.h" //SHA
*/


//RSA encryption level.
#define RSA_MODULUS 4096
//Source file for public keys.
#define DEC_KEY_PATH "./dec"
//Source file for private keys.
#define ENC_KEY_PATH "./enc"

//Only needed for the generator program.
#ifdef GENERATOR
//PBE password for the keys.
#define KEY_PASSWORD "A2SD5Ngri234s5ofAWDPdFQFWFgErgG6g6hdH73nty82WQFhrgdrte9wfeD8j9gh47ht3df"
#endif
//Destination file for the encrypted license.
#define LICENSE_FILE "license.txt"


class KeyCrypto {

	
	private:
/*
	void		SaveDER(const std::string& filename, const CryptoPP::BufferedTransformation& bt);	
	void		SaveBase64(const std::string& filename, const CryptoPP::BufferedTransformation& bt);
	void		SaveHex(const std::string& filename, const CryptoPP::BufferedTransformation& bt);
	void		Save(const std::string& filename, const CryptoPP::BufferedTransformation& bt);
	void		SavePrivateKey(const std::string& filename, const CryptoPP::RSA::PrivateKey& key, std::string base="");
	void		SavePublicKey(const std::string& filename, const CryptoPP::RSA::PublicKey& key, std::string base="");


	void		LoadBER(const std::string& filename, CryptoPP::BufferedTransformation& bt);
	void		LoadBase64(const std::string& filename, CryptoPP::BufferedTransformation& bt);
	void		LoadHex(const std::string& filename, CryptoPP::BufferedTransformation& bt);
	void		Load(const std::string& filename, CryptoPP::BufferedTransformation& bt);
	void		LoadPrivateKey(const std::string& filename, CryptoPP::RSA::PrivateKey& key, std::string base="");
	void		LoadPublicKey(const std::string& filename, CryptoPP::RSA::PublicKey& key, std::string base="");
*/
	std::string	EncryptRSA(std::string& plaintext);
	std::string	DecryptRSA(std::string& cipher);

	public:


	void		WriteToLicense(std::string& message, std::string destination);
	std::string	ReadFromLicense(std::string source);

	void		GeneratePublicKey();
	void		GeneratePrivateKey();
	void		GenerateKeyPair();
};
#endif
