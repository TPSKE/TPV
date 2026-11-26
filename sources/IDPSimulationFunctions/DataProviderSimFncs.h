#ifndef DATAPROVIDERSIMFNCS_H
#define DATAPROVIDERSIMFNCS_H

#include "IDPSimulationFunctions_global.h"
#include "ItemsDataProvider.h"

class IDPSIMULATIONFUNCTIONS_EXPORT DataProviderSimFncs : public ILiveDataProvider
{
public:
    virtual ~DataProviderSimFncs() {}
    virtual int getAllItems(std::vector<ItemDef>& outItemsList) override;
    virtual int getProviderParams(ProviderParams& outParams) override;
    virtual int readItemLive(std::string aItemName, ItemType aItemType, DataProviderLiveValueType& outValue) override;
private:
    int actStepSine = 0;
    int actStepCosine = 0;
    int actStepLinear = 0;
};

extern "C" IDPSIMULATIONFUNCTIONS_EXPORT IItemsProvider* CreateIDPInstance(std::string dllPath);

#endif // DATAPROVIDERSIMFNCS_H
