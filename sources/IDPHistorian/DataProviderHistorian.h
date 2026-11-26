#ifndef DATAPROVIDERHISTORIAN_H
#define DATAPROVIDERHISTORIAN_H

#include "IDPHistorian_global.h"
#include "ItemsDataProvider.h"
#include <QSqlDatabase>

class IDPHISTORIAN_EXPORT DataProviderHistorian : public IHistDataProvider<DataProviderHistValueType>, public ILiveDataProvider
{
public:
    DataProviderHistorian(std::string dllPath);
    virtual ~DataProviderHistorian();
    virtual int getAllItems(std::vector<ItemDef>& outItems) override;
    virtual int getProviderParams(ProviderParams& outParams) override;
    virtual int readItemHist(HistDataItemReadingParams aItemParam, std::vector<DataProviderHistValueType>& outHistData) override;
    virtual int readItemLive(std::string aItemName, ItemType aItemType, DataProviderLiveValueType& outLiveData) override;

private:
    void connectDB();
    void disconnectDB();
    void readAnalogTagParams(std::map<std::string, int>& idxs, std::vector<ItemDef>& outItems);
    void readDiscreteTagParams(std::map<std::string, int>& idxs, std::vector<ItemDef>& outItems);

    QSqlDatabase myDB;
    QString myDSNname;
    QString myUserName;
    QString myUserPassword;
    QString myConnName;
};

extern "C" IDPHISTORIAN_EXPORT IItemsProvider* CreateIDPInstance(std::string dllPath);

#endif // DATAPROVIDERHISTORIAN_H
