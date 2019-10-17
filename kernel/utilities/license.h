#ifndef  __LICENSE_H__
#define __LICENSE_H__


/** \mainpage Module for licence checking.
 * \section intro_sec Introduction
 *
 * This is basic module for license checking. It might be a 
 * little bit crude but works and it's not the main concern in
 * development of Rosetta.
 *
 *  */

#include <kernel/system/iostream.h>
#include <kernel/system/fstream.h>
#include <kernel/system/string.h>
#include <kernel/system/sstream.h>
#include <kernel/system/ctype.h>
#include <kernel/basic/vector.h>
#include <kernel/basic/algorithm.h>
#include <kernel/utilities/generator.h>
#include <kernel/utilities/keycrypto.h>

#ifdef WIN32
	#include <windows.h>
	#include <winreg.h>
	#include <atltime.h>
#endif

#ifdef LINUX
	#include <iomanip>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <arpa/inet.h>
	#include <time.h>
#endif 

/** Standard namespace.
 */
using namespace std;

/** Class dealing with copy protection and licenses.
 * Might be split in two classes. Provides only basic protection. We are 
 * assuming that non-techie should not be able to break it.
 * 
 * \author Lukasz Ligowski
 * \warning test version
 *  24/11/14 -N
 *  	Added GetFirstDeviceName(). Should make things ALOT easier with NIC ids.
 */
class License {
private:
	
	/** Built in key for Rosetta's copy (rosetta->license_gen)
	 * I might rethink it few times whether I want to give each
	 * copy of Rosetta different key or each should use the same key
	 */
	static const long int main_key = 666666666;

	/** Key used (license_gen->rosetta)
	 */
	static const long int main_server_key = 11;

/** @name ASCII armoring variables.
 */
//@{
	/** Number of ASCII characters used to encode encrypted messages. 
	 * 95 means that all non command standard ASCII will be used.
	 * \sa shift
	 */
	static const int base = 40;
	/** Encoded character width. */
	static const int base_exp = 5;	

	/** Number of first noncommand ASCII character. 
	 * \sa base
	 */
	static const int shift = 48;
//@}

public:
	/** Default location of license file for Rosetta. 
	 * Relative to working directory.
	 */
	static const string licensefile; 

	/** Default location of machine id file for Rosetta. 
	 * Relative to working directory.
	 */
	static const string machineidfile;

	/** License generating utility input file location. 
	 */
	static const string licin;

	/** Generates unique (hopefully) id. This procedure has two versions one for each platform. Each
	 * version is activated by proper defines.
	 * On MS Windows it locks to windows serial number. 
	 * On Linux it uses MAC addres of eth0.
	 * \param key key to encrypt
	 * \return machine id string
	 * \sa CheckLicense(), GenerateLicense() 
	 */
	static string GenerateId(long int key = main_key);
	static string ReadIdFile(string location = licensefile);
	// -N. Gamechanger. 
	static string GetFirstDeviceName();
	
/** @name License manipulation routines.
 */
//@{
	
	/** Rosetta should use it every start. It should contain decrypting
 	 * license, checking for files with forward date and so on...
	 * \param id encrypted machine id provided by GenerateId()
	 * \param k key to decrypt
	 * \return obvious
	 * \sa GenerateId(), GenerateLicense()
 	 */
	static bool CheckLicense(const string id, long int k = main_server_key, long int key2 = main_key);

	static bool CheckLicense();

#ifdef GENERATOR
	/** This should be used by utility program for generating licenses.
 	 * Full of crap so far. Should be split in two. One 
	 * generating output and one reading data and feeding
	 * it to the first one.
         * \param filename name of configuration input file
	 * \param key key to encrypt
	 * \return nothing, writes result to a file
	 * \sa GenerateId(), CheckLicense()
 	 */
	static void GenerateLicense(const string filename = licin, long int key = main_server_key, long int key2 = main_key);
//@}
	
	static string GenerateLicense(string subject, string machineid, int day, int month, int year, int lday, int lmonth, int lyear, long int key = main_server_key, long int key2 = main_key);

	// -N.
	static void GenerateKeys();
#endif

/** @name Encryption routines.
 * Implements crypto operations by adding pseudorandom numbers to a string.
 * \sa Generator::Get()
 */
//@{	
	/** Implements decryption, do not know yet which algorithm will be chosen. 
	 * \param key key
	 * \param enc encrypted text
	 * \return decrypted string
	 * \sa Encrypt()
	 */
	static string Decrypt(long int key, const string enc);

	/** Implements encryption, do not know yet which algorithm will be chosen.
	 * \param key key
	 * \param dec decrypted text 
	 * \return encrypted string
	 * \sa Decrypt()
	 */
	static string Encrypt(long int key, const string dec);
//@}

	/** Fast check to see if a file exists or not. 
	* Uses an open() return statement to access the filesystem.
	*/
	static bool FileExists(const string&);

};

#endif

