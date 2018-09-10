#pragma once
#include "../native/common.h"
#include "google/protobuf/io/coded_stream.h"
class CodedInputStreamInstance
{
public:
	static SHARED_PTR<google::protobuf::io::CodedInputStream> getInstance(int fileInd);
private:
	CodedInputStreamInstance(void);
	~CodedInputStreamInstance(void);
};

