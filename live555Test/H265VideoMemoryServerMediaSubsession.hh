/*
 * H265VideoMemoryServerMediaSubsession.hh
 *
 *  Created on: 2018年8月23日
 *      Author: fangwei
 */

#ifndef H265VIDEOMEMORYSERVERMEDIASUBSESSION_HH_
#define H265VIDEOMEMORYSERVERMEDIASUBSESSION_HH_

#ifndef MEMORYSERVERMEDIASUBSESSION_HH_
#include "MemoryServerMediaSubsession.hh"
#endif

class H265VideoMemoryServerMediaSubsession: public MemoryServerMediaSubsession {
public:
  static H265VideoMemoryServerMediaSubsession*
  createNew(UsageEnvironment& env, const unsigned char* videoData,
			const unsigned long videoDataSize, Boolean reuseFirstSource);

  // Used to implement "getAuxSDPLine()":
  void checkForAuxSDPLine1();
  void afterPlayingDummy1();

protected:
  H265VideoMemoryServerMediaSubsession(UsageEnvironment& env,
		  const unsigned char* videoData,
		  				const unsigned long videoDataSize, Boolean reuseFirstSource);
      // called only by createNew();
  virtual ~H265VideoMemoryServerMediaSubsession();

  void setDoneFlag() { fDoneFlag = ~0; }

protected: // redefined virtual functions
  virtual char const* getAuxSDPLine(RTPSink* rtpSink,
				    FramedSource* inputSource);
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
					      unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
				    FramedSource* inputSource);

private:
  char* fAuxSDPLine;
  char fDoneFlag; // used when setting up "fAuxSDPLine"
  RTPSink* fDummyRTPSink; // ditto
};



#endif /* H265VIDEOMEMORYSERVERMEDIASUBSESSION_HH_ */
