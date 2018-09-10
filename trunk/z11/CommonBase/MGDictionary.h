/**
 * @file MGDictionary.h
 * @brief MGDictionary 提供数据字典
 * @author Gu Rongfang
 * @version 0.1
 * @date 2012-02-04
 */
#ifndef _MGDICTIONARY_H_
#define _MGDICTIONARY_H_
#pragma once
#include <vector>
#include <deque>
#include <string>
#include <map>
#include "MGObject.h"
#include "MGString.h"
namespace z11
{
    class MGArray : public MGObject
    {
    public:
        ~MGArray(void);
        
        static MGArray *arrayWithObjectsForKeys(std::deque<MGObject * > &objects); //retain
        uint_32 objectCount();
        MGObject *objectAtIndex(uint_32 idx);
    private:
        MGArray(void);
        std::deque<MGObject *> values;
        typedef std::deque<MGObject *>::iterator DequeIterator ;
    };
    
	class MGDictionary:MGObject
	{
	public:
		~MGDictionary(void);
		
		/**
		 * \brief 根据指定的key-value对构造MGDictionary
		 * \param objects	values数组
		 * \param keys		keys数组
		 * \return 返回构造MGDictionary 
		 */
		static MGDictionary *dictionaryWithObjectsForKeys(std::vector<MGObject * > &objects,std::vector<MGObject *>& keys); //retain
		
		/**
		 * \brief 根据指定的key，retrive 相应的value
		 * \param key 指定的key
		 * \return 相应的value
		 */
		MGObject *objectForKey(const MGObject *key);
	private:
		MGDictionary(void);
		std::map<std::string,MGObject *> keys2values;
		typedef std::map<std::string,MGObject *>::iterator MapIterator ;
	};


}

#endif //_MGDICTIONARY_H_
