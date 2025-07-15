/*=====================================================================
Page.h
------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#pragma once


#include <TimeStamp.h>
#include <UnsafeString.h>
#include <string>
class OutStream;
class InStream;


/*=====================================================================
Page
-------------------

=====================================================================*/
class Page
{
public:
	Page();
	~Page();

	void writeToStream(OutStream& s);
	void readFromStream(InStream& s);

	TimeStamp created_timestamp;
	TimeStamp last_modified_timestamp;

	web::UnsafeString url_title;
	web::UnsafeString title;
	web::UnsafeString content;
	bool published;
};
