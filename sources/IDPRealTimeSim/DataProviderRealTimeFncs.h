#ifndef DATAPROVIDERREALTIMEFNCS_H
#define DATAPROVIDERREALTIMEFNCS_H

#include "IDPRealTimeSim_global.h"
#include "ItemsDataProvider.h"

class IDPREALTIMESIM_EXPORT DataProviderRealTimeFncs : public IHistDataProvider<DataProviderHistValueType>, public ILiveDataProvider
{
public:
    virtual ~DataProviderRealTimeFncs() {}
    virtual int getAllItems(std::vector<ItemDef>& outItems) override;
    virtual int getProviderParams(ProviderParams& outParams) override;
    virtual int readItemHist(HistDataItemReadingParams aItemParam, std::vector<DataProviderHistValueType>& outHistData) override;
    virtual int readItemLive(std::string aItemName, ItemType aItemType, DataProviderLiveValueType& outLiveData) override;
};

extern "C" IDPREALTIMESIM_EXPORT IItemsProvider* CreateIDPInstance(std::string dllPath);

#endif // DATAPROVIDERREALTIMEFNCS_H
