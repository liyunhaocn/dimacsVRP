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

using namespace std;

namespace hust {

// if there is "#define x  y", VERBATIM_STRINGIFY(x) will get "x".
#define VERBATIM_STRINGIFY(x)  #x
// if there is "#define x  y", RESOLVED_STRINGIFY(x) will get "y".
#define RESOLVED_STRINGIFY(x)  VERBATIM_STRINGIFY(x)

#define VERBATIM_CONCAT(a, b)  a##b
#define VERBATIM_CONCAT2(a, b, c)  a##b##c
#define VERBATIM_CONCAT3(a, b, c, d)  a##b##c##d
#define RESOLVED_CONCAT(a, b)  VERBATIM_CONCAT(a, b)
#define RESOLVED_CONCAT2(a, b, c)  VERBATIM_CONCAT2(a, b, c)
#define RESOLVED_CONCAT3(a, b, c, d)  VERBATIM_CONCAT3(a, b, c, d)

void println() { cout << endl; }
template<typename T, typename ... Types>
void println(const T& firstArg, const Types&... args) {

    //cout << "size of args: " << sizeof...(args) << endl;
    cout << firstArg << " ";
    println(args...);
}
template<typename T>
void printve(T arr) {
    cout << " ";
    for (auto& i : arr) {
        cout << i << ",";
    }
    cout << endl;
}

struct MyString {
public:
    //split 
    vector<string> split(string str, string s) {
        vector<string> ret;
        if (s.size() == 0) {
            ret.push_back(str);
            return ret;
        }
        std::size_t pos = 0;
        while ((pos = str.find(s)) != string::npos) {
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
    int str_int(string s) {
        stringstream ss;
        int ret;
        ss << s;
        ss >> ret;
        return ret;
    }

    string int_str(int s) {
        stringstream ss;
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
    Timer(const Millisecond &duration, const TimePoint &st = Clock::now(),string name="")
        : duration(duration),startTime(st), endTime(startTime + duration),name(name) {}

    Timer(LL duration, const TimePoint& st = Clock::now(), string name = "")
        : duration(Millisecond(duration * 1000)),startTime(st), endTime(startTime + Millisecond(duration*1000) ), name(name) {}

    #else
    Timer(const Millisecond &duration, const TimePoint &st = Clock::now())
        : startTime(st), endTime(startTime + duration * ClocksPerMillisecond) {}
    #endif // UTILITY_TIMER_CPP_STYLE

    static Millisecond durationInMillisecond(const TimePoint &start, const TimePoint &end) {
        #if UTILITY_TIMER_CPP_STYLE
        return std::chrono::duration_cast<Millisecond>(end - start);
        #else
        return (end - start) / ClocksPerMillisecond;
        #endif // UTILITY_TIMER_CPP_STYLE
    }

    static double durationInSecond(const TimePoint &start, const TimePoint &end) {
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
    static const char* getLocalTime(const char *format = "%Y-%m-%d(%a)%H:%M:%S") {
        static constexpr int DateBufSize = 64;
        static char buf[DateBufSize];
        time_t t = time(NULL);
        tm *date = localtime(&t);
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
        cout << name << "run :" << elapsedSeconds() << "s" << endl;
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
    string name;
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
        Info, // = Off.
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


template<typename ArbitraryId = int, typename ConsecutiveId = int, const ConsecutiveId DefaultIdNumberHint = 1024>
class ZeroBasedConsecutiveIdMap {
public:
    ZeroBasedConsecutiveIdMap(ConsecutiveId idNumberHint = DefaultIdNumberHint) : count(-1) {
        idList.reserve(static_cast<size_t>(idNumberHint));
    }


    // track a new arbitrary ID or return the sequence of a tracked one.
    ConsecutiveId toConsecutiveId(ArbitraryId arbitraryId) {
        auto iter = idMap.find(arbitraryId);
        if (iter != idMap.end()) { return iter->second; }
        idList.push_back(arbitraryId);
        return (idMap[arbitraryId] = (++count));
    }

    // return the consecutiveId_th tracked arbitrary ID.
    ArbitraryId toArbitraryId(ConsecutiveId consecutiveId) const { return idList[consecutiveId]; }

    bool isConsecutiveIdExist(ConsecutiveId consecutiveId) const { return (consecutiveId <= count); }
    bool isArbitraryIdExist(ArbitraryId arbitraryId) const { return (idMap.find(arbitraryId) != idMap.end()); }


    // number of tracked IDs.
    ConsecutiveId count;
    // idList[consecutiveId] == arbitraryId.
    std::vector<ArbitraryId> idList;
    // idMap[arbitraryId] == consecutiveId.
    std::map<ArbitraryId, ConsecutiveId> idMap;
};


template<typename Unit>
struct Interval {
    Interval() {}
    Interval(Unit intervalBegin, Unit intervalEnd) : begin(intervalBegin), end(intervalEnd) {}

    bool cover(Unit x) const { return ((begin <= x) && (x < end)); }
    bool cover(const Interval &i) const { return ((begin <= i.begin) && (i.end <= end)); }
    bool beginBefore(Unit x) const { return (begin < x); }
    bool beginBefore(const Interval &i) const { return (begin < i.begin); }
    bool endBefore(Unit x) const { return (end <= x); }
    bool endBefore(const Interval &i) const { return (end < i.end); }
    // return true if this is strictly before i (no overlap).
    bool before(const Interval &i) const { return (end <= i.begin); }

    bool isValid() const { return (begin < end); }
    static bool isValid(const Interval &i) { return i.isValid(); }

    static bool isOverlapped(const Interval &l, const Interval &r) {
        return ((l.begin < r.end) && (r.begin < l.end));
    }

    // vector measurement of the interval span.
    Unit displacement() const { return (end - begin); }
    // scalar measurement of the interval span.
    Unit length() const { return std::abs(end - begin); }

    // return the intersection of l and r if they are overlapped,
    // or the reversed gap between them if there is no intersection.
    static Interval overlap(const Interval &l, const Interval &r) {
        return Interval( (std::max)(l.begin, r.begin), (std::min)(l.end, r.end));
    }

    // return the length of the blank space between l and r if they are not interseted,
    // or the opposite number of the minimal distance to make them mutually exclusive.
    static Unit gap(const Interval &l, const Interval &r) {
        if (l.begin < r.begin) {
            if (l.end < r.end) {
                return r.begin - l.end;
            } else { // if (l.end >= r.end)
                return (std::max)(r.begin - l.end, l.begin - r.end);
            }
        } else { // if (l.begin >= r.end)
            if (l.end < r.end) {
                return (std::max)(r.begin - l.end, l.begin - r.end);
            } else { // if (l.end >= r.end)
                return l.begin - r.end;
            }
        }
    }

    Unit begin;
    Unit end;
};

class System {
public:
    struct Shell {
        struct Common {
            static std::string RedirectStdin() { return " 0< "; }
            static std::string RedirectStdout() { return " 1> "; }
            static std::string RedirectStderr() { return " 2> "; }
            static std::string RedirectStdout_app() { return  " 1>> "; }
            static std::string RedirectStderr_app() { return  " 2>> "; }
        };
        struct Win32 : public Common {
            static std::string Mkdir() { return " mkdir "; }
            static std::string NullDev() { return " nul "; }
        };
        struct Unix : public Common {
            static std::string Mkdir() { return  " mkdir -p "; }
            static std::string NullDev() { return  " /dev/null "; }

        };
    };

    #if _OS_MS_WINDOWS
    using Cmd = Shell::Win32;
    #else
    using Cmd = Shell::Unix;
    #endif // _OS_MS_WINDOWS

    static int exec(const std::string &cmd) { return system(cmd.c_str()); }

    static std::string quote(const std::string &s) { return ('\"' + s + '\"'); }

    static void makeSureDirExist(const std::string &dir) {
        exec(Cmd::Mkdir() + quote(dir) + Cmd::RedirectStderr() + Cmd::NullDev());
    }

    struct MemorySize {
        using Unit = long long;

        static constexpr Unit Base = 1024;

        friend std::ostream& operator<<(std::ostream &os, const MemorySize &memSize) {
            auto units = { "B", "KB", "MB", "GB", "TB", "PB" };
            double size = static_cast<double>(memSize.size);
            for (auto u = units.begin(); u != units.end(); ++u, size /= Base) {
                if (size < Base) {
                    os << std::setprecision(4) << size << *u;
                    break;
                }
            }

            return os;
        }

        Unit size;
    };

    struct MemoryUsage {
        MemorySize physicalMemory;
        MemorySize virtualMemory;
    };

    static MemoryUsage memoryUsage();
    static MemoryUsage peakMemoryUsage();
};

class Math {
public:
    static constexpr double DefaultTolerance = 0.01;

    static bool weakEqual(double l, double r, double tolerance = DefaultTolerance) {
        return (std::abs(l - r) < tolerance);
    }
    static bool weakLess(double l, double r, double tolerance = DefaultTolerance) { // operator<=().
        return ((l - r) < tolerance);
    }
    static bool strongLess(double l, double r, double tolerance = DefaultTolerance) { // operator<().
        return ((l - r) < -tolerance);
    }

    static double floor(double d) { return std::floor(d + DefaultTolerance); }
    static long lfloor(double d) { return static_cast<long>(d + DefaultTolerance); }

    template<typename T>
    static bool isOdd(T integer) { return ((integer % 2) == 1); }
    template<typename T>
    static bool isEven(T integer) { return ((integer % 2) == 0); }

    template<typename T>
    static T bound(T num, T lb, T ub) {
        return (std::min)( (std::max) (num, lb), ub);
    }
};

//struct MaxMatch {

//    LL MAX = 1024;
//    LL n; // X 的大小
//    vector< vector<LL>> weight; // X 到 Y 的映射（权重）
//    vector<LL> lx; // 标号
//    vector<LL> ly; // 标号
//    vector<bool> sx; // 是否被搜索过
//    vector<bool> sy; // 是否被搜索过
//    vector<LL> match; // Y(i) 与 X(match [i]) 匹配
//
//    MaxMatch(lyh::Solver& a, lyh::Solver& b) {
//        this->n = a.rts.cnt;
//        weight = vector< vector<LL>>(n, vector<LL>(n, 0));
//        lx = vector<LL>(n, 0);
//        ly = vector<LL>(n, 0);
//        sx = vector<bool>(n, 0);
//        sy = vector<bool>(n, 0);
//        match = vector<LL>(n, 0);
//
//        for (LL i = 1; i <= a.input.custCnt; i++) {
//            LL aIndex = a.rts.posOf[a.customers[i].routeID];
//            LL bIndex = b.rts.posOf[b.customers[i].routeID];
//            weight[aIndex][bIndex]++;
//            //weight[bIndex][aIndex]++;
//        }
//    }
//    LL getMaxMath() {
//        LL ret = bestmatch(true);
//
//        /*for (int i = 0; i < n; i++) {
//            printf("Y %ld -> X %ld\n", i, match[i]);
//        }*/
//        return ret;
//    }
//    // 从 X(u) 寻找增广道路，找到则返回 true
//    bool path(LL u) {
//        sx[u] = true;
//        for (LL v = 0; v < n; v++)
//            if (!sy[v] && lx[u] + ly[v] == weight[u][v])
//            {
//                sy[v] = true;
//                if (match[v] == -1 || path(match[v]))
//                {
//                    match[v] = u;
//                    return true;
//                }
//            }
//        return false;
//    }
//
//    // 参数 maxsum 为 true ，返回最大权匹配，否则最小权匹配
//    LL bestmatch(bool maxsum) {
//        LL i, j;
//        if (!maxsum)
//        {
//            for (i = 0; i < n; i++)
//                for (j = 0; j < n; j++)
//                    weight[i][j] = -weight[i][j];
//        }
//
//        // 初始化标号
//        for (i = 0; i < n; i++)
//        {
//            lx[i] = -0x1FFFFFFF;
//            ly[i] = 0;
//            for (j = 0; j < n; j++)
//                if (lx[i] < weight[i][j])
//                    lx[i] = weight[i][j];
//        }
//
//        //memset(match, -1, sizeof(match));
//        for (LL i = 0; i < match.size(); i++) {
//            match[i] = -1;
//        }
//
//        for (LL u = 0; u < n; u++)
//            while (1)
//            {
//                //memset(sx, 0, sizeof(sx));
//                for (LL i = 0; i < sx.size(); i++) {
//                    sx[i] = 0;
//                }
//                //memset(sy, 0, sizeof(sy));
//                for (LL i = 0; i < sy.size(); i++) {
//                    sy[i] = 0;
//                }
//                if (path(u))    //一直寻找增广路径，直到子图中没有增广路径，我们通过修改label来增加新的点，增加的点必为y
//                    break;
//
//                // 修改标号
//                LL dx = 0x7FFFFFFF;
//                for (i = 0; i < n; i++)
//                    if (sx[i])
//                        for (j = 0; j < n; j++)
//                            if (!sy[j])
//                                dx = min(lx[i] + ly[j] - weight[i][j], dx); //找到松弛变量最小的点
//                for (i = 0; i < n; i++)
//                {
//                    if (sx[i])
//                        lx[i] -= dx;
//                    if (sy[i])
//                        ly[i] += dx;
//                }
//            }
//
//        LL sum = 0;
//        for (i = 0; i < n; i++)
//            sum += weight[match[i]][i];
//
//        if (!maxsum)
//        {
//            sum = -sum;
//            for (i = 0; i < n; i++)
//                for (j = 0; j < n; j++)
//                    weight[i][j] = -weight[i][j]; // 如果需要保持 weight [ ] [ ] 原来的值，这里需要将其还原
//        }
//        return sum;
//    }
//};

}


#endif // vrptwNew_UTILITY_H
