
#include "H265VideoRTPSink.hh"
#include "H265VideoStreamFramer.hh"
#include "H265VideoStreamDiscreteFramer.hh"
#include "H265VideoRTPSource.hh"
#include "H265VideoMemoryServerMediaSubsession.hh"
#include "RTSPServer.hh"
#include "BasicUsageEnvironment.hh"
#include "ByteStreamCircleMemoryBufferSource.hh"
#include "onDemandRTSP.hh"
#include "sharedData.hh"
#include <thread>
#include <windows.h>

int main(int argc, char** argv)
{
	posW = 0;
	posR = 0;
    char const* inputFileName = "test.265";
	onDemandRTSP od;
	od.initRTSP(8555,800000);
	FILE* file = fopen(inputFileName,"rb");


	buffersSize[0] = 5000;
	buffers[0] = new unsigned char[ buffersSize[0] ];
	fread(buffers[0],sizeof(unsigned char),buffersSize[0],file);
	std::thread t1(&onDemandRTSP::startRTSP,od,buffers[0],buffersSize[0]);
	t1.detach();
	while(1)
	{
		Sleep(10);
		dataMutex.lock();
		buffersSize[posW] = 5000;
		if(buffers[posW]!=NULL)
			delete []buffers[posW];
		buffers[posW] = new unsigned char[ buffersSize[posW] ];
		if(fread(buffers[posW],sizeof(unsigned char),buffersSize[posW],file)==0)
		{
			fseek(file,0,SEEK_SET);
		}
		toNextPos(posW);
		printf("posW=%d\n",posW);
		dataMutex.unlock();
	}
  return 0; 
}


