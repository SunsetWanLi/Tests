#include "CodedInputStreamInstance.h"
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
struct FileInfo
{
	FileInfo()
	{
		buffer=NULL;
		size=0;
	}
	FileInfo(unsigned char *buffer,size_t size)
	{
		this->buffer=buffer;
		this->size=size;
	}
	unsigned char *buffer;
	size_t size;
};
std::map<int,FileInfo> mInputStreamMap;
CodedInputStreamInstance::CodedInputStreamInstance(void)
{
}


CodedInputStreamInstance::~CodedInputStreamInstance(void)
{
}

SHARED_PTR<google::protobuf::io::CodedInputStream> CodedInputStreamInstance::getInstance(int fileInd)
{
	if (mInputStreamMap.find(fileInd)==mInputStreamMap.end())
	{
		lseek(fileInd, 0, SEEK_SET);
		struct stat stat;
		fstat(fileInd, &stat);
		unsigned char *buffer = new unsigned char[stat.st_size];
		size_t read_bytes=read(fileInd,buffer,stat.st_size);
		assert(read_bytes==stat.st_size);
		mInputStreamMap[fileInd]=FileInfo(buffer,stat.st_size);
	}
	SHARED_PTR<google::protobuf::io::CodedInputStream> ret = SHARED_PTR<google::protobuf::io::CodedInputStream>(new google::protobuf::io::CodedInputStream(mInputStreamMap[fileInd].buffer,mInputStreamMap[fileInd].size));
	ret->SetTotalBytesLimit(mInputStreamMap[fileInd].size,mInputStreamMap[fileInd].size>>1);
	return ret;
}