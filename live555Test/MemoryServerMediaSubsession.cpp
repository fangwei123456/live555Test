#include "MemoryServerMediaSubsession.hh"

MemoryServerMediaSubsession
::MemoryServerMediaSubsession(UsageEnvironment& env, const unsigned char* videoData,
		const unsigned long videoDataSize,
			    Boolean reuseFirstSource)
  : OnDemandServerMediaSubsession(env, reuseFirstSource),
    bufferSize(videoDataSize) {
	buffer = new unsigned char[bufferSize];
	memcpy(buffer,videoData,bufferSize);

}

MemoryServerMediaSubsession::~MemoryServerMediaSubsession() {
	if(buffer!=NULL)
		free(buffer);
}
