#ifndef DATAPROVIDERHISTSIM3D_H
#define DATAPROVIDERHISTSIM3D_H

#include "IDPHistSim3D_global.h"
#include "ItemsDataProvider.h"

class IDPHISTSIM3D_EXPORT DataProviderHistSim3D : public IHistDataProvider<DataProviderHistValueType3D>
{
public:
    virtual ~DataProviderHistSim3D() {}
    virtual int getAllItems(std::vector<ItemDef>& outItems) override;
    virtual int getProviderParams(ProviderParams& outParams) override;
    virtual int readItemHist(HistDataItemReadingParams aItemParam, std::vector<DataProviderHistValueType3D>& outHistData) override;
};

extern "C" IDPHISTSIM3D_EXPORT IItemsProvider* CreateIDPInstance(std::string dllPath);

#endif // DATAPROVIDERHISTSIM3D_H
