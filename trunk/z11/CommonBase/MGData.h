/*
 * =====================================================================================
 *
 *       Filename:  MGData.h
 *
 *    Description:  提供基础的数据拷贝功能
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

#ifndef _MGDATA_H_
#define _MGDATA_H_
#pragma once
#include "Memory/MGRefObject.h"
#include <string>

namespace z11
{
	class MGData : public MGRefObject
	{
	public:
		MGData();
		virtual ~MGData();
		
		/**
		 * \brief 从指定字节块复制构造MGData
		 * \param bytes 字节块地址
		 * \param length 字节块大小
		 * \return 返回构造生成MGData
		 */
		static MGData *dataWithBytes(const void *bytes,size_t length);
		static MGData *dataWithData(const MGData* data);
		
		/**
		 * \brief 从指定字节块直接构造MGData
		 * \param bytes字节块地址
		 * \param length 字节块大小
		 * \return 返回构造生成MGData
		 */
		static MGData *dataWithBytesNoCopy(void *bytes,size_t length);
		static MGData *dataWithBytesNoCopy(void *bytes,size_t length,bool freeWhenDone);

		/**
		* \brief 从指定字节块直接构造MGData
		* \param bytes字节块地址
		* \param length 字节块大小
		* \return 返回构造生成MGData,其中如果这个MGData已经初始化过的话，则不删除以前的内存块
		*/
		MGData *initWithBytesNoCopy(void *bytes,size_t length);
		/**
		 * \brief 从指定路径文件读取数据来构造MGData
		 * \param path 需要读取数据的文件路径
		 * \return 返回构造生成MGData
		 */
		static MGData *dataWithContentsOfFile(const std::string &path);
		
		/**
		 * \brief	获取MGData的数据块, (不能被修改)
		 */
		const void *bytes() const;
		
		/**
		 * \brief 获取MGData的长度, (不能被修改)
		 * \return MGData的长度
		 */
		virtual size_t length() const;

		bool writeToFile(const std::string& path);
	protected:
		void *m_bytes;
		size_t m_capacity;
		bool m_freeWhenDone;
	};
}

#endif //_MGDATA_H_
