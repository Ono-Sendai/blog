/*=====================================================================
Post.h
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


#include <TimeStamp.h>
#include <UnsafeString.h>
#include <string>
class OutStream;
class InStream;


/*=====================================================================
Post
-------------------

=====================================================================*/
class Post
{
public:
	Post();
	~Post();

	void writeToStream(OutStream& s);
	void readFromStream(InStream& s);

	web::TimeStamp timestamp;

	web::UnsafeString title;
	web::UnsafeString content;
	bool published;
};
