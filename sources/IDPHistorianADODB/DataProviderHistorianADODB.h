#ifndef DATAPROVIDERHISTORIANADODB_H
#define DATAPROVIDERHISTORIANADODB_H

#include "IDPHistorianADODB_global.h"
#include "ItemsDataProvider.h"
#include <QAxObject>
#include <QVariant>
#include <windows.h>
#include <QWidget>

class IDPHISTORIANADODB_EXPORT DataProviderHistorianADODB : public QWidget, public IHistDataProvider<DataProviderHistValueType>, public ILiveDataProvider
{
    Q_OBJECT

public:
    DataProviderHistorianADODB(std::string dllPath);
    virtual ~DataProviderHistorianADODB();
    virtual int getAllItems(std::vector<ItemDef>& outItems) override;
    virtual int getProviderParams(ProviderParams& outParams) override;
    virtual int readItemHist(HistDataItemReadingParams aItemParam, std::vector<DataProviderHistValueType>& outHistData) override;
    virtual int readItemLive(std::string aItemName, ItemType aItemType, DataProviderLiveValueType& outLiveData) override;

private slots:
    void handleComException(int code, const QString &source, const QString &desc, const QString &help);

private:
    void connectDB();
    void disconnectDB();
    void readAnalogTagParams(std::map<std::string, int>& idxs, std::vector<ItemDef>& outItems);
    void readDiscreteTagParams(std::map<std::string, int>& idxs, std::vector<ItemDef>& outItems);

    QAxObject *myDB = nullptr;
    bool myDB_isOpen = false;
    bool connectionError = false;
    QString myDSNname;
    QString myUserName;
    QString myUserPassword;
    QString myConnName;
};

extern "C" IDPHISTORIANADODB_EXPORT IItemsProvider* CreateIDPInstance(std::string dllPath);

#endif // DATAPROVIDERHISTORIANADODB_H
