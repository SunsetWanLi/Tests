#include "../../CommonBase/CommonDef.h"
#include "NetDef.h"
#include "Utility.h"
#include <assert.h>
#include <memory.h> //memset
#include <algorithm> //replace_if
#include <functional> // equal_to
#include <iostream>
#include <time.h>
#include <math.h>
#include <set>
#include "MGLog.h"
#include "../../Platform/MGFileUtils.h"
#include "../../ExternalLib/support/zip_support/unzip.h"
#include "../../ExternalLib/support/zip_support/zip.h"
using namespace z11 ;

#if defined(_WIN32) || defined(_WIN64)
#if defined(WINCE)
#define MKDIR(dir_str ) CreateDirectory ((astr2wstr(dir_str)).c_str(),NULL)
#else
#include <sys/stat.h> //for func: stat()
#include <direct.h>  // _mkdir
#include <io.h> // access
#define  MKDIR(dir_str ) _mkdir ((dir_str).c_str() )
#endif
#else // GNU
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h> //snprintf
#include <dirent.h> // DIR
//#include <uuid/uuid.h> //  sudo apt-get install uuid-dev
#define  MKDIR(dir_str )  mkdir ((dir_str).c_str(), 0777)
typedef struct _GUID
{
	uint_32 Data1 ;
	uint_16 Data2 ;
	uint_16 Data3 ;
	byte_8 Data4[8] ;
}GUID,UUID;

#endif

#ifndef _PATH_MAX_
#if defined(_WIN32) || defined(_WIN64)
#define _PATH_MAX_ MAX_PATH // 260
#else
#define _PATH_MAX_ 4096
#endif
#endif


#define RETURN_F_IF(x) if(x) return false
#define RETURN_T_IF(x) if(x) return true


namespace z11
{
    
#if MG_TARGET_PLATFORM == MG_PLATFORM_WIN32
	/*
** Convert a UTF-8 string to microsoft unicode (UTF-16?). 
**
** Space to hold the returned string is obtained from malloc.
*/
WCHAR *win32_utf8ToUnicode(const char *zFilename){
	uint_32 nChar = 0;
	WCHAR *zWideFilename;

	nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, NULL, 0);
	zWideFilename = (WCHAR*)malloc((nChar+1)*sizeof(WCHAR) );
	if( zWideFilename==0 ){
		return 0;
	}
	wmemset(zWideFilename,0,nChar+1);
	nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, zWideFilename, nChar);
	if( nChar==0 ){
		free(zWideFilename);
		zWideFilename = 0;
	}
	return zWideFilename;
}

/*
** Convert microsoft unicode to UTF-8.  Space to hold the returned string is
** obtained from malloc().
*/
char *win32_unicodeToUtf8(const WCHAR *zWideFilename){
	uint_32 nByte = 0;
	char *zFilename;

	nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, 0, 0, 0, 0);
	zFilename = (char*)malloc( nByte+1 );
	if( zFilename==0 ){
		return 0;
	}
	memset(zFilename,0,nByte+1);
	nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, zFilename, nByte,
		0, 0);
	if( nByte == 0 ){
		free(zFilename);
		zFilename = 0;
	}
	return zFilename;
}

/*
** Convert an ansi string to microsoft unicode, based on the
** current codepage settings for file apis.
** 
** Space to hold the returned string is obtained
** from malloc.
*/
WCHAR *win32_mbcsToUnicode(const char *zFilename){
	uint_32 nByte = 0;
	WCHAR *zMbcsFilename;
	//int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	int codepage = CP_ACP;

	nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, NULL,0);
	zMbcsFilename = (WCHAR *)malloc((nByte+1)*sizeof(WCHAR) );
	if( zMbcsFilename==0 ){
		return 0;
	}
	wmemset(zMbcsFilename, 0, nByte+1);
	nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, zMbcsFilename, nByte);
	if( nByte==0 ){
		free(zMbcsFilename);
		zMbcsFilename = 0;
	}
	return zMbcsFilename;
}

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
**
** Space to hold the returned string is obtained from
** malloc().
*/
char *win32_unicodeToMbcs(const WCHAR *zWideFilename){
	uint_32 nByte = 0;
	char *zFilename;
	//int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	int codepage = CP_ACP;

	nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, 0, 0, 0, 0);
	zFilename = (char*)malloc( nByte+1 );
	if( zFilename==0 ){
		return 0;
	}
	memset(zFilename, 0, nByte+1);
	nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, zFilename, nByte,
		0, 0);
	if( nByte == 0 ){
		free(zFilename);
		zFilename = 0;
	}
	return zFilename;
}

/*
** Convert multibyte character string to UTF-8.  Space to hold the
** returned string is obtained from malloc().
*/
char *win32_mbcs_to_utf8(const char *zFilename){
	char *zFilenameUtf8;
	WCHAR *zTmpWide;

	zTmpWide = win32_mbcsToUnicode(zFilename);
	if( zTmpWide==0 ){
		return 0;
	}
	zFilenameUtf8 = win32_unicodeToUtf8(zTmpWide);
	free(zTmpWide);
	return zFilenameUtf8;
}

/*
** Convert UTF-8 to multibyte character string.  Space to hold the 
** returned string is obtained from malloc().
*/
char *win32_utf8_to_mbcs(const char *zFilename){
	char *zFilenameMbcs;
	WCHAR *zTmpWide;

	zTmpWide = win32_utf8ToUnicode(zFilename);
	if( zTmpWide==0 ){
		return 0;
	}
	zFilenameMbcs = win32_unicodeToMbcs(zTmpWide);
	free(zTmpWide);
	return zFilenameMbcs;
}
#endif

char* Utility::unicode_to_asci(const wchar_t *src )
    {
		char* dest = NULL;
#if defined(_WIN32) || defined(_WIN64)
		dest = win32_unicodeToMbcs(src);
#else
        std::string curLocale = setlocale(LC_ALL, NULL);  // curLocale = "C";
        
        size_t size = wcstombs(NULL, src, 0);
        size_t _Dsize = size + 1;
        char *_Dest = (char*)malloc(_Dsize*sizeof(char));
        memset(_Dest,0,_Dsize);
        wcstombs(_Dest,src,wcslen(src));

        setlocale(LC_ALL, curLocale.c_str());
        dest = _Dest;
#endif
        return dest;
    }
    
    wchar_t * Utility::asci_to_unicode(const char* src )
    {
		wchar_t* dest = NULL;
#if defined(_WIN32) || defined(_WIN64)
		dest =  win32_mbcsToUnicode(src);
#else //ios
        const char * curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
        
        size_t size = mbstowcs(NULL, src, 0);
        size_t _Dsize = (size + 1);
        wchar_t *_Dest = (wchar_t*)malloc(_Dsize*sizeof(wchar_t));
        wmemset(_Dest, 0, _Dsize);
        mbstowcs(_Dest,src,strlen(src));
        
        setlocale(LC_ALL, curLocale);
        dest = _Dest;
#endif
        return dest ;
    }
    
    uint_32 utf8Len(const char* utf8)
    {
        uint_32 ret = 0;
        string content = utf8;
        while (!content.empty())
        {
            uint_32 step = 1;
            char lastchar = 0;
            lastchar = content[0];
            
            if ((lastchar & 0xF8) == 0xF0) // |1111|0xxx| 4字节编码字符 //
            {
                step = 4;
            } else if ((lastchar & 0xF0) == 0xE0) // |1110|xxxx| 3字节编码字符 //
            {
                step = 3;
            } else if ((lastchar & 0xE0) == 0xC0) // |110x|xxxx| 2字节编码字符 //
            {
                step = 2;
            } else if ((lastchar & 0x80) == 0x00) // |0xxx|xxxx| 1字节编码字符 //
                step = 1;
            if (step > content.length()) {
                ret = 0;   // invalide UTF8 string //
                break;
            }
            content = content.substr(step,content.length());
            ++ret;
        }
        return ret;
    }
    
    wchar_t * Utility::utf8_to_unicode(const char* utf8 )
    {
        wchar_t *ret = NULL;
#if defined(_WIN32) || defined(_WIN64)
		ret = win32_utf8ToUnicode(utf8);
#else
        uint_32 size = utf8Len(utf8);
        if (size > 0) {
            wchar_t *pDst = (wchar_t*)malloc((size+1)*sizeof(wchar_t));
            ret = pDst;
            wmemset(pDst, 0, (size+1));
            const char *pSrc = utf8;
            while(*pSrc!='\0')
            {
                if((*pSrc&0xF0)==0xE0) //Chinese utf-8 char used 3bytes
                {
                    *pDst=(*pSrc++ & 0x0F) << 12;
                    *pDst |= (*pSrc++ & 0x3F) << 6;
                    *pDst |= (*pSrc++ & 0x3F);
                }
                else if((*pSrc&0x80)==0) //English utf-8 char used 1bytes
                {
                    *pDst=*pSrc++;
                }
                else if((*pSrc&0xE0)==0)
                {
                    *pDst=(*pSrc++ &0x1F)<<13;
                    *pDst|=(*pSrc++ &0x3F);
                }
                else //can't code
                    *pDst=0;
                pDst++;
            }
            *pDst=0;
        }
#endif
        return ret;
    }

    char * Utility::unicode_to_utf8(const wchar_t * unicode )
    {
        char *ret = NULL;
#if defined(_WIN32) || defined(_WIN64)
		ret = win32_unicodeToUtf8(unicode);
#else
        size_t size = wcstombs(NULL, unicode, 0);
        char *pDst = (char*)malloc((size+1)*sizeof(char));
        ret = pDst;
        memset(pDst, 0, size+1);
        const wchar_t *pSrc	= unicode;
        while(*pSrc!='\0')
        {
            if(*pSrc>0x400) //Chinese utf-8 
            {
                *pDst = 0xE0|(*pSrc>>12);
                pDst++;
                *pDst = 0x80|((*pSrc>>6)&0x3F);
                pDst++;
                *pDst = 0x80|(*pSrc&0x3F);
                pDst++;
            }
            else if(*pSrc<0x80) //English utf-8  
            {
                *pDst = (char)*pSrc;
                pDst++;
            }
            else
                *pDst++=0;
            pSrc++;
        }
        *pDst=0;
#endif
        return ret;
    }

	char *Utility::ansi_to_utf8(const char* ansi)
	{
        char *ret = NULL;
#if defined(_WIN32) || defined(_WIN64)
		ret =  win32_mbcs_to_utf8(ansi);
#else
        wchar_t *wstr = asci_to_unicode(ansi);
        ret = unicode_to_utf8(wstr);
        free(wstr);
#endif
        return ret;
	}

	char *Utility::utf8_to_ansi(const char* utf8)
	{
        char* ret = NULL;
#if defined(_WIN32) || defined(_WIN64)
		ret = win32_utf8_to_mbcs(utf8);
#else
        wchar_t* wstr = utf8_to_unicode(utf8);
        ret = unicode_to_asci(wstr);
        free(wstr);
#endif
        return ret;
	}
    
    MGWString Utility::astr2wstr(const MGAString& astr)
    {
		MGWString ret;
		wchar_t* wstr = asci_to_unicode(astr.c_str());
		if (wstr != NULL)
		{
			ret = MGWString(wstr);
			free(wstr);
		}
        return ret;
    }
    MGAString Utility::wstr2astr(const MGWString& wstr)
    {
		MGAString ret;
		char* str = unicode_to_asci(wstr.c_str());
		if (str != NULL)
		{
			ret = MGAString(str);
			free(str);
		}
		return ret;
    }
    MGWString Utility::ustr2wstr(const MGUString& ustr)
    {
        MGWString ret;
		wchar_t* wstr = utf8_to_unicode(ustr.c_str());
		if (wstr != NULL)
		{
			ret = MGWString(wstr);
			free(wstr);
		}
		return ret;
    }
    MGUString Utility::wstr2ustr(const MGWString& wstr)
    {
        MGUString ret;
		char* str = unicode_to_utf8(wstr.c_str());
		if (str != NULL)
		{
			ret = MGUString(str);
			free(str);
		}
		return ret;
    }
    MGUString Utility::astr2ustr(const MGAString &astr)
    {
        MGUString ret;
		char* str = ansi_to_utf8(astr.c_str());
		if (str != NULL)
		{
			ret = MGUString(str);
			free(str);
		}
		return ret;
    }
    MGAString Utility::ustr2astr(const MGUString &ustr)
    {
        MGAString ret;
		char* str = utf8_to_ansi(ustr.c_str());
		if (str != NULL)
		{
			ret = MGAString(str);
			free(str);
		}
		return ret;
    }

	 size_t Utility::wcslen( const wchar_t * wstr)
    {
        size_t len=0;
        if (wstr!=NULL)
        {
            for(len=0;wstr[len]!=0;len++);
        }
        return len;
    }
    
    wchar_t* Utility::WStringCat( wchar_t *dest, const wchar_t *src )
    {
#ifdef ANDROID_NDK
        if (dest==NULL||src==NULL)
        {
            return dest;
        }
        wchar_t *p			=	dest;
        const wchar_t *q	=	src;
        while (*p++);
        --p;
        while((*p++=*q++)!='\0');
        return dest;
#else
        return ::wcscat(dest,src);
#endif
        
    }
    
    wchar_t* Utility::WStringCpy( wchar_t *dest, const wchar_t *src )
    {
#ifdef ANDROID_NDK
        if (dest==NULL)
        {
            return dest;
        }
        if (src==NULL)
        {
            dest[0]=0;
            return dest;
        }
        const wchar_t *p=src;
        wchar_t *q=dest;
        while ((*q++=*p++)!='\0');
        return dest;
#else
        return ::wcscpy(dest,src);
#endif
        
    }
    
    int Utility::wcscmp(const wchar_t *dest, const wchar_t *src)
    {
#ifdef ANDROID_NDK
        if (dest==NULL||src==NULL)
        {
            return dest-src;
        }
        int i=0;
        for(i=0;dest[i]==src[i]&&dest[i];i++);
        return dest[i]-src[i];
#else
        return ::wcscmp(dest,src);
#endif
        
    }

    std::string Utility::getCurrentWorkingDirA()
    {
        char szFull[_PATH_MAX_];
        std::string strPath;
        
#if defined(_WIN32) || defined(_WIN64)
#ifdef WINCE
        wchar_t tempFull[_PATH_MAX_];
        GetModuleFileName (NULL, tempFull, sizeof(tempFull) / sizeof( tempFull[0])) ;
        wc2c(szFull,tempFull);
        
#else
        ::GetModuleFileNameA (NULL, szFull, sizeof(szFull) / sizeof( szFull[0])) ;
        
#endif
        
        strPath = szFull;
        strPath = strPath.substr (0, strPath.find_last_of ("\\") + 1  ) ;// e.g.: F:\wxp\MPPF\trunk\MPPF\TPF\Test\algo1\  /
#else
        int rslt = readlink("/proc/self/exe",szFull,sizeof(szFull) / sizeof( szFull[0]));
        if (rslt < 0 || rslt >= _PATH_MAX_)
        {
            strPath = "";
        }
        else
        {
            szFull[rslt] = '\0' ;
            strPath = szFull ;
        }
        strPath = strPath.substr (0, strPath.find_last_of ("/") + 1   ) ;  // 濡傦細/home/ww/are/
        
#endif
        return strPath ;
    }
    
    
    MGWString Utility::getCurrentWorkingDirW()
    {
#if defined(_WIN32) || defined(_WIN64)
        wchar_t szFull[_PATH_MAX_];
        MGWString wstrPath;
        
        ::GetModuleFileNameW (NULL, szFull, sizeof(szFull) / sizeof( szFull[0])) ;
        
        wstrPath = szFull;
        wstrPath = wstrPath.substr (0, wstrPath.find_last_of (L"\\") + 1  ) ;
        return wstrPath ;
#else
        std::string str = Utility::getCurrentWorkingDirA ();
        return Utility::ustr2wstr (str);
#endif
        
    }
    
    //
    bool Utility::getFilesInDir(const MGWString &dir1, vector<MGWString> &filenames)
    {
        bool ret = false ;
        filenames.clear();
#if defined(_WIN32) || defined(_WIN64)
        WIN32_FIND_DATA find_file_data;
        HANDLE find_handle = INVALID_HANDLE_VALUE;
        MGWString files = dir1 + L"/*";
        find_handle = FindFirstFile(files.c_str(),&find_file_data);
        BOOL find_next = TRUE;
        while(find_handle != INVALID_HANDLE_VALUE && find_next)
        {
            ret = true ;
            if(find_file_data.dwFileAttributes & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE))
            {
                filenames.push_back(find_file_data.cFileName);
            }
            find_next = FindNextFile(find_handle,&find_file_data);
        }
#else  // gnu
        struct dirent *ptr = NULL;
        MGWString dir = dir1 ;
        if (!dir.empty () && dir[ dir.size () - 1 ] != L'/')
        {
            dir += L'/' ;
        }
        DIR *dir_linux = opendir(Utility::wstr2ustr(dir).c_str());
        if (!dir_linux )
        {
            return false;
        }
        ret = true;
        
        while((ptr = readdir(dir_linux)) != NULL)
        {
            string full_path = wstr2ustr(dir) + ptr->d_name;
            struct stat f_st;
            stat(full_path.c_str(), &f_st);
            
            //if(ptr->d_type == DT_REG)
            if(S_ISREG(f_st.st_mode))
            {
                filenames.push_back(Utility::ustr2wstr(string(ptr->d_name)));
            }
            else
            {
                //
            }
        }
        
        closedir(dir_linux);
        
        
#endif
        
        return ret ;
    }
    
    bool Utility::getFilesInDir(const std::string &dir1, vector<std::string> &filenames)
    {
        bool ret = false ;
        filenames.clear();
#if defined(_WIN32) || defined(_WIN64)
        WIN32_FIND_DATA find_file_data;
        HANDLE find_handle = INVALID_HANDLE_VALUE;
#ifdef UNICODE
        MGWString files = astr2wstr(dir1) + L"/*";
        find_handle = FindFirstFile(files.c_str(),&find_file_data);
        BOOL find_next = TRUE;
        while(find_handle != INVALID_HANDLE_VALUE && find_next)
        {
            ret = true;
            if(find_file_data.dwFileAttributes & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE))
            {
                filenames.push_back(wstr2astr(find_file_data.cFileName));
            }
            find_next = FindNextFile(find_handle,&find_file_data);
        }
        FindClose(find_handle);
#else
        string files = dir1 + "/*";
        find_handle = FindFirstFile(files.c_str(),&find_file_data);
        BOOL find_next = TRUE;
        while(find_handle != INVALID_HANDLE_VALUE && find_next)
        {
            ret = true ;
            if(find_file_data.dwFileAttributes & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE))
            {
                filenames.push_back(find_file_data.cFileName);
            }
            find_next = FindNextFile(find_handle,&find_file_data);
        }
#endif
#else  // gnu
        struct dirent *ptr = NULL;
        string dir = dir1 ;
        if (!dir.empty () && dir[ dir.size () - 1 ] != '/')
        {
            dir += '/' ;
        }
        DIR *dir_linux = opendir(dir.c_str () );
        if (!dir_linux )
        {
            return false;
        }
        ret = true;
        
        while((ptr = readdir(dir_linux)) != NULL)
        {
            string full_path = dir + ptr->d_name;
            struct stat f_st;
            stat(full_path.c_str(), &f_st);
            
            //if(ptr->d_type == DT_REG)
            if(S_ISREG(f_st.st_mode))
            {
                filenames.push_back(string(ptr->d_name));
            }
            else
            {
                //
            }
        }
        
        closedir(dir_linux);
        
#endif
        return ret ;
    }
    
    string Utility::getFileFromPath(const string &path)
    {
        string ret;
        ret = path.substr(path.find_last_of('/')+1);
        return ret;
    }
    
    MGWString Utility::getFileFromPath(const MGWString &path)
    {
        MGWString ret;
        ret = path.substr(path.find_last_of(L'/')+1);
        return ret;
    }
    
    
    bool Utility::getSubdirsInDir(const MGWString &dir1, vector<MGWString> &subdirs_lists,bool include_hidden_dirs  )
    {
        bool ret = false ;
        subdirs_lists.clear();
#if defined(_WIN32) || defined(_WIN64)
        WIN32_FIND_DATA find_file_data;
        HANDLE find_handle = INVALID_HANDLE_VALUE;
        MGWString files = dir1 + L"/*";
        find_handle = FindFirstFile(files.c_str(),&find_file_data);
        BOOL find_next = TRUE;
        while(find_handle != INVALID_HANDLE_VALUE && find_next )
        {
            ret = true ;
            if((find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
               && (include_hidden_dirs || !(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )) )
            {
                subdirs_lists.push_back(find_file_data.cFileName);
            }
            find_next = FindNextFile(find_handle,&find_file_data);
        }
#else  // gnu
        struct dirent *ptr = NULL;
        MGWString dir = dir1 ;
        if (!dir.empty () && dir[ dir.size () - 1 ] != L'/')
        {
            dir += L'/' ;
        }
        DIR *dir_linux = opendir(Utility::wstr2ustr(dir).c_str());
        if (!dir_linux )
        {
            return false;
        }
        ret = true;
        
        string d_name ;
        while((ptr = readdir(dir_linux)) != NULL)
        {
            d_name = ptr->d_name ;
            if(!include_hidden_dirs)
            {
                if (!d_name.empty () && d_name[0] == '.')
                {
                    continue;
                }
            }
            
            string full_path = wstr2ustr(dir) + d_name;
            struct stat f_st;
            stat(full_path.c_str(), &f_st);
            
            if(S_ISDIR(f_st.st_mode))
            {
                subdirs_lists.push_back(Utility::ustr2wstr(d_name));
            }
        }
        
        closedir(dir_linux);
        
#endif
        
        return ret ;
    }
    
    
    bool Utility::getSubdirsInDir(const std::string &dir1, vector<string> &subdirs_lists ,bool include_hidden_dirs  )
    {
        bool ret = false ;
        subdirs_lists.clear();
#if defined(_WIN32) || defined(_WIN64)
#ifdef UNICODE
        WIN32_FIND_DATA find_file_data;
        HANDLE find_handle = INVALID_HANDLE_VALUE;
        MGWString files = astr2wstr(dir1) + L"/*";
        find_handle = FindFirstFileW(files.c_str(),&find_file_data);
        BOOL find_next = TRUE;
        while(find_handle != INVALID_HANDLE_VALUE && find_next)
        {
            ret = true ;
            if((find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
               && (include_hidden_dirs || !(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )) )
            {
                subdirs_lists.push_back(wstr2astr(find_file_data.cFileName));
            }
            find_next = FindNextFileW(find_handle,&find_file_data);
        }
        FindClose(find_handle);
#else
        WIN32_FIND_DATAA find_file_data;
        HANDLE find_handle = INVALID_HANDLE_VALUE;
        string files = dir1 + "/*";
        find_handle = FindFirstFile(files.c_str(),&find_file_data);
        BOOL find_next = TRUE;
        while(find_handle != INVALID_HANDLE_VALUE && find_next)
        {
            ret = true ;
            if((find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
               && (include_hidden_dirs || !(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )) )
            {
                subdirs_lists.push_back(find_file_data.cFileName);
            }
            find_next = FindNextFile(find_handle,&find_file_data);
        }
        
#endif
#else  // gnu
        struct dirent *ptr = NULL;
        string dir = dir1 ;
        if (!dir.empty () && dir[ dir.size () - 1 ] != '/')
        {
            dir += '/' ;
        }
        DIR *dir_linux = opendir((dir).c_str());
        if (!dir_linux )
        {
            return false;
        }
        ret = true;
        
        string d_name ;
        while((ptr = readdir(dir_linux)) != NULL)
        {
            d_name = ptr->d_name ;
            if(!include_hidden_dirs)
            {
                if (!d_name.empty () && d_name[0] == '.')
                {
                    continue;
                }
            }
            
            string full_path = dir + d_name;
            struct stat f_st;
            stat(full_path.c_str(), &f_st);
            
            if(S_ISDIR(f_st.st_mode))
            {
                subdirs_lists.push_back(d_name);
            }
        }
        
        closedir(dir_linux);
        
#endif
        while (!subdirs_lists.empty())
        {
            vector<string>::iterator it=subdirs_lists.begin();
            if (*it==".")
            {
                subdirs_lists.erase(it);
            }
            else if (*it=="..")
            {
                subdirs_lists.erase(it);
            }
            else
                break;
        }
        if (subdirs_lists.empty())
        {
            return false;
        }
        return ret ;
    }
    
    
    //
    bool Utility::createDirOnPath(const string& dir__)
    {
        if (isFileORDirExist(dir__))
        {
            return true;
        }
        bool is_success = false ;
        if (!dir__.size() )
        {
            return false ;
        }
        string dir = dir__ ;
        char delimiters =
        //#if defined(_WIN32) || defined(_WIN64)
        //		'\\' ;
        //	std::replace_if(dir.begin(),dir.end(),bind2nd(equal_to<char>(),'/'),'\\');
        //#else
        //		'/' ;
        //	std::replace_if(dir.begin(),dir.end(),bind2nd(equal_to<char>(),'\\'),'/');
        //#endif
        '/' ;
        std::replace_if(dir.begin(),dir.end(),bind2nd(equal_to<char>(),'\\'),'/');
        
        string::size_type pos1 = dir.find_first_of(delimiters);
        if (pos1 != string::npos)
        {
            is_success = true ;
        }
        int is_create_success = false ;
        string med_dir ;
        while(pos1 != string::npos)
        {
            med_dir = dir.substr(0 , pos1 );
            
            if (!isFileORDirExist(med_dir))
            {
                is_create_success = MKDIR(med_dir) ;
                if (is_create_success != 0 ) //
                {
                    //is_success = false ;
                    //break;
                }
            }
            pos1 = dir.find_first_of(delimiters,  pos1 + 1 );
        }
        
        return is_success ;
    }
    
    
    bool Utility::createDirOnPath(const MGWString& dir)
    {
#if defined(_WIN32) || defined(_WIN64)
        return createDirOnPath (Utility::wstr2astr (dir ) ) ;
#else
		return createDirOnPath (Utility::wstr2ustr (dir ) ) ;
#endif
    }
    
    
    //
    bool Utility::isFileORDirExist(const std::string& file_path_or_dir_path)
    {
        return MGFileUtils::sharedFileUtils()->fileExistsAtPath(file_path_or_dir_path);
    }
    
    
    bool Utility::isFileORDirExist(const MGWString& file_path_or_dir_path)
    {
#if defined(_WIN32) || defined(_WIN64)
        return isFileORDirExist(wstr2astr(file_path_or_dir_path));
#else
		return isFileORDirExist(wstr2ustr(file_path_or_dir_path));
#endif
    }
    
    //
    bool Utility::isDir(const std::string& path)
    {
        bool is_directory = false ;
#if defined(_WIN32) || defined(_WIN64)
        return isDir(Utility::astr2wstr(path));
#else // Linux
        DIR* dir = opendir(path.c_str ());
        if( dir != NULL )
        {
            closedir(dir);
            is_directory = true ;
        }
#endif
        return is_directory ;
    }
    
    bool Utility::isDir(const MGWString& path)
    {
        bool is_directory = false ;
#if defined(_WIN32) || defined(_WIN64)
        DWORD dwAttr = ::GetFileAttributesW(path.c_str());  //得到文件属性
        if (dwAttr&FILE_ATTRIBUTE_DIRECTORY)  // 如果是目录
            is_directory = true;
#else // Linux
        is_directory = isDir (Utility::wstr2ustr (path ) ) ;
#endif
        return is_directory ;
    }
    
    //
    bool Utility::delDirForcibly(const string &file_path)
    {
        bool is_success = true;
        string dir_path = file_path + "/";
        if (!isFileORDirExist(dir_path)) {
            return true;
        }
        vector<string> files;
        getFilesInDir(dir_path, files);
        vector<string>::iterator it;
        for (it=files.begin(); it!=files.end(); it++) {
            is_success = delAFile(dir_path + (*it)) && is_success;
        }
        getSubdirsInDir(dir_path, files);
        for (it=files.begin(); it!=files.end(); it++) {
            is_success = delDirForcibly(dir_path + (*it)) && is_success;
        }
        is_success = delDir(dir_path) && is_success;
        return is_success;
    }
    
    
    bool Utility::delDirForcibly(const MGWString &file_path)
    {
#if defined(_WIN32) || defined(_WIN64)
        return delDirForcibly(wstr2astr(file_path));
#else
		return delDirForcibly(wstr2ustr(file_path));
#endif
    }
    
    //
    bool Utility::delAFile(const string&file_full_path)
    {
        bool is_success = false ;
        
        
#if defined(_WIN32) || defined(_WIN64)
#if defined(WINCE)
        is_success=delAFile(astr2wstr(file_full_path));
#else
		is_success = _unlink (file_full_path.c_str ()) ==0;
#endif
        
#else
		is_success = unlink(file_full_path.c_str () ) ==0;
#endif
        //is_success = (ret_del == 0 ) ;
        return is_success ;
    }
    
    
    
    bool Utility::delAFile(const MGWString&file_full_path )
    {
        bool is_success = false ;
        
#if defined(_WIN32) || defined(_WIN64)
#if defined(WINCE)
        is_success=DeleteFile(file_full_path.c_str());
#else
        int ret_del =
        _wunlink(file_full_path.c_str () ) ;
        is_success = (ret_del == 0 ) ;
#endif
#else
        is_success = delAFile (Utility::wstr2ustr (file_full_path )) ;
#endif
        
        return is_success ;
    }
    
    //
    bool Utility::delDir(const string&full_path)
    {
        bool is_success = false ;
        
        
#if defined(_WIN32) || defined(_WIN64)
        is_success = delDir(Utility::astr2wstr(full_path));
#else
        is_success = rmdir(full_path.c_str () ) ==0;
#endif
        //is_success = (ret_del == 0 ) ;
        return is_success ;
    }
    
    
    bool Utility::delDir(const MGWString&full_path )
    {
        bool is_success = false ;
        
#if defined(_WIN32) || defined(_WIN64)
        is_success = RemoveDirectoryW(full_path.c_str()) == 0;
#else
        is_success = delDir (Utility::wstr2ustr (full_path )) ;
#endif
        
        return is_success ;
    }
    
    void Utility::toLower(MGWString  &wstr)
    {
        for(size_t ch = 0 ; ch < wstr.size () ; ch ++)
        {
            if (wstr[ch] >= L'A' && wstr[ch] <= L'Z' )
            {
                wstr[ch] += L'a' - L'A' ;
            }
        }
    }
    
    void Utility::toUpper(MGWString  &wstr)
    {
        for(size_t ch = 0 ; ch < wstr.size () ; ch ++)
        {
            if (wstr[ch] >= L'a' && wstr[ch] <= L'z' )
            {
                wstr[ch] -= L'a' - L'A' ;
            }
        }
    }
    
    void Utility::toLower(string  &str)
    {
        for(size_t ch = 0 ; ch < str.size () ; ch ++)
        {
            if (str[ch] >= 'A' && str[ch] <= 'Z' )
            {
                str[ch] += 'a' - 'A' ;
            }
        }
    }
    
    void Utility::toUpper(string  &str)
    {
        for(size_t ch = 0 ; ch < str.size () ; ch ++)
        {
            if (str[ch] >= 'a' && str[ch] <= 'z' )
            {
                str[ch] -= 'a' - 'A' ;
            }
        }
    }
    
    bool Utility::hasExtName(const MGWString&full_path,const MGWString&ext_name)
    {
        bool ret_val = false;
        static const wchar_t DOT = L'.';
        do
        {
            MGWString::size_type dot_pos = full_path.find_last_of(DOT);
            if(dot_pos == MGWString::npos || dot_pos + 1 == MGWString::npos )
            {
                break;
            }
            dot_pos ++ ;
            MGWString suffix = full_path.substr(dot_pos  ,full_path.size() - dot_pos);
            Utility::toUpper (suffix) ;
            MGWString ext_name_temp = ext_name ;
            Utility::toUpper (ext_name_temp ) ;
            if (suffix == ext_name_temp )
            {
                ret_val = true ;
            }
        }while(0);
        
        return ret_val;
    }
    
    bool Utility::hasExtName(const string&full_path, const string&ext_name)
    {
        bool ret_val = false;
        static const char DOT = '.';
        do
        {
            string::size_type dot_pos = full_path.find_last_of(DOT);
            if(dot_pos == string::npos || dot_pos + 1 == string::npos )
            {
                break;
            }
            dot_pos ++ ;
            string suffix = full_path.substr(dot_pos  ,full_path.size() - dot_pos);
            Utility::toUpper (suffix) ;
            string ext_name_temp = ext_name ;
            Utility::toUpper (ext_name_temp ) ;
            if (suffix == ext_name_temp )
            {
                ret_val = true ;
            }
        }while(0);
        
        return ret_val;
    }
    
    //template<typename T>//T --->string
    //std::string Utility::BuiltInTypeToString(const T &x)
    //{	// double d = 213.99 ;
    //	// string str = Utility::BuiltInTypeToString(d) ;
    //	std::ostringstream oss;
    //	//std::locale old_loc = oss.imbue(std::locale("C"));
    //	oss<<x;
    //	//oss.imbue(old_loc) ;
    //	return oss.str();
    //}
    
    //template<>
    //std::string Utility::BuiltInTypeToString(const int_32 &x)
    //{
    //	string str;
    //	char buf[12] = { 0 };
    //#ifdef WINCE
    //	_itoa_s(x, buf, 2, 10);
    //#else
    //	sprintf(buf, "%d", x);
    //#endif
    //	str = buf;
    //	return str;
    //}
    
    template<typename T>//T --->MGWString
    MGWString Utility::BuiltInTypeToWString(const T &x, streamsize prec, ios_base& (*f_radix)(std::ios_base&))
    {
		if (prec == 0)
		{
			prec = numeric_limits<T>::digits10;
		}
        std::wostringstream woss;
        //std::locale old_loc = woss.imbue(std::locale("C"));
        //woss.precision(prec);
        //woss.imbue(std::locale("C")); 
        woss<<f_radix<<setprecision(prec)<<x;
        //woss.imbue(old_loc) ;
        return woss.str();
    }
    
    vector<std::string> Utility::SplitString( const string & src, const char token )
    {
        string tmp(&token);
        return SplitString(src, string(&token, 1));
    }
    
    vector<std::string> Utility::SplitString( const std::string & src, const string & token )
    {
        vector<string> ret;
        
        string::size_type nend = 0;
        string::size_type nbegin = 0;
        while(nend != string::npos)
        {
            nend = src.find(token, nbegin);
            if(nend == string::npos)
                ret.push_back(src.substr(nbegin, src.length()-nbegin));
            else
                ret.push_back(src.substr(nbegin, nend-nbegin));
            nbegin = nend + token.size();
        }
        
        return ret;
    }
    
    std::vector<MGWString> Utility::SplitWString( const MGWString & src, const MGWString & token )
    {
        vector<MGWString> ret;
        
        wstring::size_type nend = 0;
        wstring::size_type nbegin = 0;
        while(nend != wstring::npos)
        {
            nend = src.find(token, nbegin);
            if(nend == wstring::npos)
                ret.push_back(src.substr(nbegin, src.length()-nbegin));
            else
                ret.push_back(src.substr(nbegin, nend-nbegin));
            nbegin = nend + token.size();
        }
        
        return ret;
    }
    
    void Utility::TrimString(string &src)
    {
        string::size_type idx = src.find(' ');
        while (idx != string::npos)
        {
            src.erase(idx);
            idx = src.find(' ');
        }
        idx = src.find('\t');
        while (idx != string::npos)
        {
            src.erase(idx);
            idx = src.find('\t');
        }
    }
    
    void Utility::TrimWString(MGWString &src)
    {
        wstring::size_type idx = src.find(L' ');
        while (idx != wstring::npos)
        {
            src.erase(idx);
            idx = src.find(L' ');
        }
        idx = src.find(L'\t');
        while (idx != wstring::npos)
        {
            src.erase(idx);
            idx = src.find(L'\t');
        }
    }
    
    bool Utility::unzip(const string &zip_file_path, const string &to_path)
    {
        bool ret=true;
        assert(isFileORDirExist(zip_file_path));
        string zip_to_path = to_path;
        if(zip_to_path.substr(zip_to_path.length()-1)!="/")
            zip_to_path += "/";
        unzFile f = unzOpen(zip_file_path.c_str());
        if (f == NULL) {
            ret = false;
            return ret;
        }
        int err = unzGoToFirstFile(f);
        if(err!=UNZ_OK){
            ret=false;
            unzClose(f);
            return ret;
        }
        std::vector<std::string> file_in_zip;
        while (err == UNZ_OK){
            char file_name[256];
            err = unzGetCurrentFileInfo64(f,NULL,file_name,sizeof(file_name)-1,NULL,0,NULL,0);
            std::string p =file_name;
            file_in_zip.push_back(p);
            if (err == UNZ_OK)
                err = unzGoToNextFile(f);
        }
        unzClose(f);
        if(!isFileORDirExist(zip_to_path)){
            createDirOnPath(zip_to_path);
        }
        std::vector<std::string>::iterator it = file_in_zip.begin();
        for(;it!=file_in_zip.end();it++){
            MGData *data = MGFileUtils::sharedFileUtils()->getFileDataFromZip(zip_file_path,*it);
            std::string include_file_path = zip_to_path+*it;
            if(data==NULL){       //表示这是个文件夹
                createDirOnPath(include_file_path);
            }
            else{
                if(data->writeToFile(include_file_path));
                else{															//表示写文件不成功，可能是文件路径不存在，要先创建				
                    std::string tmp_path = *it;
                    string::size_type pos = tmp_path.find("/");
                    while(pos!=string::npos){
                        std::string create_tmp_path = zip_to_path+tmp_path.substr(0,pos);
                        pos = tmp_path.find("/",pos+1);
                        if(!isFileORDirExist(create_tmp_path+"/"))
                            createDirOnPath(create_tmp_path+"/");
                    }
                    data->writeToFile(include_file_path);
                }
            }
            delete_and_set_null(data);
        }
        return ret;
    }
    
    bool Utility::unzipSpecificFile(const string &zip_file_path, const string &to_path, const string &specific_file_name)
    {
        bool ret=true;
        assert(isFileORDirExist(zip_file_path));
        string zip_to_path = to_path;
        if(zip_to_path.substr(zip_to_path.length()-1)!="/")
            zip_to_path += "/";
        unzFile f = unzOpen(zip_file_path.c_str());
        if (f == NULL) {
            ret = false;
            return ret;
        }
        int err = unzGoToFirstFile(f);
        if(err!=UNZ_OK){
            ret=false;
            delete_and_set_null(f);
            return ret;
        }
        std::vector<std::string> file_in_zip;
        while (err == UNZ_OK){
            char file_name[256];
            err = unzGetCurrentFileInfo64(f,NULL,file_name,sizeof(file_name)-1,NULL,0,NULL,0);
            std::string p =file_name;
            file_in_zip.push_back(p);
            if (err == UNZ_OK)
                err = unzGoToNextFile(f);
        }
        unzClose(f);
        if(!isFileORDirExist(zip_to_path)){
            createDirOnPath(zip_to_path);
        }
        std::vector<std::string>::iterator it = file_in_zip.begin();
        if(specific_file_name.find(".")!=string::npos){
            for(;it!=file_in_zip.end();it++){
                if((*it).find(specific_file_name)!=string::npos){
                    MGData *data = MGFileUtils::sharedFileUtils()->getFileDataFromZip(zip_file_path,*it);
                    if(data!=NULL)
                        data->writeToFile(zip_to_path+specific_file_name);
                    delete_and_set_null(data);
                    delete_and_set_null(f);
                    return ret;
                }
            }
        }
        else{
            for(;it!=file_in_zip.end();it++){
                if((*it).find(specific_file_name)!=string::npos){
                    MGData *data = MGFileUtils::sharedFileUtils()->getFileDataFromZip(zip_file_path,*it);
                    string::size_type _pos = (*it).find_last_of("/");
                    string file_name = (*it).substr(_pos+1);
                    if(data!=NULL){
                        if(!isFileORDirExist(zip_to_path+specific_file_name+"/"))
                            createDirOnPath(zip_to_path+specific_file_name+"/");
                        data->writeToFile(zip_to_path+specific_file_name+"/"+file_name);
                    }
                    delete_and_set_null(data);
                }
            }
        }
        return ret;
    }

	bool Utility::zip(const string &zip_file_path, vector<string> files, const string& folder_path)
    {
        bool ret = false;
		zipFile f = zipOpen(zip_file_path.c_str(), 0);
		if (f != NULL)
		{
			for (auto it = files.begin(); it != files.end(); ++it)
			{
				int nRet = zipOpenNewFileInZip(f, 
										(*it).c_str(),
										NULL, 
										NULL, 
										0,
										NULL,
										0, 
										NULL,
										Z_DEFLATED,
										Z_DEFAULT_COMPRESSION);

				if (nRet == ZIP_OK)
				{
					MGData* data = MGData::dataWithContentsOfFile(folder_path+(*it));
					// read the file and output to zip
					if (data != NULL)
					{
						zipWriteInFileInZip(f, data->bytes(), (unsigned int)data->length());
						ret = true;
					}
					delete_and_set_null(data);
				}

				zipCloseFileInZip(f);
			}
		}

		zipClose(f,NULL);

        return ret;
    }
    
    double Utility::calPolygonArea(const std::vector<z11::MGPoint> &v_points)
    {
        if (v_points.size() < 3)
            return 0.0;
        double s = 0.0;
        std::vector<z11::MGPoint>::size_type size = v_points.size();
        
        for(std::vector<z11::MGPoint>::size_type i=1; i<size; i++)
            s += (v_points[i].y + v_points[i-1].y) * (v_points[i].x - v_points[i-1].x);
        
        //if (v_points[0] != v_points[size - 1])
		s += (v_points[0].y + v_points[size - 1].y) * (v_points[0].x - v_points[size - 1].x);
        
        return fabs(s / 2.0);
    }
    
    // Winsock鍒濆鍖栵紝鍔犺浇DLL銆?
#if defined(_WIN32) || defined(_WIN64)
    bool initWinsock() 
    {
        WORD wV = MAKEWORD(2, 2);// version 2.2
        WSADATA wsaData;
        int err;
        
        err = ::WSAStartup(wV, &wsaData);
        if(err)
        {
            assert(0);
            return false;
        }
        
        if ( LOBYTE( wsaData.wVersion ) != 2 ||
            HIBYTE( wsaData.wVersion ) != 2 ) 
        {
            assert(0);
            uninitWinsock();
            return false ; 
        }
        
        // The WinSock DLL is acceptable. Proceed
        return true;
    }
    
    // detach winsock dll
    void  __cdecl uninitWinsock() 
    {
        ::WSACleanup () ;//
    }
    
    
    Winsock2Setting::Winsock2Setting()
    {
        initWinsock () ;
    }
    
    Winsock2Setting::~Winsock2Setting() 
    {
        uninitWinsock () ;
    }
#endif // #if defined(_WIN32) || defined(_WIN64)
    
} // namespace z11
