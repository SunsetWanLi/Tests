#pragma once
#include <iosfwd>
#include "../../CommonBase/MGWString.h"
#include <vector>
#include <ios>
#include <iostream>
#include <iomanip>
#include <clocale>
#include <sstream>
#include "../../CommonBase/CommonDef.h"
#include "../../CommonBase/MGStructure.h"
#include "NonCopyable.h"
#include <limits>
#if defined(_WIN32) || defined(_WIN64)
#ifdef WINCE
#include <winsock2.h>
#include <windows.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif
#else
#include <locale>
#include <unistd.h>
#include <stdlib.h>
#endif

using namespace std;
namespace z11
{
    class Utility : private NonCopyable
    {
        //Utility() ;
        //~Utility() ;
    public:
		// 以下所有的字符转换，不需要事先分配存储空间，返回的指针需要调用者自己释放，使用Free()释放 //
		// 宽字符到窄字符，Unicode to Ansi //
        static char* unicode_to_asci(const wchar_t *src);
		// Ansi to Unicode
        static wchar_t *asci_to_unicode(const char* src);
		// Utf8 to Unicode
        static wchar_t *utf8_to_unicode(const char* utf8);
		// Unicode to Utf8
        static char *unicode_to_utf8(const  wchar_t * unicode);
		// Ansi to Utf8
		static char *ansi_to_utf8(const char* ansi);
		// Utf8 to Ansi
		static char *utf8_to_ansi(const char* utf8);

        static size_t wcslen(const wchar_t *);
        static int    wcscmp(const wchar_t *, const wchar_t *);
        static wchar_t* WStringCat(wchar_t *dest, const wchar_t *src);
        static wchar_t* WStringCpy(wchar_t *dest, const wchar_t *src);
        
		// Ansi to Unicode
        static MGWString astr2wstr(const MGAString& astr);
		// Unicode to Ansi
        static MGAString wstr2astr(const MGWString& wstr);
		// Utf8 to Unicode
        static MGWString ustr2wstr(const MGUString& ustr);
		// Unicode to Utf8
        static MGUString wstr2ustr(const MGWString& wstr);
		// Ansi to Utf8
        static MGUString astr2ustr(const MGAString& astr);
		// Utf8 to Ansi
        static MGAString ustr2astr(const MGUString& ustr);
        
        
        static std::string getCurrentWorkingDirA() ;
        static MGWString getCurrentWorkingDirW() ;
        
        static bool isNum(char c) { return (c >= '0' && c <= '9'); }
        static bool isNum(wchar_t c) { return (c >= L'0' && c <= L'9'); }
        
        static bool getFilesInDir(const MGWString &dir, vector<MGWString> &filenames);
        static bool getFilesInDir(const std::string &dir, vector<string> &filenames);
        
        //
        static string getFileFromPath(const string& path);
        static MGWString getFileFromPath(const MGWString& path);
        
        //
        static bool getSubdirsInDir(const MGWString &dir, vector<MGWString> &subdirs_lists, bool include_hidden_dirs = false);
        static bool getSubdirsInDir(const string &dir, vector<string> &subdirs_lists, bool include_hidden_dirs = false);
        
        //
        static bool isFileORDirExist(const string& file_path_or_dir_path);
        static bool isFileORDirExist(const MGWString& file_path_or_dir_path);
        
        //
        static bool isDir(const string& path);
        static bool isDir(const MGWString& path);
        
        //
        static bool createDirOnPath(const string& dir);
        static bool createDirOnPath(const MGWString& dir);
        
        //
        static bool delDirForcibly(const string &file_path);
        static bool delDirForcibly(const MGWString &file_path);
        
        //
        static bool delAFile(const string &file_path) ;
        static bool delAFile(const MGWString &file_path ) ;
        
        //
        static bool delDir(const string &file_path) ;
        static bool delDir(const MGWString &file_path ) ;
        
        //
        static void toLower(string &str);
        static void toUpper(string &str);
        
        static void toLower(MGWString &wstr);
        static void toUpper(MGWString &wstr);
        
        static bool hasExtName(const MGWString &full_path, const MGWString &ext_name);
        static bool hasExtName(const string &full_path, const string &ext_name);
        
        template<typename T>//string --->  T
        static T BuiltInTypeFromString(const string& str, ios_base& (*f_radix)(std::ios_base&) = std::dec);
        
        template<typename T>//MGWString --->  T
        static T BuiltInTypeFromWString(const MGWString& wstr, ios_base& (*f_radix)(std::ios_base&) = std::dec);
        
        template<typename T>//T --->string
        static std::string BuiltInTypeToString(const T &x, streamsize prec = 0, ios_base& (*f_radix)(std::ios_base&) = std::dec);
        
        template<typename T>//T --->MGWString
        static MGWString BuiltInTypeToWString(const T &x, streamsize prec = 0, ios_base& (*f_radix)(std::ios_base&) = std::dec);
        
        static std::vector<std::string> SplitString(const string & src, const char token);
        static std::vector<std::string> SplitString(const string & src, const string & token);
        static std::vector<MGWString> SplitWString(const MGWString & src, const MGWString & token);
        
        static void TrimString(string& src);
        static void TrimWString(MGWString& src);
        
        //使用下面2个接口，win32工程需要增加附加包含目录..\..\..\z11\trunk\z11\ExternalLib\zlib
        static bool unzip(const string &zip_file_path, const string &zip_to_path);             //解压全部文件到指定路径
        static bool unzipSpecificFile(const string &zip_file_path, const string &zip_to_path, const string &specific_file_name);  //解压特定文件或文件夹到指定路径
		static bool zip(const string& zip_file_path, vector<string> files, const string& folder_path);						// 将指定的文件列表全部压缩到指定文件内 //
        //必须保证传入的点是顺序的（逆序或顺序）
        static double calPolygonArea(const vector<MGPoint> &v_points);
    };
    
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    template<typename T>//T --->string
    std::string Utility::BuiltInTypeToString(const T &x, streamsize prec, ios_base& (*f_radix)(std::ios_base&))
    {	// double d = 213.99 ;
        // string str = Utility::BuiltInTypeToString(d) ;
		if (prec == 0)
		{
			prec = numeric_limits<T>::digits10;
		}
        std::ostringstream oss;
        //int prec=std::numeric_limits<T>::digits10; // 18
        //oss.precision(prec);// 覆盖默认精度  //
        //std::locale old_loc = oss.imbue(std::locale("C"));
        oss<<f_radix<<setprecision(prec)<<x;
        //oss.imbue(old_loc) ;
        return oss.str();
    }
    
    template<typename T>//string --->  T
    T Utility::BuiltInTypeFromString(const string& str, ios_base& (*f_radix)(std::ios_base&))
    {	// 缺省十进制，std::hex,std::oct
        std::istringstream iss(str);
        T t;
        if(!(iss>>f_radix>>t).fail())
        {
            return t;
        }
        else
        {
            return static_cast<T>(0);
        }
    }
    
    template<typename T>//MGWString --->  T
    T Utility::BuiltInTypeFromWString(const MGWString& wstr, ios_base& (*f_radix)(std::ios_base&))
    {																	// 缺省十进制，std::hex,std::oct
        std::wistringstream wiss(wstr);
        T t;
        if(!(wiss>>f_radix>>t).fail())
        {
            return t;
        }
        else
        {
            return static_cast<T>(0);
        }
    }
    
#if defined(_WIN32) || defined(_WIN64)
	bool initWinsock() ; // initiates use of WS2_32.DLL by a process.
	void __cdecl uninitWinsock() ;
    
	class Winsock2Setting : private NonCopyable
	{
	public:
		Winsock2Setting() ;
		~Winsock2Setting() ;
	};
#endif // #if defined(_WIN32) || defined(_WIN64)
}// namespace z11

#if defined(_WIN32) || defined(_WIN64)
// wifstream, wofstream,wfstream
#define WIOFSTREAM(wfs,wstr_path) ((wfs).open((wstr_path).c_str()))
#define WIOFSTREAM_EX(wfs,wstr_path, options) ((wfs).open((wstr_path).c_str(), (options) ))
// ifstream, ofstream,fstream
#define IOFSTREAM(ifs, wstr_path) ((ifs).open((wstr_path).c_str() ) )
#define IOFSTREAM_EX(ifs, wstr_path, options) ((ifs).open((wstr_path).c_str(), (options) ))
#define FILEPATH(wstr_path) (wstr_path.c_str() )
#elif defined(ANDROID_NDK)
// wifstream, wofstream,wfstream
#define WIOFSTREAM(wfs,wstr_path) ((wfs).open(z11::Utility::wstr2ustr(wstr_path).c_str()))
#define WIOFSTREAM_EX(wfs,wstr_path, options) ((wfs).open(z11::Utility::wstr2ustr(wstr_path).c_str(), (options) ))
// ifstream, ofstream,fstream
#define IOFSTREAM(ifs, wstr_path) ((ifs).open(z11::Utility::wstr2ustr(wstr_path).c_str() ) )
#define IOFSTREAM_EX(ifs, wstr_path, options) ((ifs).open(z11::Utility::wstr2ustr(wstr_path).c_str(), (options) ))
//#define OFSTREAM(ifs, wstr_path) ifs.open(wstr_path.c_str() ) ;
#define FILEPATH(wstr_path) (z11::Utility::wstr2ustr(wstr_path).c_str())


#else
#define WIOFSTREAM(wfs,wstr_path) ((wfs).open(MPPF::Utility::wstr2astr((wstr_path)).c_str()))
#define WIOFSTREAM_EX(wfs,wstr_path, options) ((wfs).open(MPPF::Utility::wstr2astr(wstr_path).c_str(), (options)))
#define IOFSTREAM(ifs, wstr_path) do{std::string curLocale = setlocale(LC_ALL, NULL);\
(ifs).open(MPPF::Utility::wstr2astr(wstr_path).c_str()); \
setlocale(LC_ALL, curLocale.c_str());}while(0);
#define IOFSTREAM_EX(ifs, wstr_path, options) do{std::string curLocale = setlocale(LC_ALL, NULL);\
(ifs).open(MPPF::Utility::wstr2astr(wstr_path).c_str(), (options) ); \
setlocale(LC_ALL, curLocale.c_str());	}while(0);
#define FILEPATH(wstr_path) ( z11::Utility::wstr2astr(wstr_path).c_str() )
#endif  // #if defined(_WIN32) || defined(_WIN64)

//
#if defined(_WIN32) || defined(_WIN64)
#ifndef INITWINSOCK 
#define INITWINSOCK MPPF::Winsock2Setting __The_Initializer_ ;
#endif 
#else // linux 
#ifndef INITWINSOCK 
#define INITWINSOCK 
#endif 
#endif //

//
#if defined(_WIN32) || defined(_WIN64)
#ifndef INITWINSOCK22 
#include <stdlib.h>
#define INITWINSOCK22 do{\
MPPF::initWinsock () ;		\
atexit (MPPF::uninitWinsock);\
}while(0) ;
#endif 
#else // linux 
#ifndef INITWINSOCK22 
#define INITWINSOCK22 
#endif 
#endif // #if defined(_WIN32) || defined(_WIN64)

#if defined(_WIN32) || defined(_WIN64)
#define barrier()  MemoryBarrier() 
#define ATOI_64 _atoi64 
#define ABS_64 _abs64
//#define sleep_ms(ms ) ::Sleep(ms ) //
#else
#define barrier()  __asm__ __volatile__ ("":::"memory")
#define ATOI_64 atoll
#define ABS_64 llabs
//#define sleep_ms(ms ) usleep((ms ) * 1000 ) 
#endif