/*=====================================================================
User.cpp
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "User.h"


#include <SHA256.h>
#include <InStream.h>
#include <OutStream.h>
#include <StringUtils.h>
#include "WebsiteExcep.h"


User::User()
{

}


User::~User()
{

}


bool User::isPasswordValid(const std::string& password_attempt) const
{
	std::vector<unsigned char> attempt_digest = computePasswordHash(password_attempt, this->password_hash_salt);
	return attempt_digest == this->hashed_password;
}


std::vector<unsigned char> User::computePasswordHash(const std::string& password, const std::string& salt)
{
	const std::string message = "jdfrY%TFkj&Cg&------" + password + "------" + salt;

	std::vector<unsigned char> digest;
	SHA256::hash((const unsigned char*)&(*message.begin()), (const unsigned char*)&(*message.begin()) + message.size(), digest);

	return digest;
}


static const unsigned int USER_SERIALISATION_VERSION = 1;


void User::writeToStream(OutStream& s)
{
	s.writeUInt32(USER_SERIALISATION_VERSION);

	s.writeStringLengthFirst(name);
	s.writeStringLengthFirst(email_address);

	s.writeUInt32((uint32)hashed_password.size());
	s.writeData(hashed_password.data(), hashed_password.size());

	s.writeStringLengthFirst(password_hash_salt);
}


void User::readFromStream(InStream& stream)
{
	const uint32 v = stream.readUInt32();
	if(v > USER_SERIALISATION_VERSION)
		throw web::WebsiteExcep("Unknown version " + toString(v) + ", expected " + toString(USER_SERIALISATION_VERSION) + ".");

	name = stream.readStringLengthFirst(/*max string length=*/10000);
	email_address = stream.readStringLengthFirst(/*max string length=*/10000);

	const uint32 hashed_password_size = stream.readUInt32();
	if(hashed_password_size > 100000)
		throw glare::Exception("Invalid hashed_password_size");
	hashed_password.resize(hashed_password_size);
	stream.readData(hashed_password.data(), hashed_password_size);

	password_hash_salt = stream.readStringLengthFirst(/*max string length=*/10000);
}
