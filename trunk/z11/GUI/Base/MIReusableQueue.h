/*
 * =====================================================================================
 *
 *       Filename:  MIReusableQueue.h
 *
 *    Description: maintain a queue for reuse
 *
 *        Version:  1.0
 *        Created:  06/09/2012 02:57:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include <string>
#include <map>
#include "System/Shared/Syncable.h"
#include "System/Shared/MGTime.h"
namespace z11
{
	class MIReusableCell
	{
	public:
		virtual ~MIReusableCell()
		{

		}
		virtual void prepareForReuse()
		{

		}
		virtual void enqueForReuse()
		{

		}
		int_64 reuseable_life;
		std::string m_reuse_identifier;
	};
	class MIReusableQueueBasedAccessTime:public Syncable
	{
	public:
		MIReusableQueueBasedAccessTime();
		MIReusableCell *dequeueReusableCellWithIdentifier(const std::string &identifier);
		void collection();
		void enqueue(MIReusableCell *cell);
		void enqueueReusableCell(MIReusableCell *cell);
		MIReusableCell *contain(const std::string &identify);
		void clear();
		void destory();
	private:
		std::map<std::string,MIReusableCell *>  reusable_cell_map;
		std::map<int_64 ,MIReusableCell *> reuse_life_map;
		std::map<std::string,MIReusableCell *>  cell_map;
		typedef std::map<std::string,MIReusableCell *>::iterator String2CellIterator ;
		typedef std::map<int_64,MIReusableCell *>::iterator Int2CellIterator ;
		int_64 getReuseLife();
		size_t max_cache_cell_size;
        int_64 time_value;
	};
};