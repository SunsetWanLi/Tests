#ifndef _I_BUFFER_H_
#define _I_BUFFER_H_

namespace GIS_Embedded
{

	class IBuffer
	{
	public:
		virtual ~IBuffer() {};
		virtual int availableSize() const = 0;
		virtual int spaceFree(int size) = 0;	//内存替换策略 size表示需要的空间数
	};

}

#endif