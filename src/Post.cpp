/*=====================================================================
Post.cpp
-------------------
Copyright Nicholas Chapman 2023 -
=====================================================================*/
#include "Post.h"


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


Post::Post()
:	published(false)
{

}


Post::~Post()
{

}


static const unsigned int POST_SERIALISATION_VERSION = 2;


void Post::writeToStream(OutStream& s)
{
	s.writeUInt32(POST_SERIALISATION_VERSION);

	timestamp.writeToStream(s);

	s.writeStringLengthFirst(title.str());
	s.writeStringLengthFirst(content.str());

	s.writeUInt32(published ? 1 : 0);
}


void Post::readFromStream(InStream& stream)
{
	const uint32 v = stream.readUInt32();
	if(v > POST_SERIALISATION_VERSION)
		throw web::WebsiteExcep("Unknown version " + toString(v) + ", expected " + toString(POST_SERIALISATION_VERSION) + ".");

	timestamp.readFromStream(stream);

	title = stream.readStringLengthFirst(/*max string length=*/100000000);
	content = stream.readStringLengthFirst(/*max string length=*/100000000);

	if(v < 2)
		published = true;
	else
		published = stream.readUInt32() != 0;
}
