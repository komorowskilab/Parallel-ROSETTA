#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/stat.h> //For stat

#include <dirent.h> //for struct dirent

#include <kernel/utilities/keycrypto.h>
#include <stdexcept> //For runtime_error

using namespace keyczar;


//----------------------------------------------------------------
// Name.........: FileExists
// Author.......: Nicholas Baltzer
// Date.........: 08/10/13
// Description..: Helper for checking key files. Returns true if a file exists, false otherwise.
// Comments.....: 
// Revisions....: 
//================================================================

bool FileExists(const std::string& filename)
{
    struct stat buf;
//    if (stat(filename.c_str(), &buf) != -1)
//  {
//        return true;
//    }
//    return false;
	return (stat(filename.c_str(), &buf) != -1);
}

//----------------------------------------------------------------
// Name.........: DirectoryExists
// Author.......: Nicholas Baltzer
// Date.........: 19/11/14
// Description..: Helper for checking key dirs. Returns true if a dir exists, false otherwise.
// Comments.....: Grabbed in entirety from 
// 			http://stackoverflow.com/questions/4980815/c-determining-if-directory-not-a-file-exists-in-linux
// Revisions....: 
//================================================================

bool DirectoryExists( const char* pzPath )
{
    if ( pzPath == NULL) return false;

    DIR *pDir;
    bool bExists = false;

    pDir = opendir (pzPath);

    if (pDir != NULL)
    {
        bExists = true;    
        (void) closedir (pDir);
    }

    return bExists;
}






//----------------------------------------------------------------
// Name.........: SaveHex
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Root for Saving Hex encrypted keys. Provides the actual hex encoding, writing, and allocation
//			cleaning.
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::SaveHex(const std::string& filename, const BufferedTransformation& bt) {

	//Create an encoder.
        HexEncoder encoder;

	//Copy the data into the encoder.
        bt.CopyTo(encoder);
	//And close the encoder (also causes it to execute encoding on the data).
        encoder.MessageEnd();

	//And then, save the data.
	Save(filename, encoder); 

}
*/
//----------------------------------------------------------------
// Name.........: SaveBase64
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Root for Saving Base64 encrypted keys. Provides the actual Base64 encoding, writing, and allocation
//			cleaning.
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::SaveBase64(const std::string& filename, const BufferedTransformation& bt) {

	//Create the encoder.
	Base64Encoder encoder;
	
	//Move the data into it.
	bt.CopyTo(encoder);
	//Close the encoder (causing it to encode the data).
	encoder.MessageEnd();

	//And save it.
	Save(filename, encoder);

}
*/


//----------------------------------------------------------------
// Name.........: Save
// Author.......: Nicholas Baltzer
// Date.........: 30/09/13
// Description..: Saver. Takes a buffer and writes it to file in a cryptographically secure fashion. 
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::Save(const std::string& filename, const BufferedTransformation& bt) {
	
	//Create an output stream.
	FileSink file(filename.c_str());
	//Copy the byte buffer to the output stream.
	bt.CopyTo(file);
	//And close the output.
	file.MessageEnd();
}

*/
//----------------------------------------------------------------
// Name.........: SavePrivateKey
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Interface for saving private keys. Takes a filename to svae to, the key to be saved,
//			and the chosen encoding to save it. Calls the other save functions.
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::SavePrivateKey(const std::string& filename, const RSA::PrivateKey& key, std::string base) {


	ByteQueue queue;
	//Using Hex encoding...
	if (base == "HEX") {
		//Save the key data as a byte buffer.
		key.Save(queue);
		//And save the buffer after hex encoding it.
		SaveHex(filename, queue);
	}
	//Using Base64 encoding...
	else if (base == "BASE64") {
		//Save the data to a byte buffer.
		key.Save(queue);
		//And save the buffer after encoding it.
		SaveBase64(filename, queue);
	}
	//If using DEREncoding...
	else if (base == "DER" || base == "BER") {
		//Use the existing function to save the key to a byte buffer and then encode the buffer.
		key.DEREncodePrivateKey(queue);
		//And then save te buffer.
		Save(filename, queue);
	}
	//If no encoding is used...
	else {
		//Save the key to a byte buffer.
		key.Save(queue);
		//Then save the buffer.
		Save(filename, queue);
	}
}
*/
//----------------------------------------------------------------
// Name.........: SavePublicKey
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Interface for saving public keys. Takes a filename to svae to, the key to be saved,
//			and the chosen encoding to save it. Calls the other save functions.
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::SavePublicKey(const std::string& filename, const RSA::PublicKey& key, std::string base) {

	ByteQueue queue;
	//Using Hex encoding...
	if (base == "HEX") {
		//Save the key data as a byte buffer.
		key.Save(queue);
		//And save the buffer after hex encoding it.
		SaveHex(filename, queue);
	}
	//Using Base64 encoding...
	else if (base == "BASE64") {
		//Save the data to a byte buffer.
		key.Save(queue);
		//And save the buffer after encoding it.
		SaveBase64(filename, queue);
	}
	//If using DEREncoding...
	else if (base == "DER" || base == "BER") {
		//Use the existing function to save the key to a byte buffer and then encode the buffer.
		key.DEREncodePublicKey(queue);
		//And then save te buffer.
		Save(filename, queue);
	}
	//If no encoding is used...
	else {
		//Save the key to a byte buffer.
		key.Save(queue);
		//Then save the buffer.
		Save(filename, queue);
	}
}
*/
//----------------------------------------------------------------
// Name.........: LoadPrivateKey
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Interface for loading private keys. Takes a filename to load from, the key to be loaded,
//			and a decoding algorithm to use prior to loading it. Calls the other load functions.
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::LoadPrivateKey(const std::string& filename, RSA::PrivateKey& key, std::string base) {

	//Create the buffer we'll get the final contents from,.
	ByteQueue queue;

	//If the encoding is HEX...0
	if (base == "HEX") {
		//Load the file and HexDecode it.
		LoadHex(filename, queue);
		//Apply the data as a key.
		key.Load(queue);
	} 
	//If the encoding is Base64...
	else if (base == "BASE64") {
		//Decode the file with Base64...
		LoadBase64(filename, queue);
		//And load it as a key.
		key.Load(queue);
	}
	//If the data is DER encoded...
	else if (base == "DER" ||  base == "BER") {
		//Load the data straight off...
		Load(filename, queue);
		//And use the existing BER decode function burrowed in the Keyloading sequence.
		key.BERDecodePrivateKey(queue, false /*optParams*///, queue.MaxRetrievable());
/*
	}
	//If the data is not encoded...
	else {
		//Load it up.
		Load(filename, queue);
		//And apply it as a key.
		key.Load(queue);
	}
}
*/
//----------------------------------------------------------------
// Name.........: LoadPublicKey
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Interface for loading public keys. Takes a filename to load from, the key to be loaded,
//			and a decoding algorithm to use prior to loading it. Calls the other load functions.
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::LoadPublicKey(const std::string& filename, RSA::PublicKey& key, std::string base) {
	//Create the buffer we'll get the final contents from,.
	ByteQueue queue;

	//If the encoding is HEX...
	if (base == "HEX") {
		//Load the file and HexDecode it.
		LoadHex(filename, queue);
		//Apply the data as a key.
		key.Load(queue);
	} 
	//If the encoding is Base64...
	else if (base == "BASE64") {
		//Decode the file with Base64...
		LoadBase64(filename, queue);
		//And load it as a key.
		key.Load(queue);
	}
	//If the data is DER encoded...
	else if (base == "DER" ||  base == "BER") {
		//Load the data straight off...
		Load(filename, queue);
		//And use the existing BER decode function burrowed in the Keyloading sequence.
		key.BERDecodePublicKey(queue, false /*optParams*///, queue.MaxRetrievable());
/*
	}
	//If the data is not encoded...
	else {
		//Load it up.
		Load(filename, queue);
		//And apply it as a key.
		key.Load(queue);
	}

}
*/

//----------------------------------------------------------------
// Name.........: LoadHex
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Root for the LoadHex*** functions. Provides the actual decoding and file loading.
//			
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::LoadHex(const std::string& filename, BufferedTransformation& bt) {

	//Create an input stream, and load it fully after decoding the file with a Hex algorithm.
	FileSource file(filename.c_str(), true /*pumpAll*///, new HexDecoder());
	//Move the decoded data to a byte buffer.
/*
	file.TransferTo(bt);
	//Close the buffer.
	bt.MessageEnd();
        
}
*/
//----------------------------------------------------------------
// Name.........: LoadBase64
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Root for the LoadBase64*** functions. Provides the actual decoding and file loading.
//			
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::LoadBase64(const std::string& filename, BufferedTransformation& bt) {
	//Create an input stream, load the file, and decode the file contents using
	//Base64 before outputting it.
	FileSource file(filename.c_str(), true, new Base64Decoder());
	//Move the decoded data to the byte buffer.
	file.TransferTo(bt);
	//Close the buffer.
	bt.MessageEnd();

}
*/

//----------------------------------------------------------------
// Name.........: Load
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Root for Load functions. Loads a byte buffer without applying a decode algorithm. Used for
//			non-encoded data or data with an inherent decode (such as DER/BER).
// Comments.....: 
// Revisions....: 
//================================================================
/*
void
KeyCrypto::Load(const std::string& filename, BufferedTransformation& bt) {

	//Create an input stream, load the entire file into it.
	FileSource file(filename.c_str(), true /*pumpAll*///);
	//Move the data from the input stream to a byte buffer.
/*
	file.TransferTo(bt);
	//And close the buffer.
	bt.MessageEnd();

}
*/

//----------------------------------------------------------------
// Name.........: EncryptRSA
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Takes a string, encodes it with RSA using the keyfile source and returns the cipher generated.
//			
// Comments.....: 
// Revisions....: 08/13/13 -N
//			Added a Signing / Verification scheme with PSS/SHA1. Now Signing and encoding is
//			handled with the Private Key.
//		  03/11/14 -N
//		  	Switched from Crypto++ to Keyczar. Everything is remade with the new API.
//================================================================

std::string
KeyCrypto::EncryptRSA(std::string& plaintext) {
#ifdef GENERATOR
	scoped_ptr<rw::KeysetReader> reader(new rw::KeysetPBEJSONFileReader(ENC_KEY_PATH, KEY_PASSWORD));

//	Keyczar* kcSigner = keyczar::Crypter::Read(DEC_KEY_PATH);
//	Keyczar* kcSigner = Signer::Read(ENC_KEY_PATH);


	Keyczar* kcSigner = Signer::Read(*reader);
//	Keyczar* kcSigner = Crypter::Read(*reader);

//	DIR* dir = opendir(ENC_KEY_PATH);
//	struct dirent* pdir;
	
//	if (dir != NULL) {
//		while (pdir = readdir(dir)) {
//			std::cout << "entry " << pdir->d_name << std::endl;
//		}
//	}

//	kcSigner->set_encoding(Keyczar::NO_ENCODING);


	if (!kcSigner) {
		std::cerr<< "Signer failed to load. Exiting..." << std::endl;
		exit(17);	
	}

	std::string sSignature, sAttachedMessage;
	
	bool bResult = kcSigner->AttachedSign(plaintext, plaintext, &sSignature);
//	bool bResult = kcSigner->Encrypt(plaintext, &sSignature);

	if (bResult) {
		bResult = kcSigner->AttachedVerify(sSignature, plaintext, &sAttachedMessage);
		assert(bResult);
	}

//	std::cout << sAttachedMessage << std::endl;

	delete kcSigner;

	return sSignature;
#else
	return plaintext;
#endif
}


//----------------------------------------------------------------
// Name.........: DecryptRSA
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Takes an encrypted string and decodes it using the KEY_FILE source.
// Comments.....: 
// Revisions....: 08/10/13 -N
//			Added Signing / Verification scheme with PSS/SHA1. Now Verification and decoding is handled
//			by the Public Key. This way a new public key cannot be generated by the user (who previously would
//			need access to the Private Key for the decryption to work).
//		  03/11/14 -N
//		  	Switched from Crypto++ to Keyczar. Everything is remade with the new API.
//================================================================
std::string
KeyCrypto::DecryptRSA(std::string& cipher) {

	//Read a password protected keyset.
//	scoped_ptr<rw::KeysetReader> reader(new rw::KeysetPBEJSONFileReader(DEC_KEY_PATH, KEY_PASSWORD));
	
	Keyczar* kcVerifier = Verifier::Read(DEC_KEY_PATH);	
	//Load that keyset.
//	Keyczar* kcVerifier = Verifier::Read(*reader);	
	


//	kcVerifier->set_encoding(Keyczar::NO_ENCODING);

	//Verify that it works.
	if (!kcVerifier){
		std::cerr << "Verifier failed to load. Exiting..." << std::endl;
		exit(16);
	}
	
	std::string sAttached, sPlainText;
	std::string sCipher = cipher;

	kcVerifier->GetAttachedWithoutVerify(sCipher, &sAttached);

	bool bResult = kcVerifier->AttachedVerify(sCipher, sAttached, &sPlainText);
	if (!bResult) {
		std::cerr << "Could not verify license. Exiting..." << std::endl;
		exit (116);
	}
	assert(bResult);
//	bool bResult = kcVerifier->Decrypt(sCipher, &sPlainText);
//	std::cout << sCipher << std::endl;
//	std::cout << sPlainText << std::endl;
	

	return sPlainText;
}



//----------------------------------------------------------------
// Name.........: WriteToLicense
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Writes message to destination in a secure, non-ascii fashion.
// Comments.....: 
// Revisions....: 03/11/14 -N
//		  Switched from Crypto++ to Keyczar. Everything is remade with the new API.
//		  Also, now encrypts the license before writing it, prompting the removal
//		  of EncryptRSA as a public function.
//================================================================
void
KeyCrypto::WriteToLicense(std::string& message, std::string destination) {

	std::ofstream ofLicense(destination.c_str());
	if (ofLicense.good()) {
		ofLicense << EncryptRSA(message);
	}
	ofLicense.close();

} 

//----------------------------------------------------------------
// Name.........: ReadFromLicense
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Reads a file and returns it as a string in a secure non-ascii fashion.
// Comments.....: 
// Revisions....: 03/11/14 -N
//		  Switched from Crypto++ to Keyczar. Everything is remade with the new API.
//		  Also, now decrypts the license before returning it, prompting the removal
//		  of DecryptRSA as a public function.
//================================================================
std::string
KeyCrypto::ReadFromLicense(std::string source) {


	std::ifstream ifLicense(source.c_str());
	std::stringstream ssLicense;
	std::string sLine;

	if (ifLicense.good()) {
		while (getline(ifLicense, sLine)) {
			ssLicense << sLine;
		}
	}

	ifLicense.close();
	std::string sOut(ssLicense.str());
	return DecryptRSA(sOut);
}
//----------------------------------------------------------------
// Name.........: GenerateKeyPair
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Wrapper for generating private and public keypairs. Both keys will be written to file.
// Comments.....: 
// Revisions....: 11/11/14 -N
// 			Switched crypto from Crypto++ to Keyczar. New API, everything remade.
//		  19/11/14 -N
//		  	Going with encrypted keys (PBE) and a sign/verify approach. The license data
//		  	will be passed as the attached message.
//================================================================
void 
KeyCrypto::GenerateKeyPair() {
//Only keep this stuff in the generator program.
#ifdef GENERATOR
	
	bool bKeyExists = true;
	int nKeyName = 1;

	//Check if there is already a keyset for signing.
	std::string sDir = "enc/";
	//Does the directory exist already?
	if (!DirectoryExists(sDir.c_str())) {
		//If not, create it.
		mkdir(sDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
	else {
		
		//Does the dir hold a keychain?
		if (FileExists(sDir + "meta")) {
			//Then remove it.
			remove((sDir + "meta").c_str());
		}
		//Go through any keys in the dir. By convention they are named 1 .. n.
		while (bKeyExists) {
			//If a key exists...
			if (FileExists((sDir + std::to_string(nKeyName)).c_str())) {
				//Delete it.
				remove((sDir + std::to_string(nKeyName)).c_str());
				//Move on the next.
				++nKeyName;
			}
			else {	
				bKeyExists = false;
			}
		}
	}

	nKeyName = 1;
	bKeyExists = true;

	//Check if there is already a keyset for verifying.
	sDir = "dec/";
	//Does the directory exist already?
	if (!DirectoryExists(sDir.c_str())) {
		//If not, create it.
		mkdir(sDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
	else {
		//Does the dir hold a keychain?
		if (FileExists((sDir + "meta").c_str())) {
			//Then remove it.
			remove((sDir + "meta").c_str());
		}
		//Go through any keys in the dir. By convention they are named 1 .. n.
		while (bKeyExists) {
			//If a key exists...
			if (FileExists((sDir + std::to_string(nKeyName)).c_str())) {
				//Delete it.
				remove((sDir + std::to_string(nKeyName)).c_str());
				//Move on the next.
				++nKeyName;
			}
			else {	
				bKeyExists = false;
			}
		}
	}

	//Build a tool.
	scoped_ptr<keyczar::keyczar_tool::KeyczarTool> kzTool(new keyczar::keyczar_tool::KeyczarTool(keyczar::keyczar_tool::KeyczarTool::JSON_FILE));
	//Build an encryption.
	const keyczar::keyczar_tool::KeyczarTool::KeyEncryption keEncryption = keyczar::keyczar_tool::KeyczarTool::PBE;
	

	if (kzTool.get() == NULL) {

		std::cerr << "Failed to init KeyGenerator. Exiting..." << std::endl;
		exit(11);
	}
	//Build a keyset.
	bool bSuccess = kzTool->CmdCreate(
		ENC_KEY_PATH,					//Where the keyset will be stored.
		keyczar::KeyPurpose::SIGN_AND_VERIFY, 		//Purpose of the keyset.
		"KeyGenRosetta",				//Name. Kinda pointless.
		keyczar::keyczar_tool::KeyczarTool::RSA); 	//Type of encryption.

	if (!bSuccess) {

		std::cerr << "Failed to create keyset. Exiting..." << std::endl;
		exit(12);
	}
	//Build a key.
	const int nKeyVersion = kzTool->CmdAddKey(
		ENC_KEY_PATH,		//Path to the dir where the encoding key shall be
		keyczar::KeyStatus::ACTIVE,	//Key status.
		RSA_MODULUS,		//Key size (in this case 4096 bits)
		keEncryption,		//Type of encryption for the key.
		KEY_PASSWORD);		//Password to be used for that encryption.

	if (nKeyVersion <= 0) {

		std::cerr << "Failed to generate encoding key. Exiting..." << std::endl;
		exit(13);
	}
	//Promote that key (as a test).
	bSuccess = kzTool->CmdPromote(
		ENC_KEY_PATH,		//Promoting the key to primary.
		nKeyVersion);		//Match the created key.

	if (!bSuccess) {

		std::cerr << "Failed to promote encoding key. Exiting..." << std::endl;
		exit(14);
	}
	//Build a public key for verifying.
	bSuccess = kzTool->CmdPubKey(
		ENC_KEY_PATH,		//Source for the public key.
		DEC_KEY_PATH,		//Destination of the public key.
		keEncryption,		//Encryption type for the key.
		KEY_PASSWORD);		//Password for that encryption.
	
	if (!bSuccess) {

		std::cerr << "Failed to generate decoding key. Exiting..." << std::endl;
		exit(15);
	}
	
#endif
}

//----------------------------------------------------------------
// Name.........: GeneratePublicKey
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Generates a public key from the private key in GEN_KEY source and saves it to KEY_FILE.
//			The key is validated before writing.
// Comments.....: 
// Revisions....: 
//================================================================
void
KeyCrypto::GeneratePublicKey() {
/*
	
	RSA::PrivateKey privateKey;
	//Secure generator.
	AutoSeededRandomPool secGen;
	//Load the private key.
	LoadPrivateKey(GEN_KEY, privateKey, ENCODING);
	//From the private key, generate a public key.
	RSA::PublicKey publicKey(privateKey);
	//Validate the new key.
	if (!publicKey.Validate(secGen, 3))
		throw std::runtime_error("Validation of public key failed");
	//Save the key.
	SavePublicKey(KEY_FILE, publicKey, ENCODING);
*/
}


//----------------------------------------------------------------
// Name.........: GeneratePrivateKey
// Author.......: Nicholas Baltzer
// Date.........: 25/09/13
// Description..: Generates a private key and saves it to GEN_KEY. The key is validated before being written.
// Comments.....: 
// Revisions....: 
//================================================================
void
KeyCrypto::GeneratePrivateKey() {
/*
	//Get a secure generator.
	AutoSeededRandomPool secGen;
	RSA::PrivateKey privateKey;
	
	//Generate a private key using the secure generator and athe modulus (which determines the size of the key).
	//The modulus is defined in the header.
	privateKey.GenerateRandomWithKeySize(secGen, RSA_MODULUS);

	//Validate the key. 
	if (!privateKey.Validate(secGen, 3))
		throw std::runtime_error("Validation of private key failed");
	//Save the key to the path defined by GEN_KEY (set in header) after encoding it with ENCODING (set in header).
	SavePrivateKey(GEN_KEY, privateKey, ENCODING);
*/
}
