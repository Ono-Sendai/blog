/*=====================================================================
User.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


#include <string>
#include <vector>
class OutStream;
class InStream;


/*=====================================================================
User
-------------------

=====================================================================*/
class User
{
public:
	User();
	~User();

	// Return digest
	static std::vector<unsigned char> computePasswordHash(const std::string& password, const std::string& salt);

	bool isPasswordValid(const std::string& password) const;

	void writeToStream(OutStream& s);
	void readFromStream(InStream& s);

	std::string name;
	std::string email_address;
	std::vector<unsigned char> hashed_password;
	std::string password_hash_salt;

private:

};
