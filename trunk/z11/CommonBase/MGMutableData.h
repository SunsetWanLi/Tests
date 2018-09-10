/*
 * =====================================================================================
 *
 *       Filename:  MGMutableData.h
 *
 *    Description:  提供可变的data管理 
 *
 *        Version:  1.0
 *        Created:  02/04/2012 01:46:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#ifndef _MG_MUTABLE_DATA_H_
#define _MG_MUTABLE_DATA_H_
#pragma once
#include "MGData.h"
#include "MGStructure.h"
namespace z11
{
	class MGMutableData : public MGData
	{
	public:
		static MGMutableData *dataWithCapacity(size_t aNumItems);

		void increaseLengthBy(size_t extraLength);
		void setLength(size_t length);

		void *mutableBytes();

		void appendBytes(const void *_bytes,size_t length);
		void appendData(const MGData *otherData);
		virtual size_t length() const;

		void replaceBytesInRangeWithBytes(const MGRange &range,const void *replace_bytes);
	private:
		byte_8 * alloc(size_t size);
		MGMutableData();
		size_t m_length;
	};
}
#endif //_MG_MUTABLE_DATA_H_
