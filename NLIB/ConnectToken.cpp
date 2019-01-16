#include "ConnectToken.h"

#include <iostream>
#include <string>

#include "cryptopp/cryptlib.h"
#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "cryptopp/base64.h"

void ConnectToken::test()
{
	unsigned char key[CryptoPP::AES::MAX_KEYLENGTH];
	unsigned char iv[CryptoPP::AES::MAX_BLOCKSIZE];

	std::string plaintext = "Now is the time for all good men to come to the aide...";
	std::string ciphertext;
	std::string decryptedtext;

	//
	// Dump Plain Text
	//
	std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
	std::cout << plaintext;
	std::cout << std::endl << std::endl;

	//
	// Create Cipher Text
	//
	CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::MAX_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
	stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length() + 1);
	stfEncryptor.MessageEnd();

	//
	// Dump Cipher Text
	//
	std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

	for (uint32_t i = 0; i < ciphertext.size(); i++) {

		std::cout << "0x" << std::hex << (0xFF & static_cast<byte>(ciphertext[i])) << " ";
	}

	std::cout << std::endl << std::endl;

	//
	// Decrypt
	//
	CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::MAX_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

	CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
	stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
	stfDecryptor.MessageEnd();

	//
	// Dump Decrypted Text
	//
	std::cout << "Decrypted Text: " << std::endl;
	std::cout << decryptedtext;
	std::cout << std::endl << std::endl;
}

ConnectToken::ConnectToken()
{

}

ConnectToken::~ConnectToken()
{

}

void ConnectToken::Write(ByteStream& stream)
{
	_token = NLIB_CONNECT_TOKEN;

	stream.Write(_token, NLIB_CONNECT_TOKEN_LENGTH);
}

E_READ_RESULT ConnectToken::Read(ByteStream& stream)
{
	NLIB_STREAM_READ_BYTE(_token, NLIB_CONNECT_TOKEN_LENGTH);
	NLIB_VALIDATE(memcmp(_token, NLIB_CONNECT_TOKEN, NLIB_CONNECT_TOKEN_LENGTH) == 0, E_READ_RESULT::INVALID_TOKEN);

	return E_READ_RESULT::SUCCESS;
}

uint32_t ConnectToken::Length()
{
	return NLIB_CONNECT_TOKEN_LENGTH;
}