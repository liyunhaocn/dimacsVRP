////////////////////////////////
/// usage : 1.	utilities.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef vrptwNew_UTILITY_H
#define vrptwNew_UTILITY_H

#pragma warning(disable:4996)

#include <algorithm>
#include <chrono>
#include <initializer_list>
#include <vector>
#include <map>
#include <random>
#include <iostream>
#include <iomanip>

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>

#include "./Flag.h"

#define UTILITY_NOT_IMPLEMENTED  throw "Not implemented yet!";

// [on] use chrono instead of ctime in Timer.
#define UTILITY_TIMER_CPP_STYLE  1

// [off] use chrono instead of ctime in DateTime.
#define UTILITY_DATE_TIME_CPP_STYLE  0

namespace hust {

static void println() { std::cout << std::endl; }
template<typename T, typename ... Types>
static void println(const T& firstArg, const Types&... args) {

    //cout << "size of args: " << sizeof...(args) << endl;
    std::cout << firstArg << " ";
    println(args...);
}
template<typename T>
static void printve(T arr) {
    std::cout << " ";
    for (auto& i : arr) {
        std::cout << i << ",";
    }
    std::cout << std::endl;
}

template<typename T>
static Vec<int> putEleInVec(T arr) {
    Vec<int> ret;
    ret.reserve(arr.size());
    for (const auto& i : arr) {
        ret.push_back(i);
    }
    return ret;
}

struct MyString {
public:
    //split 
    Vec<std::string> split(std::string str, std::string s) {
        Vec<std::string> ret;
        if (s.size() == 0) {
            ret.push_back(str);
            return ret;
        }
        std::size_t pos = 0;
        while ((pos = str.find(s)) != std::string::npos) {
            if (pos > 0) {
                ret.push_back(str.substr(0, pos));
            }
            str = str.substr(pos + s.size());
        }
        if (str.size() > 0) {
            ret.push_back(str);
        }
        return ret;
    }
    //string to LL
    int str_int(std::string s) {
        std::stringstream ss;
        int ret;
        ss << s;
        ss >> ret;
        return ret;
    }

    std::string int_str(int s) {
        std::stringstream ss;
        ss << s;
        return ss.str();
    }
};

class Random {
public:
    using Generator = std::mt19937;

    Random(unsigned seed) : rgen(seed) {}
    Random() : rgen(generateSeed()) {}


    static int generateSeed() {
        return static_cast<int>(std::time(nullptr) + std::clock());
    }

    Generator::result_type operator()() { return rgen(); }

    // pick with probability of (numerator / denominator).
    bool isPicked(unsigned numerator, unsigned denominator) {
        return ((rgen() % denominator) < numerator);
    }

    // pick from [min, max).
    int pick(int min, int max) {
        return ((rgen() % (max - min)) + min);
    }
    // pick from [0, max).
    int pick(int max) {
        return (rgen() % max);
    }

    Generator rgen;
};

// count | 1 2 3 4 ...  k   k+1   k+2   k+3  ...  n
// ------|------------------------------------------
// index | 0 1 2 3 ... k-1   k    k+1   k+2  ... n-1
// prob. | 1 1 1 1 ...  1  k/k+1 k/k+2 k/k+3 ... k/n
class Sampling {
public:
    Sampling(Random &randomNumberGenerator, int targetNumber)
        : rgen(randomNumberGenerator), targetNum(targetNumber), pickCount(0) {}

    // return 0 for not picked.
    // return an integer i \in [1, targetNum] if it is the i_th item in the picked set.
    int isPicked() {
        if ((++pickCount) <= targetNum) {
            return pickCount;
        } else {
            int i = rgen.pick(pickCount) + 1;
            return (i <= targetNum) ? i : 0;
        }
    }

    // return -1 for no need to replace any item.
    // return an integer i \in [0, targetNum) as the index to be replaced in the picked set.
    int replaceIndex() {
        if (pickCount < targetNum) {
            return pickCount++;
        } else {
            int i = rgen.pick(++pickCount);
            return (i < targetNum) ? i : -1;
        }
    }

    void reset() {
        pickCount = 0;
    }

protected:
    Random &rgen;
    int targetNum;
    int pickCount;
};

class Timer {
public:
    #if UTILITY_TIMER_CPP_STYLE
    using Millisecond = std::chrono::milliseconds;
    using TimePoint = std::chrono::steady_clock::time_point;
    using Clock = std::chrono::steady_clock;
    #else
    using Millisecond = int;
    using TimePoint = int;
    struct Clock {
        static TimePoint now() { return clock(); }
    };
    #endif // UTILITY_TIMER_CPP_STYLE


    static constexpr double MillisecondsPerSecond = 1000;
    static constexpr double ClocksPerSecond = CLOCKS_PER_SEC;
    static constexpr int ClocksPerMillisecond = static_cast<int>(ClocksPerSecond / MillisecondsPerSecond);


    #if UTILITY_TIMER_CPP_STYLE
    Timer(const Millisecond& duration, const TimePoint& st = Clock::now(), std::string name = "")
        : duration(duration), startTime(st), endTime(startTime + duration), name(name) {}

    Timer(LL duration, const TimePoint& st = Clock::now(), std::string name = "")
        : duration(Millisecond(duration * 1000)), startTime(st), endTime(startTime + Millisecond(duration * 1000)), name(name) {}

    #else
    Timer(const Millisecond& duration, const TimePoint& st = Clock::now())
        : startTime(st), endTime(startTime + duration * ClocksPerMillisecond) {}
    #endif // UTILITY_TIMER_CPP_STYLE

    static Millisecond durationInMillisecond(const TimePoint& start, const TimePoint& end) {
        #if UTILITY_TIMER_CPP_STYLE
        return std::chrono::duration_cast<Millisecond>(end - start);
        #else
        return (end - start) / ClocksPerMillisecond;
        #endif // UTILITY_TIMER_CPP_STYLE
    }

    static double durationInSecond(const TimePoint& start, const TimePoint& end) {
        #if UTILITY_TIMER_CPP_STYLE
        return std::chrono::duration_cast<Millisecond>(end - start).count() / MillisecondsPerSecond;
        #else
        return (end - start) / ClocksPerSecond;
        #endif // UTILITY_TIMER_CPP_STYLE
    }

    static Millisecond toMillisecond(double second) {
        #if UTILITY_TIMER_CPP_STYLE
        return Millisecond(static_cast<int>(second * MillisecondsPerSecond));
        #else
        return static_cast<Millisecond>(second * MillisecondsPerSecond);
        #endif // UTILITY_TIMER_CPP_STYLE
    }

    // there is no need to free the pointer. the format of the format string is 
    // the same as std::strftime() in http://en.cppreference.com/w/cpp/chrono/c/strftime.
    static const char* getLocalTime(const char* format = "%Y-%m-%d(%a)%H:%M:%S") {
        static constexpr int DateBufSize = 64;
        static char buf[DateBufSize];
        time_t t = time(NULL);
        tm* date = localtime(&t);
        strftime(buf, DateBufSize, format, date);
        return buf;
    }
    static const char* getTightLocalTime() { return getLocalTime("%Y%m%d%H%M%S"); }

    bool isTimeOut() const {
        return (Clock::now() > endTime);
    }

    TimePoint getNow() {
        return (Clock::now());
    }

    Millisecond restMilliseconds() const {
        return durationInMillisecond(Clock::now(), endTime);
    }

    double restSeconds() const {
        return durationInSecond(Clock::now(), endTime);
    }

    Millisecond elapsedMilliseconds() const {
        return durationInMillisecond(startTime, Clock::now());
    }

    double elapsedSeconds() const {
        return durationInSecond(startTime, Clock::now());
    }

    const TimePoint& getStartTime() const { return startTime; }
    const TimePoint& getEndTime() const { return endTime; }

    void disp() {
        std::cout << name << "run :" << elapsedSeconds() << "s" << std::endl;
    }

    double getRunTime() {
        return durationInSecond(startTime, Clock::now());
    }

    bool reStart() {
        startTime = Clock::now();
        endTime = startTime + duration;
        return true;
    }

    bool setLenUseSecond(LL s) {
        duration = Millisecond(s * 1000);
        return true;
    }
protected:
    TimePoint startTime;
    TimePoint endTime;
    Millisecond duration;
    std::string name;
};

class DateTime {
public:
    static constexpr int MinutesPerDay = 60 * 24;
    static constexpr int MinutesPerHour = 60;
    static constexpr int SecondsPerMinute = 60;


    // TODO[szx][8]: use different names for the arguments.
    DateTime(int year = 0, int month = 0, int day = 0, int hour = 0, int minute = 0, int second = 0)
        : year(year), month(month), day(day), hour(hour), minute(minute), second(second) {}
    DateTime(tm &t) : DateTime(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min) {}
    DateTime(time_t t) : DateTime(*std::localtime(&t)) {}

    // get an inconsistent tm struct which requires std::mktime() to revise.
    operator std::tm() const {
        std::tm datetime;

        datetime.tm_year = year - 1900;
        datetime.tm_mon = month - 1;
        datetime.tm_mday = day;
        datetime.tm_hour = hour;
        datetime.tm_min = minute;
        datetime.tm_sec = second;

        datetime.tm_isdst = -1;

        //datetime.tm_wday = 0; // ignored by mktime().
        //datetime.tm_yday = 0; // ignored by mktime().

        return datetime;
    }

    operator time_t() const {
        std::tm t = static_cast<std::tm>(*this);
        return std::mktime(&t);
    }

    friend DateTime operator+(const DateTime &dateTime, time_t second) {
        time_t t = static_cast<time_t>(dateTime);
        t += second;
        return DateTime(t);
    }
    friend DateTime operator-(const DateTime &dateTime, time_t second) { return (dateTime + (-second)); }

    friend std::ostream& operator<<(std::ostream &os, DateTime &dateTime) {
        os << dateTime.year << '-'
            << std::setw(2) << std::setfill('0') << dateTime.month << '-'
            << std::setw(2) << std::setfill('0') << dateTime.day << ' '
            << std::setw(2) << std::setfill('0') << dateTime.hour << ':'
            << std::setw(2) << std::setfill('0') << dateTime.minute << ':'
            << std::setw(2) << std::setfill('0') << dateTime.second;
        return os;
    }

    static double durationInSecond(const DateTime &l, const DateTime &r) {
        #if UTILITY_DATE_TIME_CPP_STYLE
        using Clock = std::chrono::system_clock;
        using TimePoint = Clock::time_point;
        TimePoint tpl = Clock::from_time_t(static_cast<time_t>(l));
        TimePoint tpr = Clock::from_time_t(static_cast<time_t>(r));
        return std::chrono::duration_cast<std::chrono::seconds>(tpl - tpr).count();
        #else
        return std::difftime(static_cast<time_t>(l), static_cast<time_t>(r));
        #endif // UTILITY_DATE_TIME_CPP_STYLE
    };

    int year; // year since Common Era.
    int month; // months in year.
    int day; // days in month.
    int hour; // hours in day.
    int minute; // minutes in hour.
    int second; // seconds in minute.
};

class Log {
public:
    using Manipulator = std::ostream& (*)(std::ostream&);


    enum Level {
        On,
        Off, // the default state if not specified.

        Fatal = On,
        Error = On,
        Warning = On,
        Debug = On,
        Info = On, // = Off.
    };

#define SZX_DEBUG 1

    #if SZX_DEBUG
    static bool isTurnedOn(int level) { return (level == On); }
    static bool isTurnedOff(int level) { return !isTurnedOn(level); }
    #else
    static bool isTurnedOn(int level) { return false; }
    static bool isTurnedOff(int level) { return true; }
    #endif // SZX_DEBUG


    Log(int logLevel, std::ostream &logFile) : level(logLevel), os(logFile) {}
    Log(int logLevel) : Log(logLevel, std::cerr) {}

    template<typename T>
    Log& operator<<(const T &obj) {
        if (isTurnedOn(level)) { os << obj; }
        return *this;
    }
    Log& operator<<(Manipulator manip) {
        if (isTurnedOn(level)) { os << manip; }
        return *this;
    }

protected:
    int level;
    std::ostream &os;
};

struct Union {

    Vec<int> fa;
    Vec<int> rank;
    Union(int maxSize) {
        fa = Vec<int>(maxSize);
        rank = Vec<int>(maxSize);
        int n = fa.size();
        for (int i = 0; i < n; ++i)
        {
            fa[i] = i;
            rank[i] = 1;
        }
    }


    int find(int x)
    {
        return x == fa[x] ? x : (fa[x] = find(fa[x]));
    }

    inline void merge(int i, int j)
    {
        int x = find(i), y = find(j);
        if (rank[x] <= rank[y])
            fa[x] = y;
        else
            fa[y] = x;
        if (rank[x] == rank[y] && x != y)
            rank[y]++;
    }

};

}


#endif // vrptwNew_UTILITY_H
