#include "MGDictionary.h"
#include "../System/Shared/MGLog.h"
namespace z11
{
    MGArray::MGArray(void)
    {
        
    }
    
    MGArray::~MGArray(void)
    {
        
    }
    
    MGArray* MGArray::arrayWithObjectsForKeys(std::deque<MGObject *> &objects)
    {
        MGArray* ret = new MGArray();
        ret->values = objects;
        return ret;
    }
    
    uint_32 MGArray::objectCount()
    {
        return values.size();
    }
    
    MGObject* MGArray::objectAtIndex(uint_32 idx)
    {
        MGObject* ret = NULL;
        if (idx < values.size()) {
            ret = values.at(idx);
        }
        return ret;
    }
    
	MGDictionary::MGDictionary(void)
	{
	}

	MGDictionary::~MGDictionary(void)
	{
		//for (MapIterator it=keys2values.begin();it!=keys2values.end();it++)
		//{
		//	delete_and_set_null(it->second);
		//}
	}

	MGDictionary * MGDictionary::dictionaryWithObjectsForKeys( std::vector<MGObject * > &objects, std::vector<MGObject *>& keys )
	{
		MGDictionary *ret=new MGDictionary();
		for (size_t i=0;i<keys.size();i++)
		{
			MGString *str=dynamic_cast<MGString *>(keys[i]);
			assert(str!=NULL);
			ret->keys2values[str->toSTLString()]=objects[i];
		}
		return ret;
	}

	MGObject * MGDictionary::objectForKey( const MGObject *key )
	{
		MGObject* ret = NULL;
		const MGString *str=dynamic_cast<const MGString *>(key);
		assert(str!=NULL);
		MapIterator it=keys2values.find(str->toSTLString());
		if (it!=keys2values.end())
		{
			ret = it->second;
		}
		return ret;
	}

}