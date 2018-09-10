#include "MGTime.h"
#include "MGLog.h"
#include <stdio.h>
#include "Utility.h"
#ifdef WIN32
#include <time.h>
#include <winsock2.h>
#include <windows.h>
#else //GNU
#include <time.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#ifdef WINCE

size_t strftime(
				char *strDest,
				size_t maxsize,
				const char *format,
				const struct tm *timeptr
				)
{
	return 0;
}
time_t time(time_t * timer)
{
	SYSTEMTIME sysTimeStruct;
	FILETIME fTime;
	ULARGE_INTEGER int64time;
	time_t locTT = 0;

	if ( timer == NULL ) {
		timer = &locTT;
	}

	GetSystemTime( &sysTimeStruct );
	if ( SystemTimeToFileTime( &sysTimeStruct, &fTime ) ) {
		memcpy( &int64time, &fTime, sizeof( FILETIME ) );
		/* Subtract the value for 1970-01-01 00:00 (UTC) */
		int64time.QuadPart -= 0x19db1ded53e8000;
		/* Convert to seconds. */
		int64time.QuadPart /= 10000000;
		*timer = int64time.QuadPart;
	}

	return *timer;
}
#endif
namespace z11
{
    int getMaxDayInMonth(int year, int month)
    {
        int day;
        switch (month)
        {
            case 1:
                day = 31;
                break;
            case 2:
                day = 28;
                break;
            case 3:
                day = 31;
                break;
            case 4:
                day = 30;
                break;
            case 5:
                day = 31;
                break;
            case 6:
                day = 30;
                break;
            case 7:
                day = 31;
                break;
            case 8:
                day = 31;
                break;
            case 9:
                day = 30;
                break;
            case 10:
                day = 31;
                break;
            case 11:
                day = 30;
                break;
            case 12:
                day = 31;
                break;
        }
        if (month == 2) {
            if( (year%4 == 0 && year%100 != 0) || (year%400==0) ) {
                day++;
            }
        }
        return day;
    }
    
	int MGTimeDelta::InDays() const
	{
		return static_cast<int>(delta_ / MGTime::kMicrosecondsPerDay);
	}

	int MGTimeDelta::InHours() const
	{
		return static_cast<int>(delta_ / MGTime::kMicrosecondsPerHour);
	}

	int MGTimeDelta::InMinutes() const
	{
		return static_cast<int>(delta_ / MGTime::kMicrosecondsPerMinute);
	}

	double MGTimeDelta::InSecondsF() const
	{
		return static_cast<double>(delta_) / MGTime::kMicrosecondsPerSecond;
	}

	int_64 MGTimeDelta::InSeconds() const
	{
		return delta_ / MGTime::kMicrosecondsPerSecond;
	}

	double MGTimeDelta::InMillisecondsF() const
	{
		return static_cast<double>(delta_) / MGTime::kMicrosecondsPerMillisecond;
	}

	int_64 MGTimeDelta::InMilliseconds() const
	{
		return delta_ / MGTime::kMicrosecondsPerMillisecond;
	}

	int_64 MGTimeDelta::InMillisecondsRoundedUp() const
	{
		return (delta_ + MGTime::kMicrosecondsPerMillisecond - 1) /
			MGTime::kMicrosecondsPerMillisecond;
	}

	int_64 MGTimeDelta::InMicroseconds() const
	{
		return delta_;
	}

	z11::MGTimeDelta MGTimeDelta::FromDaysF( double days )
	{
		return FromMicroseconds((int_64)(days*MGTime::kMicrosecondsPerDay));
	}

	z11::MGTimeDelta MGTimeDelta::FromHoursF( double hours )
	{
		return FromMicroseconds((int_64)(hours*MGTime::kMicrosecondsPerHour));
	}

	z11::MGTimeDelta MGTimeDelta::FromMinutesF( double minutes )
	{
		return FromMicroseconds((int_64)(minutes*MGTime::kMicrosecondsPerMinute));
	}

	z11::MGTimeDelta MGTimeDelta::FromSecondsF( double secs )
	{
		return FromMicroseconds((int_64)(secs*MGTime::kMicrosecondsPerSecond));
	}

	z11::MGTimeDelta MGTimeDelta::FromMillisecondsF( double ms )
	{
		return FromMicroseconds((int_64)(ms*MGTime::kMicrosecondsPerMillisecond));
	}

	// static
	MGTime MGTime::FromTimeT(time_t tt)
	{
		if(tt == 0)
		{
			return MGTime(); // 维持0表示不存在.
		}
		return MGTime(tt*kMicrosecondsPerSecond);
	}

	time_t MGTime::ToTimeT() const
	{
		if(us_ == 0)
		{
			return 0; // 维持0表示不存在.
		}
		return (time_t)(us_ / kMicrosecondsPerSecond);
	}

	// static
	MGTime MGTime::FromDoubleT(double dt)
	{
		if(dt == 0)
		{
			return MGTime(); // 维持0表示不存在.
		}
		return MGTime(static_cast<int_64>(dt * static_cast<double>(kMicrosecondsPerSecond)));
	}

	double MGTime::ToDoubleT() const
	{
		if(us_ == 0)
		{
			return 0; // 维持0表示不存在.
		}
		return (static_cast<double>(us_) / static_cast<double>(kMicrosecondsPerSecond));
	}

    bool MGTime::FromFormat(const std::string& format, MGTime &parsed_time)
    {
        bool ret = false;
        vector<string> str = Utility::SplitString(format, " ");
        if (str.size() == 2) {
            Exploded exploded;
            string str_section = str[0];
            const char *ch = str_section.c_str();
            while (*ch) {
                if ((*ch) < '0' || (*ch) > '9') {
                    break;
                }
                ch++;
            }
            
            //年月日//
            if (*ch != '\0') {
                vector<string> _str = Utility::SplitString(str_section, ch);
                if (_str.size() == 3) {
                    exploded.year = Utility::BuiltInTypeFromString<int>(_str[0]);
                    exploded.month = Utility::BuiltInTypeFromString<int>(_str[1]);
                    exploded.day_of_month = Utility::BuiltInTypeFromString<int>(_str[2]);
                    
                    str_section = str[1];
                    ch = str[1].c_str();
                    while (*ch) {
                        if ((*ch) < '0' || (*ch) > '9') {
                            break;
                        }
                        ch++;
                    }
                    //时分秒//
                    if (*ch != '\0') {
                        _str = Utility::SplitString(str_section, ch);
                        if (_str.size() == 3) {
                            exploded.hour = Utility::BuiltInTypeFromString<int>(_str[0]);
                            exploded.minute = Utility::BuiltInTypeFromString<int>(_str[1]);
                            exploded.second = Utility::BuiltInTypeFromString<int>(_str[2]);
                            exploded.day_of_week = 0;
                            exploded.millisecond = 0;
                            ret = exploded.HasValidValues();
                            if (ret) {
                                //parsed_time = FromLocalExploded(exploded);
                                
                                struct tm _tm;
                                time_t _time;
                                _tm.tm_year = exploded.year;
                                _tm.tm_mon = exploded.month;
                                _tm.tm_mday = exploded.day_of_month;
                                _tm.tm_hour = exploded.hour;
                                _tm.tm_min = exploded.minute;
                                _tm.tm_sec = exploded.second;
                                
                                _tm.tm_year -= 1900;
                                _tm.tm_mon --;
                                _tm.tm_isdst=-1;
                                
                                _time = mktime(&_tm);
                                parsed_time = MGTime(int_64(_time) * 1000000);
                            }
                        }
                    }
                }
            }
        }
        return ret;
    }

	// static
	bool MGTime::FromString(const wchar_t* time_string, MGTime* parsed_time)
	{
		DCHECK((time_string!=NULL) && (parsed_time!=NULL));
		std::string ascii_time_string = Utility::wstr2ustr(time_string);
		if(ascii_time_string.length() == 0)
		{
			return false;
		}
		//PRTime result_time = 0;
		//PRStatus result = PR_ParseTimeString(ascii_time_string.c_str(),
		//	PR_FALSE, &result_time);
		//if(PR_SUCCESS != result)
		//{
		//	return false;
		//}
		//result_time += kTimeTToMicrosecondsOffset;
		//*parsed_time = MGTime(result_time);
		//return true;
		return false;
	}

	std::string MGTime::CurTimeToString(bool is_local, const std::string& format)
	{
		Exploded ex;
		explode(ex, is_local);
		string ret = "";
		string last_string = format;
		string::size_type ch_end = 0;
		do
		{
			last_string = last_string.substr(ch_end);
			char ch = *(last_string.c_str());
			ch_end = last_string.find_first_not_of(ch);
			string object = last_string.substr(0, ch_end);
			string::size_type tmp_end = ch_end;
			if (tmp_end == string::npos) {
				tmp_end = last_string.size();
			}
			object = stringForKey(ex, object);
			tmp_end = (object.size() > tmp_end) ? tmp_end : object.size();
			object = object.substr(object.size()-tmp_end, tmp_end);
			ret.append(object);
		}while(ch_end != string::npos);
		return ret;
	}

	std::string MGTime::stringForKey(const Exploded& exploded, const std::string& key)
	{
		string ret = key;
		char ch = *(key.c_str());
		char times[32] = {'\0'};
		switch (ch)
		{
		case 'y'://year
			sprintf(times, "%d", exploded.year);
			ret = times;
			break;
		case 'M'://month
			sprintf(times, "%02d", exploded.month);
			ret = times;
			break;
		case 'd'://day
			sprintf(times, "%02d", exploded.day_of_month);
			ret = times;
			break;
		case 'H'://hour
			sprintf(times, "%02d", exploded.hour);
			ret = times;
			break;
		case 'm'://minute
			sprintf(times, "%02d", exploded.minute);
			ret = times;
			break;
		case 's'://second
			sprintf(times, "%02d", exploded.second);
			ret = times;
			break;
		default:
			break;
		}
		return ret;
	}

	inline bool is_in_range(int value, int lo, int hi)
	{
		return lo<=value && value<=hi;
	}

	bool MGTime::Exploded::HasValidValues() const
	{
		return is_in_range(month, 1, 12) &&
			is_in_range(day_of_week, 0, 6) &&
			is_in_range(day_of_month, 1, getMaxDayInMonth(year, month)) &&
			is_in_range(hour, 0, 23) &&
			is_in_range(minute, 0, 59) &&
			is_in_range(second, 0, 59) &&
			is_in_range(millisecond, 0, 999);
	}

} //namespace base


// Windows计时器基本介绍
//
// 一篇不错的文章: http://www.ddj.com/windows/184416651
// 一个mozilla bug: http://bugzilla.mozilla.org/show_bug.cgi?id=363258
//
// Windows缺省的计数器, GetSystemTimeAsFileTime精度并不高.
// 最好情况大约是15.5ms.
//
// QueryPerformanceCounter是实现高精度时钟不错的选择, 但在某些硬件上有问题,
// 有的时候会跳跃. 笔记本上, QPC调用代价很高. 台式机上比timeGetTime()慢3-4
// 倍, 笔记本上会慢10倍.
//
// 另外一种选择就是使用timeGetTime(), 精度是1ms, 但是只要调用APIs(
// timeBeginPeriod())就会影响系统上其它的应用. 缺省精度只有15.5ms.
// 因此很不幸, 我们不能调用timeBeginPeriod因为不想影响其它应用.
// 再者在移动平台调用更快的多媒体计时器会损害电池寿命. 参见intel文章:
// http://softwarecommunity.intel.com/articles/eng/1086.htm
//
// 为了解决问题, 还将使用timeGetTime(), 只在不是以电池供电运行的时候才提高
// 系统的计数器精度. 使用timeBeginPeriod(1)以便消息循环等待和时间测量精度
// 保持一致, 否则WaitForSingleObject(..., 1)在不被唤醒的时候最少堵塞15ms.

using z11::MGTime;
using z11::MGTimeDelta;
using z11::MGTimeTicks;

namespace z11
{
#ifdef WIN32
	// 据MSDN: FILETIME "用64位数值表示自January 1, 1601 (UTC)以来的百纳秒数".
	int_64 FileTimeToMicroseconds(const FILETIME& ft)
	{
		// bit_cast解决字节对齐问题, 除以10转换百纳秒到毫秒.
		// 仅在适合little-endian机器.
		//return bit_cast<int_64, FILETIME>(ft) / 10;
		return 0;
	}

	void MicrosecondsToFileTime(int_64 us, FILETIME* ft)
	{
		//DCHECK(us >= 0) << "MGTime is less than 0, negative values are not "
		//	"representable in FILETIME";

		// 乘以10转换毫秒到百纳秒. bit_cast解决字节对齐问题.
		// 仅在适合little-endian机器.
		//*ft = bit_cast<FILETIME, int_64>(us * 10);
	}
#endif

	// static
	MGTime MGTime::Now()
	{
#ifdef WIN32
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	return MGTime::FromInternalValue(time(NULL)*kMicrosecondsPerSecond+sys.wMilliseconds*kMicrosecondsPerMillisecond);
#else
	return MGTime::FromInternalValue(MGTimeTicks::Now().ticks_);
#endif
	return MGTime(0);
	}

	// static
	MGTime MGTime::fromExploded(const Exploded &exploded, bool is_local/* = true*/)
	{
#ifdef WINCE
		// 创建系统拆分时间结构, 代表本地时间或者UTC.
		SYSTEMTIME st;
		st.wYear = exploded.year;
		st.wMonth = exploded.month;
		st.wDayOfWeek = exploded.day_of_week;
		st.wDay = exploded.day_of_month;
		st.wHour = exploded.hour;
		st.wMinute = exploded.minute;
		st.wSecond = exploded.second;
		st.wMilliseconds = exploded.millisecond;

		// 转换到FILETIME.
		FILETIME ft;
		if(!SystemTimeToFileTime(&st, &ft))
		{
			//NOTREACHED() << "Unable to convert time";
			return MGTime(0);
		}

		// 确保是UTC.
		if(is_local)
		{
			FILETIME utc_ft;
			LocalFileTimeToFileTime(&ft, &utc_ft);
			return MGTime(FileTimeToMicroseconds(utc_ft));
		}
		return MGTime(FileTimeToMicroseconds(ft));
        
#else
        time_t timep = time(NULL);
		int_32 local_hour = localtime(&timep)->tm_hour;
		int_32 gm_hour = gmtime(&timep)->tm_hour;
		int_32 time_zone = local_hour-gm_hour;
        if(time_zone < 0)
        {
            time_zone += 24;
        }
        
        struct tm time_struct;
        time_struct.tm_year = exploded.year-1900;
        time_struct.tm_mon = exploded.month-1;
        time_struct.tm_mday = exploded.day_of_month;
        if(is_local)
        {
            time_struct.tm_hour = exploded.hour;
        }
        else
        {
            time_struct.tm_hour = exploded.hour+time_zone;
        }
        time_struct.tm_min = exploded.minute;
        time_struct.tm_sec = exploded.second;
        time_struct.tm_isdst = 0;
        time_struct.tm_yday = 0;
        time_struct.tm_wday = exploded.day_of_week;

#ifndef WIN32
		time_struct.tm_gmtoff = localtime(&timep)->tm_gmtoff;
#endif
        
        return MGTime(mktime(&time_struct)*kMicrosecondsPerSecond+exploded.millisecond*kMicrosecondsPerMillisecond);
#endif
	}

	void MGTime::explode(Exploded &exploded, bool is_local/*= true*/) const
	{
#ifdef WIN32
		if(isNull())
		{
			SYSTEMTIME sys;
			GetLocalTime(&sys);

			exploded.year = sys.wYear;
			exploded.month = sys.wMonth;
			exploded.day_of_month = sys.wDay;
			exploded.day_of_week = sys.wDayOfWeek;
			exploded.hour = sys.wHour;
			exploded.minute = sys.wMinute;
			exploded.second = sys.wSecond;
			exploded.millisecond = sys.wMilliseconds;
		}
		else
		{
			char tmp[256];
			time_t timep = ToTimeT();

			strftime(tmp, sizeof(tmp), "%Y/%m/%d %X %w", localtime(&timep));
			sscanf(tmp, "%d/%d/%d %d:%d:%d %d",
				&exploded.year,
				&exploded.month,
				&exploded.day_of_month,
				&exploded.hour,
				&exploded.minute,
				&exploded.second,
				&exploded.day_of_week);

			exploded.millisecond = (ToInternalValue()/kMicrosecondsPerMillisecond)%kMillisecondsPerSecond;
		}
#else	//GNU
		struct timeval now;
		gettimeofday(&now, NULL);

		time_t timep;
        if(isNull())
        {
            timep = time(NULL);
			exploded.millisecond = now.tv_usec/1000;
        }
        else
        {
            timep = ToTimeT();
			exploded.millisecond = (ToInternalValue()/kMicrosecondsPerMillisecond)%kMillisecondsPerSecond;
        }
        
		struct tm *p;
		if (is_local)
		{
			p = localtime(&timep); // 取得当地时间 //
		}
		else
		{
			p = gmtime(&timep);  // UTC 时间 //
		}

		exploded.year = 1900+p->tm_year;
		exploded.month = 1+p->tm_mon;
		exploded.day_of_week =p->tm_wday;
		exploded.day_of_month = p->tm_mday;
		exploded.hour = p->tm_hour;
		exploded.minute = p->tm_min;
		exploded.second = p->tm_sec;
#endif
	}

    Syncable MGTimeTicks::time_lock;
    
	// static
	MGTimeTicks MGTimeTicks::Now()
	{
#ifdef WIN32
// 		LARGE_INTEGER liTime, liFreq;
//         time_lock.lock();
// 		QueryPerformanceFrequency( &liFreq );
// 		QueryPerformanceCounter( &liTime );
//         double quadValue = 1000000.0 / (double)liFreq.QuadPart;
//         time_lock.unlock();
// 		return MGTimeTicks() + MGTimeDelta::FromMicroseconds(liTime.QuadPart * quadValue);
		return MGTimeTicks((int_64)(timeGetTime())*1000);
#else //GNU
		struct timeval now;
        time_lock.lock();
		gettimeofday(&now, NULL);
        time_lock.unlock();
		return MGTimeTicks() + MGTimeDelta::FromMicroseconds(now.tv_sec*MGTime::kMicrosecondsPerSecond + now.tv_usec);
		//return MGTimeTicks() + MGTimeDelta::FromMilliseconds((now.tv_sec*MGTime::kMillisecondsPerSecond + now.tv_usec/MGTime::kMicrosecondsPerMillisecond));
#endif
	}
}
