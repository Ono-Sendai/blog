/*=====================================================================
UserWebSession.cpp
------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "UserWebSession.h"


#include <Exception.h>
#include <Base64.h>
#include <CryptoRNG.h>


UserWebSession::UserWebSession()
{}


UserWebSession::~UserWebSession()
{}


std::string UserWebSession::generateRandomKey() // throws glare::Exception on failure
{
	const int NUM_BYTES = 16;
	uint8 data[NUM_BYTES];

	CryptoRNG::getRandomBytes(data, NUM_BYTES); // throws glare::Exception on failure

	std::string key;
	Base64::encode(data, NUM_BYTES, key);
	return key;
}
