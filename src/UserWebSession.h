/*=====================================================================
UserWebSession.h
----------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


#include "TimeStamp.h"
#include <ThreadSafeRefCounted.h>
#include <Reference.h>
#include <OutStream.h>
#include <InStream.h>


/*=====================================================================
UserWebSession
--------------
A logged-in web session.
Looked up by a cookie which stores the ID.
=====================================================================*/
class UserWebSession : public ThreadSafeRefCounted
{
public:
	UserWebSession();
	~UserWebSession();

	static std::string generateRandomKey(); // throws glare::Exception on failure

	std::string id;

	int user_index;

	web::TimeStamp created_time;
};


typedef Reference<UserWebSession> UserWebSessionRef;
