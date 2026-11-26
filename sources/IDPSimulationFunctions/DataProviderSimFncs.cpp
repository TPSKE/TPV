#include "DataProviderSimFncs.h"
#include <cmath>

IItemsProvider* CreateIDPInstance(std::string dllPath)
{
    (void)dllPath;
    return new DataProviderSimFncs;
}

int DataProviderSimFncs::getAllItems(std::vector<ItemDef>& outItemsList)
{
    outItemsList.clear();

    outItemsList.push_back({ "Sine", "Sine function", "", -100.0, 100.0, ItemType::Analog});
    outItemsList.push_back({ "Cosine", "Cosine function", "", -100.0, 100.0, ItemType::Analog});
    outItemsList.push_back({ "Linear", "Linear function", "", 0.0, 100.0, ItemType::Analog});

    return 1;
}

int DataProviderSimFncs::getProviderParams(ProviderParams& outParams)
{
    outParams.fullName = "Provider for simulate some functions.";
    outParams.isResolutionEnabled = false;
    outParams.isRowcountEnabled = false;

    return 1;
}

int DataProviderSimFncs::readItemLive(std::string aItemName, ItemType aItemType, DataProviderLiveValueType& outValue)
{
    (void)aItemType;
    if (aItemName == "Sine")
    {
        double sinval = 50.0 * std::sin(static_cast<double>(actStepSine) * (6.28 / 100.0));
        actStepSine++;
        if (actStepSine > 100) actStepSine = 0;
        outValue = std::make_pair(sinval, true);
    }
    if (aItemName == "Cosine")
    {
        double cosval = 100.0 * std::cos(static_cast<double>(actStepCosine) * (6.28 / 100.0));
        actStepCosine++;
        if (actStepCosine > 100) actStepCosine = 0;
        outValue = std::make_pair(cosval, true);
    }
    if (aItemName == "Linear")
    {
        int linval = actStepLinear;
        if (linval > 100) linval = 200 - linval;
        outValue = std::make_pair(static_cast<double>(linval), true);
        actStepLinear++;
        if (actStepLinear > 200) actStepLinear = 0;
    }

    return 1;
}
