/*=====================================================================
Page.cpp
--------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "Page.h"


#include <InStream.h>
#include <OutStream.h>
#include "WebsiteExcep.h"
#include <mysocket.h>
#include <Lock.h>
#include <Clock.h>
#include <StringUtils.h>
#include <PlatformUtils.h>
#include <KillThreadMessage.h>
#include <Parser.h>
#include <MemMappedFile.h>


Page::Page()
:	published(false)
{

}


Page::~Page()
{

}


static const unsigned int PAGE_SERIALISATION_VERSION = 1;


void Page::writeToStream(OutStream& s)
{
	s.writeUInt32(PAGE_SERIALISATION_VERSION);

	created_timestamp.writeToStream(s);
	last_modified_timestamp.writeToStream(s);

	s.writeStringLengthFirst(url_title.str());
	s.writeStringLengthFirst(title.str());
	s.writeStringLengthFirst(content.str());

	s.writeUInt32(published ? 1 : 0);
}


void Page::readFromStream(InStream& stream)
{
	const uint32 v = stream.readUInt32();
	if(v > PAGE_SERIALISATION_VERSION)
		throw web::WebsiteExcep("Unknown version " + toString(v) + ", expected " + toString(PAGE_SERIALISATION_VERSION) + ".");

	created_timestamp.readFromStream(stream);
	last_modified_timestamp.readFromStream(stream);

	url_title = stream.readStringLengthFirst(/*max string length=*/100000000);
	title = stream.readStringLengthFirst(/*max string length=*/100000000);
	content = stream.readStringLengthFirst(/*max string length=*/100000000);

	published = stream.readUInt32() != 0;
}
