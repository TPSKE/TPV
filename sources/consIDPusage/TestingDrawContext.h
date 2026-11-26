#ifndef TESTINGDRAWCONTEXT_H
#define TESTINGDRAWCONTEXT_H

#include "TrendDispObject.h"

class TestingDrawContext : public IWindowNotiyInterface
{
public:
    void showValue(DataValueType aActValue) override;
    void notifyReadingStatus(int aStatus) override;
    virtual void updateGridTimes(std::string axis1, std::string axis2, std::string axis3) override;
    virtual void updateGridRanges(double axis0, double axis1, double axis2, double axis3, double axis4) override;
};

#endif // TESTINGDRAWCONTEXT_H
