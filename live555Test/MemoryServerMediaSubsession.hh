/*
 * MemoryServerMediaSubsession.hh
 *
 *  Created on: 2018年8月23日
 *      Author: fangwei
 */

#ifndef MEMORYSERVERMEDIASUBSESSION_HH_
#define MEMORYSERVERMEDIASUBSESSION_HH_

#ifndef _SERVER_MEDIA_SESSION_HH
#include "ServerMediaSession.hh"
#endif

#ifndef _ON_DEMAND_SERVER_MEDIA_SUBSESSION_HH
#include "OnDemandServerMediaSubsession.hh"
#endif

class MemoryServerMediaSubsession: public OnDemandServerMediaSubsession {
protected: // we're a virtual base class
	MemoryServerMediaSubsession(UsageEnvironment& env, const unsigned char* videoData,
				const unsigned long videoDataSize,
			    Boolean reuseFirstSource);
  virtual ~MemoryServerMediaSubsession();

protected:
  unsigned char* buffer;
  unsigned long bufferSize;
};




#endif /* MEMORYSERVERMEDIASUBSESSION_HH_ */
