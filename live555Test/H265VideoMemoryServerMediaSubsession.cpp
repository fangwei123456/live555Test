#include "H265VideoMemoryServerMediaSubsession.hh"


#include "H265VideoRTPSink.hh"
#include "ByteStreamCircleMemoryBufferSource.hh"
#include "H265VideoStreamFramer.hh"

H265VideoMemoryServerMediaSubsession*
H265VideoMemoryServerMediaSubsession::createNew(UsageEnvironment& env,
		const unsigned char* videoData,
						const unsigned long videoDataSize,
					      Boolean reuseFirstSource) {
  return new H265VideoMemoryServerMediaSubsession(env, videoData,
			videoDataSize, reuseFirstSource);
}

H265VideoMemoryServerMediaSubsession::H265VideoMemoryServerMediaSubsession(UsageEnvironment& env,
		const unsigned char* videoData,
						const unsigned long videoDataSize, Boolean reuseFirstSource)
  : MemoryServerMediaSubsession(env, videoData,
			videoDataSize, reuseFirstSource),
    fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL) {
}

H265VideoMemoryServerMediaSubsession::~H265VideoMemoryServerMediaSubsession() {
  delete[] fAuxSDPLine;
}

static void afterPlayingDummy(void* clientData) {

  H265VideoMemoryServerMediaSubsession* subsess = (H265VideoMemoryServerMediaSubsession*)clientData;
  subsess->afterPlayingDummy1();
}

void H265VideoMemoryServerMediaSubsession::afterPlayingDummy1() {
  // Unschedule any pending 'checking' task:
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
  // Signal the event loop that we're done:
  setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData) {
  H265VideoMemoryServerMediaSubsession* subsess = (H265VideoMemoryServerMediaSubsession*)clientData;
  subsess->checkForAuxSDPLine1();
}

void H265VideoMemoryServerMediaSubsession::checkForAuxSDPLine1() {
  nextTask() = NULL;

  char const* dasl;
  if (fAuxSDPLine != NULL) {
    // Signal the event loop that we're done:
    setDoneFlag();
  } else if (fDummyRTPSink != NULL && (dasl = fDummyRTPSink->auxSDPLine()) != NULL) {
    fAuxSDPLine = strDup(dasl);
    fDummyRTPSink = NULL;

    // Signal the event loop that we're done:
    setDoneFlag();
  } else if (!fDoneFlag) {
    // try again after a brief delay:
    int uSecsToDelay = 100000; // 100 ms
    nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)checkForAuxSDPLine, this);
  }
}

char const* H265VideoMemoryServerMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
  if (fAuxSDPLine != NULL) return fAuxSDPLine; // it's already been set up (for a previous client)

  if (fDummyRTPSink == NULL) { // we're not already setting it up for another, concurrent stream
    // Note: For H265 video files, the 'config' information (used for several payload-format
    // specific parameters in the SDP description) isn't known until we start reading the file.
    // This means that "rtpSink"s "auxSDPLine()" will be NULL initially,
    // and we need to start reading data from our file until this changes.
    fDummyRTPSink = rtpSink;

    // Start reading the file:
    fDummyRTPSink->startPlaying(*inputSource, afterPlayingDummy, this);

    // Check whether the sink's 'auxSDPLine()' is ready:
    checkForAuxSDPLine(this);
  }

  envir().taskScheduler().doEventLoop(&fDoneFlag);

  return fAuxSDPLine;
}

FramedSource* H265VideoMemoryServerMediaSubsession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = 500; // kbps, estimate

  // Create the video source:
  ByteStreamCircleMemoryBufferSource* memorySource = ByteStreamCircleMemoryBufferSource::createNew(envir(), buffer, bufferSize, false);
  if (buffer == NULL) return NULL;


  // Create a framer for the Video Elementary Stream:
  return H265VideoStreamFramer::createNew(envir(), memorySource);
}

RTPSink* H265VideoMemoryServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
  return H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}
