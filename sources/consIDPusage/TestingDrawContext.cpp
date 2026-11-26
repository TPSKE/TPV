#include "TestingDrawContext.h"
#include "TestingLogger.h"


void TestingDrawContext::showValue(DataValueType aActValue)
{
    TestingLogger::log(CDT::CDTtoString(aActValue.cdt)
                       + " (value=" + std::to_string(aActValue.value)
                       + ") valid=" + std::to_string(aActValue.valid) + " [" + std::to_string(aActValue.x) + ", " + std::to_string(aActValue.y) + "]");
}

void TestingDrawContext::notifyReadingStatus(int aStatus)
{
    TestingLogger::log("Reading : " + std::to_string(aStatus));
}

void TestingDrawContext::updateGridTimes(std::string axis1, std::string axis2, std::string axis3)
{
    TestingLogger::log("      Grid axes time points:");
    TestingLogger::log(axis1 + ", " + axis2 + ", " + axis3);
}

void TestingDrawContext::updateGridRanges(double axis0, double axis1, double axis2, double axis3, double axis4)
{
    TestingLogger::log("      Grid axes range steps:");
    TestingLogger::log(std::to_string(axis0) + ", " + std::to_string(axis1) + ", " + std::to_string(axis2) + ", " + std::to_string(axis3) + ", " + std::to_string(axis4));
}
