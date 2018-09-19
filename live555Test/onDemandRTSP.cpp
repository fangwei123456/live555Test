/*
 * onDemandRTSP.cpp
 *
 *  Created on: 2018年8月23日
 *      Author: fangwei
 */

#include "onDemandRTSP.hh"


void onDemandRTSP::initRTSP(int portNum, unsigned packetSize)
{
	OutPacketBuffer::maxSize = packetSize;
  // Begin by setting up our usage environment:
  scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server:
  rtspServer = RTSPServer::createNew(*env, portNum, authDB);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    return;
  }


	}
void onDemandRTSP::startRTSP(const unsigned char* initialData, const unsigned long initialDataSize)
{
	  char const* descriptionString
	    = "Session streamed by \"testOnDemandRTSPServer\"";

	  // Set up each of the possible streams that can be served by the
	  // RTSP server.  Each such stream is implemented using a
	  // "ServerMediaSession" object, plus one or more
	  // "ServerMediaSubsession" objects for each audio/video substream.
	sms = ServerMediaSession::createNew(*env, "h265ESVideoTest", "h265ESVideoTest",
				      descriptionString);
    sms->addSubsession(H265VideoMemoryServerMediaSubsession
		       ::createNew(*env, initialData, initialDataSize, false));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, "h265ESVideoTest");



  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
  } else {
    *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return

}

void onDemandRTSP::announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
			   char const* streamName) {
  char* url = rtspServer->rtspURL(sms);
  UsageEnvironment& env = rtspServer->envir();
  env << "\n\"" << streamName <<"\n";
  env << "Play this stream using the URL \"" << url << "\"\n";
  delete[] url;
}

