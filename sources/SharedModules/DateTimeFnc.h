#ifndef DATETIMEFNC_H
#define DATETIMEFNC_H

#include <string>

constexpr int cdtSecond = 1000;
constexpr int cdtMinute = 60000;
constexpr int cdtHour = 3600000;
constexpr int cdtDay = 86400000;

class CDT
{
public:
    CDT() { now(); }
    CDT(int aYear, int aMonth, int aDay, int aHour, int aMinute, int aSecond, int aMsecond) :
        year{ aYear }, month{ aMonth }, day{ aDay }, hour{ aHour }, minute{ aMinute }, second{ aSecond }, msecond{ aMsecond }
    {
    }
    std::string now();
    static std::string CDTtoString(CDT aDT, bool useMilliseconds = true);
    static CDT StringtoCDT(std::string aDT);
    static long long getDiffInMilliseconds(const CDT& lhs, const CDT& rhs);
    static std::string getDiffInString(const CDT& lhs, const CDT& rhs);

    int year;
    int month;//1-12
    int day;//1-31
    int hour;//0-23
    int minute;//0-59
    int second;//0-59
    int msecond;//0-999
};

bool operator<=(const CDT& lhs, const CDT& rhs);
bool operator==(const CDT& lhs, const CDT& rhs);
CDT operator+(const CDT& lhs, const int& rhs);
CDT operator+=(CDT& lhs, const int& rhs);
CDT operator-(const CDT& lhs, const int& rhs);

#endif // DATETIMEFNC_H
