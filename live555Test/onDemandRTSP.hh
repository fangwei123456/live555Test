/*
 * onDemandRTSP.hh
 *
 *  Created on: 2018年8月23日
 *      Author: fangwei
 */

#ifndef ONDEMANDRTSP_HH_
#define ONDEMANDRTSP_HH_
#include "H265VideoRTPSink.hh"
#include "H265VideoStreamFramer.hh"
#include "H265VideoStreamDiscreteFramer.hh"
#include "H265VideoRTPSource.hh"
#include "H265VideoMemoryServerMediaSubsession.hh"
#include "RTSPServer.hh"
#include "BasicUsageEnvironment.hh"
#include "ByteStreamCircleMemoryBufferSource.hh"

class onDemandRTSP
{
public:
	UsageEnvironment* env;
	TaskScheduler* scheduler;
	UserAuthenticationDatabase* authDB;
	RTSPServer* rtspServer;
	ServerMediaSession* sms;
	void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
				   char const* streamName);
	void initRTSP(int portNum, unsigned packetSize);
	void startRTSP(const unsigned char* initialData, const unsigned long initialDataSize);
};




#endif /* ONDEMANDRTSP_HH_ */
