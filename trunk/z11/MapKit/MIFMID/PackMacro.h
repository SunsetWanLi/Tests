#pragma once
#ifdef WINCE
#define MY_UNALIGNED UNALIGNED
#else
#define MY_UNALIGNED
#endif

#define PACK_SIMPLE_VAL(ptr, val)\
	memcpy(ptr, &val, sizeof(val));	\
	ptr += sizeof(val);
#define PACK_INT_32(ptr, val)\
	*(int_32 MY_UNALIGNED*)ptr = val;\
	ptr += sizeof(int_32);
#define PACK_INT_64(ptr,val)\
	*(int_64 MY_UNALIGNED*)ptr = val;\
	ptr += sizeof(int_64);
#define PACK_GO_ID(ptr, val) PACK_INT_64(ptr, val)
#define PACK_DOUBLE(ptr, val)\
	*(double MY_UNALIGNED*)ptr = val;\
	ptr += sizeof(double);
#define PACK_PTR(target_ptr, value_ptr, size)\
	{\
	*(int_32 MY_UNALIGNED*)target_ptr = size;\
	target_ptr += sizeof(int_32);\
	memcpy(target_ptr, value_ptr, size);\
	target_ptr += size;\
	}

//-----------------------------------	

#define UNPACK_SIMPLE_VAL(ptr, val)\
	memcpy(&val, ptr, sizeof(val));\
	ptr += sizeof(val);
//#endif //UNPACK_SIMPLE_MEMBER
#define UNPACK_GO_ID(ptr, val)\
	val = *((GO_ID MY_UNALIGNED*)ptr);\
	ptr += sizeof(GO_ID);

#define UNPACK_INT_32(ptr, val)\
	val = *((int_32 MY_UNALIGNED*)ptr);\
	ptr += sizeof(int_32);
#define UNPACK_DOUBLE(ptr, val)\
	val = *((double MY_UNALIGNED*)ptr);\
	ptr += sizeof(double);
	
#define UNPACK_FLOAT(ptr, val)\
	val += *((float*)ptr);\
	ptr += sizeof(float);
	
#define UNPACK_PTR(target_ptr, src_ptr, size)\
	memcpy(target_ptr, src_ptr, size);\
	src_ptr += size;
