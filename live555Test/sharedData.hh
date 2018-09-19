#ifndef SHARE_DATA_HH_
#define SHARE_DATA_HH_
#include <mutex>

#define ringNum 16
#define publishNum 6//frames published once

void toNextPos(int &pos);
int nextPos(const int pos);
extern unsigned char* buffers[ringNum];
extern unsigned long buffersSize[ringNum];
extern unsigned long reallyBuffersSize[ringNum];
extern int posR, posW;
extern std::mutex dataMutex;
#endif
