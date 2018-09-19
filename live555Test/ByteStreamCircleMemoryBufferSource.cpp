/*
 * ByteStreamCircleMemoryBufferSource.cpp
 *
 *  Created on: 2018年8月23日
 *      Author: fangwei
 */
#include "ByteStreamCircleMemoryBufferSource.hh"
#include "GroupsockHelper.hh"
#include "sharedData.hh"
////////// ByteStreamCircleMemoryBufferSource //////////

ByteStreamCircleMemoryBufferSource*
ByteStreamCircleMemoryBufferSource::createNew(UsageEnvironment& env,
					u_int8_t* buffer, u_int64_t bufferSize,
					Boolean deleteBufferOnClose,
					unsigned preferredFrameSize,
					unsigned playTimePerFrame) {
  if (buffer == NULL) return NULL;

  return new ByteStreamCircleMemoryBufferSource(env, buffer, bufferSize, deleteBufferOnClose, preferredFrameSize, playTimePerFrame);
}

ByteStreamCircleMemoryBufferSource::ByteStreamCircleMemoryBufferSource(UsageEnvironment& env,
							   u_int8_t* buffer, u_int64_t bufferSize,
							   Boolean deleteBufferOnClose,
							   unsigned preferredFrameSize,
							   unsigned playTimePerFrame)
  : FramedSource(env), fBuffer(buffer), fBufferSize(bufferSize), fCurIndex(0), fDeleteBufferOnClose(deleteBufferOnClose),
    fPreferredFrameSize(preferredFrameSize), fPlayTimePerFrame(playTimePerFrame), fLastPlayTime(0),
    fLimitNumBytesToStream(False), fNumBytesToStream(0) {
}

ByteStreamCircleMemoryBufferSource::~ByteStreamCircleMemoryBufferSource() {
  if (fDeleteBufferOnClose) delete[] fBuffer;
}

void ByteStreamCircleMemoryBufferSource::seekToByteAbsolute(u_int64_t byteNumber, u_int64_t numBytesToStream) {
  fCurIndex = byteNumber;
  if (fCurIndex > fBufferSize) fCurIndex = fBufferSize;

  fNumBytesToStream = numBytesToStream;
  fLimitNumBytesToStream = fNumBytesToStream > 0;
}

void ByteStreamCircleMemoryBufferSource::seekToByteRelative(int64_t offset, u_int64_t numBytesToStream) {
  int64_t newIndex = fCurIndex + offset;
  if (newIndex < 0) {
    fCurIndex = 0;
  } else {
    fCurIndex = (u_int64_t)offset;
    if (fCurIndex > fBufferSize) fCurIndex = fBufferSize;
  }

  fNumBytesToStream = numBytesToStream;
  fLimitNumBytesToStream = fNumBytesToStream > 0;
}

void ByteStreamCircleMemoryBufferSource::doGetNextFrame() {
  if (fLimitNumBytesToStream && fNumBytesToStream == 0) {
    handleClosure();
    return;
  }

  //If fCurIndex >= fBufferSize, read next data from circle
  if(fCurIndex >= fBufferSize)
  {
	  dataMutex.lock();
	  if(fBuffer!=NULL)
	  	delete []fBuffer;


	  int tempPos = posR, sumSize = 0;
	  for(int i=0; i<publishNum; i++)
	  {
		  sumSize = sumSize + buffersSize[tempPos];
		  tempPos = nextPos(tempPos);
	  }
	  fBuffer = new unsigned char[sumSize];

	  tempPos = posR;
	  fBufferSize = 0;
	  for(int i=0; i<publishNum; i++)
	  {
		  memcpy(fBuffer+fBufferSize,buffers[tempPos],buffersSize[tempPos]*sizeof(unsigned char));
		  fBufferSize = fBufferSize + buffersSize[tempPos];
		  tempPos = nextPos(tempPos);
	  }
	  fBufferSize = sumSize;

	  fCurIndex = 0;
	  posR = tempPos;
	  dataMutex.unlock();

//	  dataMutex.lock();
//	  if(fBuffer!=NULL)
//	  	free(fBuffer);
//	  fBufferSize = buffersSize[posR];
//	  fBuffer = new unsigned char[fBufferSize];
//	  memcpy(fBuffer,buffers[posR],buffersSize[posR]*sizeof(unsigned char));
//	  fCurIndex = 0;
//	  toNextPos(posR);
//	  printf("                                posR=%d\n",posR);
//	  dataMutex.unlock();
  }


  // Try to read as many bytes as will fit in the buffer provided (or "fPreferredFrameSize" if less)
  fFrameSize = fMaxSize;
  if (fLimitNumBytesToStream && fNumBytesToStream < (u_int64_t)fFrameSize) {
    fFrameSize = (unsigned)fNumBytesToStream;
  }
  if (fPreferredFrameSize > 0 && fPreferredFrameSize < fFrameSize) {
    fFrameSize = fPreferredFrameSize;
  }

  if (fCurIndex + fFrameSize > fBufferSize) {
    fFrameSize = (unsigned)(fBufferSize - fCurIndex);
  }

  memmove(fTo, &fBuffer[fCurIndex], fFrameSize);
  fCurIndex += fFrameSize;
  fNumBytesToStream -= fFrameSize;

  // Set the 'presentation time':
  if (fPlayTimePerFrame > 0 && fPreferredFrameSize > 0) {
    if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
      // This is the first frame, so use the current time:
      gettimeofday(&fPresentationTime, NULL);
    } else {
      // Increment by the play time of the previous data:
      unsigned uSeconds	= fPresentationTime.tv_usec + fLastPlayTime;
      fPresentationTime.tv_sec += uSeconds/1000000;
      fPresentationTime.tv_usec = uSeconds%1000000;
    }

    // Remember the play time of this data:
    fLastPlayTime = (fPlayTimePerFrame*fFrameSize)/fPreferredFrameSize;
    fDurationInMicroseconds = fLastPlayTime;
  } else {
    // We don't know a specific play time duration for this data,
    // so just record the current time as being the 'presentation time':
    gettimeofday(&fPresentationTime, NULL);
  }

  // Inform the downstream object that it has data:
  FramedSource::afterGetting(this);
}



