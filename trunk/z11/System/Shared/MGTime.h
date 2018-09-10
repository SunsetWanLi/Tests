//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGTime.h)
/// @brief (Time 类，提供计时功能)
///
///( MGTime表示一个绝对的时间点, 由于依赖平台的起始时间, 内部用微秒(s/1,000,000)表示.
///	MGTimeDelta表示一段时间, 内部用微秒表示. MGTimeTicks表示一个递增的抽象时间, 用于计算时间差, 内部用微秒表示.
/// 它无法转换成可读的时间, 确保不会减小.(用户修改时间, MGTime::Now() 可能会减小或者猛增
/// 由于内部用64位数值表示, 所以按值传递是高效的.)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-07-22)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
//例如如果要计算两个时间之差
//可以通过计算两次MGTimeTicks::Now之差就可以了
//得到的对象是TimeDelta，如果需要转换为毫秒，可以考虑InMilliseconds，要可以考虑其浮点数版本InMillisecondsF
//TimeDelta也可以转换为其他的类型 例如InDays()，InHours()，InMinutes()，InSecondsF()，InSeconds()
//
#ifndef _MGTIME_H_
#define _MGTIME_H_
#pragma once
#include "../../CommonBase/CommonDef.h"
#include <string>
#include <time.h>
#include "../../System/Shared/Syncable.h"
namespace z11
{
    int getMaxDayInMonth(int year, int month);
	class MGTime;
	class MGTimeTicks;
	class MGTimeDelta
	{
	public:
		MGTimeDelta() : delta_(0) {}

		// 转换时间到TimeDelta. //
		static MGTimeDelta FromDays(int_64 days);
		static MGTimeDelta FromHours(int_64 hours);
		static MGTimeDelta FromMinutes(int_64 minutes);
		static MGTimeDelta FromSeconds(int_64 secs);
		static MGTimeDelta FromMilliseconds(int_64 ms);
		static MGTimeDelta FromMicroseconds(int_64 us);
		//double 版 //
		static MGTimeDelta FromDaysF(double days);
		static MGTimeDelta FromHoursF(double hours);
		static MGTimeDelta FromMinutesF(double minutes);
		static MGTimeDelta FromSecondsF(double secs);
		static MGTimeDelta FromMillisecondsF(double ms);

		// 返回TimeDelta对象的内部数值. 不要直接做算术运算, 应该使用提供的操作函数. //
		int_64 ToInternalValue() const
		{
			return delta_;
		}

		// 返回TimeDelta的一些单位. F版本返回浮点类型, 其它的返回取整后的数值. //
		//
		// InMillisecondsRoundedUp()取整到下一微秒. //
		int InDays() const;
		int InHours() const;
		int InMinutes() const;
		double InSecondsF() const;
		int_64 InSeconds() const;
		double InMillisecondsF() const;
		int_64 InMilliseconds() const;
		int_64 InMillisecondsRoundedUp() const;
		int_64 InMicroseconds() const;

		MGTimeDelta& operator = (const MGTimeDelta &other)
		{
			delta_ = other.delta_;
			return *this;
		}

		// 与其它TimeDelta运算.
		MGTimeDelta operator + (const MGTimeDelta &other) const
		{
			return MGTimeDelta(delta_ + other.delta_);
		}
		MGTimeDelta operator - (const MGTimeDelta &other) const
		{
			return MGTimeDelta(delta_ - other.delta_);
		}

		MGTimeDelta& operator += (const MGTimeDelta &other)
		{
			delta_ += other.delta_;
			return *this;
		}
		MGTimeDelta& operator -= (const MGTimeDelta &other)
		{
			delta_ -= other.delta_;
			return *this;
		}
		MGTimeDelta operator - () const
		{
			return MGTimeDelta(-delta_);
		}

		// 与整数运算, 只能做乘除法, 加减运算应该使用上面的操作. //
		MGTimeDelta operator * (int_64 a) const
		{
			return MGTimeDelta(delta_ * a);
		}
		MGTimeDelta operator / (int_64 a) const
		{
			return MGTimeDelta(delta_ / a);
		}
		MGTimeDelta& operator *= (int_64 a)
		{
			delta_ *= a;
			return *this;
		}
		MGTimeDelta& operator /= (int_64 a)
		{
			delta_ /= a;
			return *this;
		}
		int_64 operator / (const MGTimeDelta &a) const
		{
			return delta_ / a.delta_;
		}

		// 实现在后面, 因为依赖其它类的定义. //
		MGTime operator + (const MGTime &time) const;
		MGTimeTicks operator + (const MGTimeTicks &ticks) const;

		// 比较操作. //
		bool operator == (const MGTimeDelta &other) const
		{
			return delta_ == other.delta_;
		}
		bool operator != (const MGTimeDelta &other) const
		{
			return delta_ != other.delta_;
		}
		bool operator < (const MGTimeDelta &other) const
		{
			return delta_ < other.delta_;
		}
		bool operator <= (const MGTimeDelta &other) const
		{
			return delta_ <= other.delta_;
		}
		bool operator > (const MGTimeDelta &other) const
		{
			return delta_ > other.delta_;
		}
		bool operator >= (const MGTimeDelta &other) const
		{
			return delta_ >= other.delta_;
		}

	private:
		friend class MGTime;
		friend class MGTimeTicks;
		friend MGTimeDelta operator * (int_64 a, const MGTimeDelta &td);

		// 用微秒时间差构造TimeDelta对象. 函数私有化以避免用户直接构造. //
		// 使用FromSeconds、FromMilliseconds等函数替换. //
		explicit MGTimeDelta(int_64 delta_us) : delta_(delta_us) {}

		// 微秒时间差. //
		int_64 delta_;
	};

	class MGTime
	{
	public:
		static const int_64 kMillisecondsPerSecond = 1000;
		static const int_64 kMicrosecondsPerMillisecond = 1000;
		static const int_64 kMicrosecondsPerSecond = kMicrosecondsPerMillisecond *
			kMillisecondsPerSecond;
		static const int_64 kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
		static const int_64 kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
		static const int_64 kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
		static const int_64 kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
		static const int_64 kNanosecondsPerMicrosecond = 1000;
		static const int_64 kNanosecondsPerSecond = kNanosecondsPerMicrosecond *
			kMicrosecondsPerSecond;

		// 拆分过的时间, 以便更好的格式化. 类似于Win32的SYSTEMTIME结构体. //
		struct Exploded
		{
			int year;          // 四位数字表示年 "2007" //
			int month;         // 从1起始的月份 (数值1==一月, 类推) //
			int day_of_week;   // 从0起始的工作日 (0==星期日, 类推) //
			int day_of_month;  // 从1起始的天 (1-31) //
			int hour;          // 时 (0-23) //
			int minute;        // 分 (0-59) // 
			int second;        // 秒 (0-59, 正闰秒可能会导致超过60) //
			int millisecond;   // 毫秒 (0-999) //

			// 简单的校验数据成员是否在各自范围以内. 'true'并不代表可以成功 //
			// 转换成一个Time. //
			bool HasValidValues() const;
		};

		// NULL时间. 使用Time::Now()获取当前时间. //
		explicit MGTime() : us_(0) {}

		// 如果对象没有初始化, 返回true. //
		bool isNull() const
		{
			return us_ == 0;
		}

		// 返回当前时间. 注意系统时间的改变会导致时间退后, 无法保证时间是严格递增 //
		// 的, 也就是无法保证两次调用Now()的时间一定是不同的. 从1970年开始. //
		static MGTime Now();

		// UTC的time_t和Time类之间的转换.
		static MGTime FromTimeT(time_t tt);
		time_t ToTimeT() const;

		// Time和double表示的自起始时间(Jan 1, 1970)计秒之间的转换. Webkit使用这种  //
		// 表示方式. 因为WebKit初始化double时间值0表示未初始化, 这里也映射一个空的 //
		// 对象表示未初始化. //
		static MGTime FromDoubleT(double dt);
		double ToDoubleT() const;

		// 转换整数值到Time, 用已知的兼容数据反序列化|Time|结构. 不提供这样的 //
		// 构造函数是因为从调用者的视角整数类型无法确定是合法的时间. //
		static MGTime FromInternalValue(int_64 us)
		{
			return MGTime(us);
		}
        
        //年月日跟时分秒之间一定要用空格来分割，年月日与时分秒之中不能用空格来分割//
        //right:    2012-10-16 17:20:22//
        //error:    2012 10 16 17:20:22//
        //error:    2012-10/16 17:20:22//
        static bool FromFormat(const std::string& format, MGTime &parsed_time);

		// 从字符串转换到时间对象. 例如: "Tue, 15 Nov 1994 12:45:26 GMT". //
		// 如果没有指定时区, 使用本地时间. //
		static bool FromString(const wchar_t* time_string, MGTime* parsed_time);

		// 序列化, 用FromInternalValue可重新构造对象. //
		// 不要直接做算术运算, 应该使用提供的操作函数. //
		int_64 ToInternalValue() const
		{
			return us_;
		}

		// 拆分Time到本地时间(|is_local = true|)或者UTC(|is_local = false|). //
		void explode(Exploded &exploded, bool is_local = true) const;
        
		// 本地时间(|is_local = true|)或者UTC(|is_local = false|)组合成Time. //
		static MGTime fromExploded(const Exploded &exploded, bool is_local = true);

		// 返回当前日期 format为格式 如:yyyy-MM-dd HH:mm:ss
		// 字符串顺序可以变换 如:yyyy/dd/MM HH:mm
		std::string UTCCurTimeToString(const std::string& format)
		{
			return CurTimeToString(false, format);
		}
		std::string LocalCurTimeToString(const std::string& format)
		{
			return CurTimeToString(true, format);
		}

		MGTime& operator = (const MGTime &other)
		{
			us_ = other.us_;
			return *this;
		}

		// 计算时间差. //
		MGTimeDelta operator - (const MGTime &other) const
		{
			return MGTimeDelta(us_ - other.us_);
		}

		// 修改时间. //
		MGTime& operator += (const MGTimeDelta &delta)
		{
			us_ += delta.delta_;
			return *this;
		}
		MGTime& operator -= (const MGTimeDelta &delta)
		{
			us_ -= delta.delta_;
			return *this;
		}

		// 返回新时间. //
		MGTime operator + (const MGTimeDelta &delta) const
		{
			return MGTime(us_ + delta.delta_);
		}
		MGTime operator-(const MGTimeDelta &delta) const
		{
			return MGTime(us_ - delta.delta_);
		}

		// 比较操作. //
		bool operator == (const MGTime &other) const
		{
			return us_ == other.us_;
		}
		bool operator != (const MGTime &other) const
		{
			return us_ != other.us_;
		}
		bool operator < (const MGTime &other) const
		{
			return us_ < other.us_;
		}
		bool operator <= (const MGTime &other) const
		{
			return us_ <= other.us_;
		}
		bool operator > (const MGTime &other) const
		{
			return us_ > other.us_;
		}
		bool operator >= (const MGTime &other) const
		{
			return us_ >= other.us_;
		}

	private:
        //根据字符串key，从exploded中获取对应年月日字符串 //
        std::string stringForKey(const Exploded &exploded, const std::string &key);
        // 返回当前日期 format为格式 如:yyyy-MM-dd HH:mm:ss //
        std::string CurTimeToString(bool is_local, const std::string &format);

		friend class MGTimeDelta;

		explicit MGTime(int_64 us) : us_(us) {}

		// UTC微秒数表示时间.  //
		int_64 us_;
	};

	inline MGTime MGTimeDelta::operator + (const MGTime &time) const
	{
		return MGTime(time.us_ + delta_);
	}

	// static
	inline MGTimeDelta MGTimeDelta::FromDays(int_64 days)
	{
		return MGTimeDelta(days * MGTime::kMicrosecondsPerDay);
	}

	// static
	inline MGTimeDelta MGTimeDelta::FromHours(int_64 hours)
	{
		return MGTimeDelta(hours * MGTime::kMicrosecondsPerHour);
	}

	// static
	inline MGTimeDelta MGTimeDelta::FromMinutes(int_64 minutes)
	{
		return MGTimeDelta(minutes * MGTime::kMicrosecondsPerMinute);
	}

	// static
	inline MGTimeDelta MGTimeDelta::FromSeconds(int_64 secs)
	{
		return MGTimeDelta(secs * MGTime::kMicrosecondsPerSecond);
	}

	// static
	inline MGTimeDelta MGTimeDelta::FromMilliseconds(int_64 ms)
	{
		return MGTimeDelta(ms * MGTime::kMicrosecondsPerMillisecond);
	}

	// static
	inline MGTimeDelta MGTimeDelta::FromMicroseconds(int_64 us)
	{
		return MGTimeDelta(us);
	}

	class MGTimeTicks
	{
	public:
		MGTimeTicks() : ticks_(0) {}

		// 平台相关的计数器表示"right now".  //
		// 时钟的分辨率是1-15ms, 具体依赖硬件/软件配置. 只能用于程序内计时，同一次运行，起始值一样，不同运行，起始不一定一样. //
		static MGTimeTicks Now();

		// 如果对象没初始化, 返回true.  //
		bool isNull() const
		{
			return ticks_ == 0;
		}

		// 返回TimeTicks对象内部数值. //
		int_64 ToInternalValue() const
		{
			return ticks_;
		}

		MGTimeTicks& operator = (const MGTimeTicks &other)
		{
			ticks_ = other.ticks_;
			return *this;
		}

		// 计算差值.  //
		MGTimeDelta operator - (const MGTimeTicks &other) const
		{
			return MGTimeDelta(ticks_ - other.ticks_);
		}

		// 修改.  //
		MGTimeTicks& operator += (const MGTimeDelta &delta)
		{
			ticks_ += delta.delta_;
			return *this;
		}
		MGTimeTicks& operator -= (const MGTimeDelta &delta)
		{
			ticks_ -= delta.delta_;
			return *this;
		}

		// 返回新值.  //
		MGTimeTicks operator + (const MGTimeDelta &delta) const
		{
			return MGTimeTicks(ticks_ + delta.delta_);
		}
		MGTimeTicks operator - (const MGTimeDelta &delta) const
		{
			return MGTimeTicks(ticks_ - delta.delta_);
		}

		// 比较操作.  //
		bool operator == (const MGTimeTicks &other) const
		{
			return ticks_ == other.ticks_;
		}
		bool operator != (const MGTimeTicks &other) const
		{
			return ticks_ != other.ticks_;
		}
		bool operator < (const MGTimeTicks &other) const
		{
			return ticks_ < other.ticks_;
		}
		bool operator <= (const MGTimeTicks &other) const
		{
			return ticks_ <= other.ticks_;
		}
		bool operator > (const MGTimeTicks &other) const
		{
			return ticks_ > other.ticks_;
		}
		bool operator >= (const MGTimeTicks &other) const
		{
			return ticks_ >= other.ticks_;
		}

	protected:
		friend class MGTimeDelta;
		friend class MGTime;
        static Syncable time_lock;
		// 请使用Now()构造新对象. 本构造函数内部使用. 用微秒计数. //
		explicit MGTimeTicks(int_64 ticks) : ticks_(ticks) {}

		// 微秒计数器. //
		int_64 ticks_;
	};

	inline MGTimeTicks MGTimeDelta::operator + (const MGTimeTicks &ticks) const
	{
		return MGTimeTicks(ticks.ticks_ + delta_);
	}

}
#endif //_MGTIME_H_
