#ifdef __MG_PLATFORM_FILEUTILS_CPP__
#include "MGFileUtils.h"
#include "../CommonBase/MGData.h"
#include "../ExternalLib/support/zip_support/unzip.h"
#include "../System/Shared/MGLog.h"
#include "../System/Shared/Syncable.h"

namespace z11
{
	MGData* MGFileUtils::getFileDataFromZip(const std::string &zip_file_name,const std::string &file_name,size_t offset_pos,size_t length)
	{
		
		assert(offset_pos==0);
		assert(length==npos);

		unsigned char * pBuffer = NULL;
		unzFile pFile = NULL;
		size_t size = 0;

		do 
		{
			MG_BREAK_IF(zip_file_name.empty() || file_name.empty());

			pFile = unzOpen(zip_file_name.c_str());
			MG_BREAK_IF(!pFile);

			int nRet = unzLocateFile(pFile, file_name.c_str(), NULL);
			MG_BREAK_IF(UNZ_OK != nRet);

			char szFilePathA[260];
			unz_file_info FileInfo;
			nRet = unzGetCurrentFileInfo(pFile, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0);
			MG_BREAK_IF(UNZ_OK != nRet);

			size_t file_size=FileInfo.uncompressed_size;
			MG_BREAK_IF (offset_pos>=file_size);
			if (length==npos || length>file_size-offset_pos)
			{
				length=file_size-offset_pos;
			}
			
			nRet = unzOpenCurrentFile(pFile);
			MG_BREAK_IF(UNZ_OK != nRet);

			//nRet = unzSetOffset(pFile,offset_pos);
			//MG_BREAK_IF(UNZ_OK != nRet);

			pBuffer = new unsigned char[length+1];
			pBuffer[length]=0;
			int nSize = 0;
			nSize = unzReadCurrentFile(pFile, pBuffer, (unsigned int)length);
			assert(nSize == 0 || nSize == (int)length);
			//CCAssert(nSize == 0 || nSize == (int)FileInfo.uncompressed_size, "the file size is wrong");

			//size = FileInfo.uncompressed_size;
			size=length;
			unzCloseCurrentFile(pFile);
		} while (0);

		if (pFile)
		{
			unzClose(pFile);
		}
		MGData *data=NULL;
		if (pBuffer!=NULL && size!=0)
		{
			data=MGData::dataWithBytesNoCopy(pBuffer,size);
		}
		
		return data;
	}

	bool MGFileUtils::fileExistsAtPathInZip(const std::string &zip_file_name,const std::string &file_name )
	{
		unzFile pFile = NULL;
		bool  exist = false;

		do 
		{
			MG_BREAK_IF(zip_file_name.empty() || file_name.empty());

			pFile = unzOpen(zip_file_name.c_str());
			MG_BREAK_IF(!pFile);

			int nRet = unzLocateFile(pFile, file_name.c_str(), NULL);
			MG_BREAK_IF(UNZ_OK != nRet);

			exist=true;
		} while (0);

		if (pFile)
		{
			unzClose(pFile);
		}
		return exist;
	}
    
    bool MGFileUtils::fileExistsAtPathInBundle(const std::string &zip_file_name, const std::string &file_name)
	{
		static unzFile pFile = NULL;
        static Syncable apk_file_locker;
		bool  exist = false;
        
        apk_file_locker.lock();
		do
		{
			MG_BREAK_IF(zip_file_name.empty() || file_name.empty());
            
            if (pFile == NULL)
            {
                pFile = unzOpen(zip_file_name.c_str());
            }
			MG_BREAK_IF(!pFile);
            
            if(file_name[file_name.size()-1] == '/')
            {
                int err = unzGoToFirstFile(pFile);
                while (err == UNZ_OK)
                {
                    char file_in_zip[256];
                    err = unzGetCurrentFileInfo64(pFile, NULL, file_in_zip, 255, NULL, 0, NULL, 0);
                    if(string::npos != string(file_in_zip).find(file_name))
                    {
                        exist = true;
                        break;
                    }
                    
                    if (UNZ_OK == err)
                    {
                        err = unzGoToNextFile(pFile);
                    }
                }
            }
            else
            {
                int err = unzLocateFile(pFile, file_name.c_str(), NULL);
                if(UNZ_OK == err)
                {
                    exist = true;
                }
            }
		}
        while (0);
        apk_file_locker.unlock();
        
		return exist;
	}


}

#endif 
