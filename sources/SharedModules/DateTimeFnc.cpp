#include "DateTimeFnc.h"
#include <chrono>
#include <ctime>
#include <time.h>
#include <mutex>

std::mutex localtime_mutex;

CDT tm2cdt(std::tm atm)
{
    return CDT{ atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday,
               atm.tm_hour, atm.tm_min, atm.tm_sec, 0 };
}

std::tm cdt2tm(CDT acdt)
{
    std::tm retVal;

    retVal.tm_year = acdt.year - 1900;
    retVal.tm_mon = acdt.month - 1;
    retVal.tm_mday = acdt.day;
    retVal.tm_hour = acdt.hour;
    retVal.tm_min = acdt.minute;
    retVal.tm_sec = acdt.second;
    retVal.tm_isdst = -1;

    return retVal;
}

bool operator<=(const CDT& lhs, const CDT& rhs)
{
    std::tm lhs_tm = cdt2tm(lhs);
    std::tm rhs_tm = cdt2tm(rhs);
    std::time_t lhs_time_t = mktime(&lhs_tm);
    std::time_t rhs_time_t = mktime(&rhs_tm);

    return lhs_time_t <= rhs_time_t;
}

bool operator==(const CDT& lhs, const CDT& rhs)
{
    if (lhs.year != rhs.year ||
        lhs.month != rhs.month ||
        lhs.day != rhs.day ||
        lhs.hour != rhs.hour ||
        lhs.minute != rhs.minute ||
        lhs.second != rhs.second ||
        lhs.msecond != rhs.msecond) return false;

    return true;
}

CDT operator+(const CDT& lhs, const int& rhs)
{
    std::tm lhs_tm = { }, *lhs_tm_ret;

    lhs_tm = cdt2tm(lhs);

    std::time_t lhs_time_t = mktime(&lhs_tm);
    lhs_time_t = lhs_time_t + rhs / 1000;

    if (lhs.msecond + rhs % 1000 > 999) lhs_time_t++;

    localtime_mutex.lock();
    lhs_tm_ret = localtime(&lhs_time_t);
    CDT cdt_ret = tm2cdt(*lhs_tm_ret);
    localtime_mutex.unlock();

    cdt_ret.msecond = (lhs.msecond + rhs % 1000) % 1000;

    return cdt_ret;
}

CDT operator+=(CDT& lhs, const int& rhs)
{
    lhs = lhs + rhs;

    return lhs;
}

CDT operator-(const CDT& lhs, const int& rhs)
{
    std::tm lhs_tm = { }, * lhs_tm_ret;
    int msecond = 0;

    lhs_tm = cdt2tm(lhs);

    std::time_t lhs_time_t = mktime(&lhs_tm);
    lhs_time_t = lhs_time_t - rhs / 1000;

    if (lhs.msecond < rhs % 1000)
    {
        lhs_time_t--;
        msecond = 1000 + lhs.msecond - rhs % 1000;
    }
    else msecond = abs(rhs % 1000 - lhs.msecond);

    localtime_mutex.lock();
    lhs_tm_ret = localtime(&lhs_time_t);
    CDT cdt_ret = tm2cdt(*lhs_tm_ret);
    localtime_mutex.unlock();

    cdt_ret.msecond = msecond;

    return cdt_ret;
}

long long CDT::getDiffInMilliseconds(const CDT& lhs, const CDT& rhs)
{
    std::tm lhs_tm = { }, rhs_tm = { };

    lhs_tm = cdt2tm(lhs);
    rhs_tm = cdt2tm(rhs);

    lhs_tm.tm_isdst = 1;
    rhs_tm.tm_isdst = 1;

    std::time_t lhs_time_t = mktime(&lhs_tm);
    std::time_t rhs_time_t = mktime(&rhs_tm);

    std::chrono::time_point<std::chrono::system_clock> lhs_tp = std::chrono::system_clock::from_time_t(lhs_time_t);
    std::chrono::time_point<std::chrono::system_clock> rhs_tp = std::chrono::system_clock::from_time_t(rhs_time_t);

    long long diff_lhs = std::chrono::duration_cast<std::chrono::milliseconds>(lhs_tp.time_since_epoch()).count() + lhs.msecond;
    long long diff_rhs = std::chrono::duration_cast<std::chrono::milliseconds>(rhs_tp.time_since_epoch()).count() + rhs.msecond;

    return(diff_lhs - diff_rhs);
}

std::string CDT::getDiffInString(const CDT& lhs, const CDT& rhs)
{
    std::string retStr;

    long long ms = getDiffInMilliseconds(lhs, rhs);

    int milliseconds = ms % 1000;
    ms = ms / 1000;
    int seconds = ms % 60;
    ms = ms / 60;
    int minutes = ms % 60;
    ms = ms / 60;
    int hours = ms % 24;
    int days = (int)ms / 24;

    retStr = "D: " + std::to_string(days) + ", H: " + std::to_string(hours) +
             ", M: " + std::to_string(minutes) + ", S: " + std::to_string(seconds) + ", MS: " + std::to_string(milliseconds);

    return retStr;
}

std::string CDT::now()
{
    std::chrono::time_point<std::chrono::system_clock> dtm_now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(dtm_now);
    std::tm* now_tm;
    std::string ret_dtm;

    localtime_mutex.lock();
    now_tm = localtime(&now_time_t);

    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(dtm_now.time_since_epoch()) % 1000;

    year = now_tm->tm_year + 1900;
    month = now_tm->tm_mon + 1;
    day = now_tm->tm_mday;
    hour = now_tm->tm_hour;
    minute = now_tm->tm_min;
    second = now_tm->tm_sec;
    msecond = static_cast<int>(nowMs.count());

    localtime_mutex.unlock();

    ret_dtm = CDTtoString(*this);

    return ret_dtm;
}

std::string CDT::CDTtoString(CDT aDT, bool useMilliseconds)
{
    std::string str_rep;

    str_rep = std::to_string(aDT.year);
    str_rep += "-";
    if (aDT.month < 10) str_rep += "0";
    str_rep += std::to_string(aDT.month);
    str_rep += "-";
    if (aDT.day < 10) str_rep += "0";
    str_rep += std::to_string(aDT.day);

    str_rep += " ";
    if (aDT.hour < 10) str_rep += "0";
    str_rep += std::to_string(aDT.hour);
    str_rep += ":";
    if (aDT.minute < 10) str_rep += "0";
    str_rep += std::to_string(aDT.minute);
    str_rep += ":";
    if (aDT.second < 10) str_rep += "0";
    str_rep += std::to_string(aDT.second);

    if(useMilliseconds)
    {
        str_rep += ".";
        if (aDT.msecond < 100) str_rep += "0";
        if (aDT.msecond < 10) str_rep += "0";
        str_rep += std::to_string(aDT.msecond);
    }

    return str_rep;
}

CDT CDT::StringtoCDT(std::string aDT)
{
    CDT retcdt;

    retcdt.year = atoi(aDT.substr(0, 4).c_str());
    retcdt.month = atoi(aDT.substr(5, 2).c_str());
    retcdt.day = atoi(aDT.substr(8, 2).c_str());
    retcdt.hour = atoi(aDT.substr(11, 2).c_str());
    retcdt.minute = atoi(aDT.substr(14, 2).c_str());
    retcdt.second = atoi(aDT.substr(17, 2).c_str());
    retcdt.msecond = atoi(aDT.substr(20, 3).c_str());

    return retcdt;
}
