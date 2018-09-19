#include "sharedData.hh"

unsigned char* buffers[ringNum];
unsigned long buffersSize[ringNum];
unsigned long reallyBuffersSize[ringNum];
int posR, posW;
std::mutex dataMutex;
void toNextPos(int &pos)
{
	pos++;
	if(pos==ringNum)
		pos = 0;
}

int nextPos(const int pos)
{
	if(pos==ringNum-1)
		return 0;
	else return (pos+1);
}
