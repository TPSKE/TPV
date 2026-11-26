#include "DataProviderHistorianADODB.h"
#include <QDomDocument>
#include <QFile>

IItemsProvider* CreateIDPInstance(std::string dllPath = "")
{
    return new DataProviderHistorianADODB(dllPath);
}

void DataProviderHistorianADODB::connectDB()
{
    if(connectionError) return;

    if(myDSNname.isEmpty())
    {
        QFile trendFile(QString::fromStdString(this->myDllPath + "/InSQLParam.cfg"));

        if(!trendFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open file! [" << trendFile.fileName() << "]";;
            return;
        }

        QDomDocument doc;
        if(!doc.setContent(&trendFile))
        {
            qDebug() << "Unable to parse file into DOM tree.";
            trendFile.close();
            return;
        }

        trendFile.close();

        QDomElement docElement = doc.documentElement();
        QDomNode node = docElement.firstChild();
        while(!node.isNull())
        {
            QDomElement elem;

            elem = node.toElement();

            if(elem.nodeName() == "DSNName") myDSNname = elem.text();
            if(elem.nodeName() == "UserName") myUserName = elem.text();
            if(elem.nodeName() == "Password") myUserPassword = elem.text();

            node = node.nextSibling();
        }
    }

    if(!myDSNname.isEmpty())
    {
        CoInitialize(nullptr);
        myDB = new QAxObject("ADODB.Connection");

        QObject::connect(myDB, SIGNAL(exception(int, QString, QString, QString)), this, SLOT(handleComException(int, QString, QString, QString)));

        //QString connStr = "Provider=SQLOLEDB;Data Source=192.168.1.18;Initial Catalog=Runtime;User Id=insql_man;Password=paoc;";
        QString connStr = "Provider=SQLOLEDB;Data Source=" + myDSNname + ";Initial Catalog=Runtime;User Id=" + myUserName + ";Password=" + myUserPassword + ";";
        myDB->dynamicCall("Open(const QString&)", connStr);
        myDB_isOpen = true;
    }
}

void DataProviderHistorianADODB::handleComException(int code, const QString &source, const QString &desc, const QString &help)
{
    connectionError = true;

    qDebug() << "COM Exception Code:" << code;
    qDebug() << "Source:" << source;
    qDebug() << "Description:" << desc;
    qDebug() << "Help:" << help;
}

void DataProviderHistorianADODB::disconnectDB()
{
    if(myDB_isOpen && !connectionError)
    {
        myDB->dynamicCall("Close()");
        myDB_isOpen = false;
        delete myDB;
    }
    CoUninitialize();
}

DataProviderHistorianADODB::DataProviderHistorianADODB(std::string dllPath)
{
    myDllPath = dllPath;
    connectDB();
}

DataProviderHistorianADODB::~DataProviderHistorianADODB()
{
    disconnectDB();
}

int DataProviderHistorianADODB::getAllItems(std::vector<ItemDef>& outItems)
{
    outItems.clear();

    if(!myDB_isOpen) connectDB();

    if(myDB_isOpen && !connectionError)
    {
        QString qry;

        //-- Analog + Digital tags
        //select a.tagtype, a.tagname, a.description, b.computername, b.applicationname, c.name, a.itemname
        //from tag a, ioserver b,topic c where a.ioserverkey = b.ioserverkey and a.topickey = c.topickey

        qry = "select a.tagtype, a.tagname, a.description, b.computername, b.applicationname, c.name, a.itemname ";
        qry += "from tag a, ioserver b,topic c where a.ioserverkey = b.ioserverkey and a.topickey = c.topickey";

        std::map<std::string, int> analogIdxs;
        std::map<std::string, int> discreteIdxs;

        QAxObject *recordset = myDB->querySubObject("Execute(const QString&)", qry);

        if(recordset != nullptr && recordset->isNull() == false)
        {
            while(!recordset->property("EOF").toBool())
            {
                ItemDef newItem;

                QAxObject *fields = recordset->querySubObject("Fields");

                QAxObject *tagtypeField = fields->querySubObject("Item(const QVariant&)", "tagtype");
                QAxObject *tagnameField = fields->querySubObject("Item(const QVariant&)", "tagname");
                QAxObject *descField = fields->querySubObject("Item(const QVariant&)", "description");
                QAxObject *computernameField = fields->querySubObject("Item(const QVariant&)", "computername");
                QAxObject *applicationnameField = fields->querySubObject("Item(const QVariant&)", "applicationname");
                QAxObject *nameField = fields->querySubObject("Item(const QVariant&)", "name");
                QAxObject *itemnameField = fields->querySubObject("Item(const QVariant&)", "itemname");

                newItem.name = tagnameField->property("Value").toString().toStdString();
                newItem.description = descField->property("Value").toString().toStdString();
                newItem.dataSource = computernameField->property("Value").toString().toStdString();;
                newItem.dataSource += "|";
                newItem.dataSource += applicationnameField->property("Value").toString().toStdString();
                newItem.dataSource += "|";
                newItem.dataSource += nameField->property("Value").toString().toStdString();
                newItem.dataSource += "|";
                newItem.dataSource += itemnameField->property("Value").toString().toStdString();

                if(tagtypeField->property("Value").toInt() == 1)
                {
                    newItem.type = ItemType::Analog;
                    analogIdxs[newItem.name] = static_cast<int>(outItems.size());
                }
                else
                {
                    newItem.type = ItemType::Discrete;
                    discreteIdxs[newItem.name] = static_cast<int>(outItems.size());
                }

                outItems.push_back(newItem);

                delete tagtypeField;
                delete tagnameField;
                delete descField;
                delete computernameField;
                delete applicationnameField;
                delete nameField;
                delete itemnameField;
                delete fields;

                recordset->dynamicCall("MoveNext()");
            }
            recordset->dynamicCall("Close()");
            delete recordset;
        }

        readAnalogTagParams(analogIdxs, outItems);
        readDiscreteTagParams(discreteIdxs, outItems);
    }

    return 1;
}

void DataProviderHistorianADODB::readAnalogTagParams(std::map<std::string, int>& idxs, std::vector<ItemDef>& outItems)
{
    //-- Analog tags params
    //select a.tagname, a.mineu, a.maxeu, b.unit
    //from analogtag a, engineeringunit b where a.eukey = b.eukey and a.tagname = 'xyz'

    QString qry;

    qry = "select a.tagname, a.mineu, a.maxeu, b.unit ";
    qry += "from analogtag a, engineeringunit b where a.eukey = b.eukey";

    QAxObject *recordset = myDB->querySubObject("Execute(const QString&)", qry);

    if(recordset != nullptr && recordset->isNull() == false)
    {
        while(!recordset->property("EOF").toBool())
        {
            QAxObject *fields = recordset->querySubObject("Fields");

            QAxObject *tagnameField = fields->querySubObject("Item(const QVariant&)", "tagname");
            QAxObject *mineuField = fields->querySubObject("Item(const QVariant&)", "mineu");
            QAxObject *maxeuField = fields->querySubObject("Item(const QVariant&)", "maxeu");
            QAxObject *unitField = fields->querySubObject("Item(const QVariant&)", "unit");

            int outIndex = idxs[tagnameField->property("Value").toString().toStdString()];

            outItems[outIndex].rangeMin = mineuField->property("Value").toDouble();
            outItems[outIndex].rangeMax = maxeuField->property("Value").toDouble();
            outItems[outIndex].description += " [";
            outItems[outIndex].description += unitField->property("Value").toString().toStdString();
            outItems[outIndex].description += "]";

            delete tagnameField;
            delete mineuField;
            delete maxeuField;
            delete unitField;
            delete fields;

            recordset->dynamicCall("MoveNext()");
        }
        recordset->dynamicCall("Close()");
        delete recordset;
    }
}

void DataProviderHistorianADODB::readDiscreteTagParams(std::map<std::string, int>& idxs, std::vector<ItemDef>& outItems)
{
    //-- Discrete tags params
    //select a.tagname, b.message0, b.message1
    //from discretetag a, message b where a.messagekey = b.messagekey and a.tagname = 'xyz'

    QString qry;

    qry = "select a.tagname, b.message0, b.message1 ";
    qry += "from discretetag a, message b where a.messagekey = b.messagekey";

    QAxObject *recordset = myDB->querySubObject("Execute(const QString&)", qry);

    if(recordset != nullptr && recordset->isNull() == false)
    {
        while(!recordset->property("EOF").toBool())
        {
            QAxObject *fields = recordset->querySubObject("Fields");

            QAxObject *tagnameField = fields->querySubObject("Item(const QVariant&)", "tagname");
            QAxObject *mess0Field = fields->querySubObject("Item(const QVariant&)", "message0");
            QAxObject *mess1Field = fields->querySubObject("Item(const QVariant&)", "message1");

            int outIndex = idxs[tagnameField->property("Value").toString().toStdString()];

            outItems[outIndex].rangeMin = -1.0;
            outItems[outIndex].rangeMax = 2.0;
            outItems[outIndex].description += " [";
            outItems[outIndex].description += mess0Field->property("Value").toString().toStdString();
            outItems[outIndex].description += "/";
            outItems[outIndex].description += mess1Field->property("Value").toString().toStdString();
            outItems[outIndex].description += "]";

            delete tagnameField;
            delete mess0Field;
            delete mess1Field;
            delete fields;

            recordset->dynamicCall("MoveNext()");
        }
        recordset->dynamicCall("Close()");
        delete recordset;
    }
}

int DataProviderHistorianADODB::getProviderParams(ProviderParams& outParams)
{
    outParams.fullName = "Data provider for Historian server ADODB";
    outParams.isResolutionEnabled = true;
    outParams.isRowcountEnabled = true;

    return 1;
}

int DataProviderHistorianADODB::readItemHist(HistDataItemReadingParams aItemParam, std::vector<DataProviderHistValueType>& outHistData)
{
    outHistData.clear();

    DataProviderHistValueType val;

    if(!myDB_isOpen) connectDB();

    if(myDB_isOpen && !connectionError)
    {
        QString qry;

        if (aItemParam.itemType == ItemType::Analog)
        {
            qry = "select value,convert(char(40), datetime, 121) as 'DTM' from v_analoghistory where tagname = '";
            qry += aItemParam.itemName;
            qry += "' and wwretrievalmode = 'Cyclic'";
            if(aItemParam.itemRetrievalType == RetrievalType::RowCount || aItemParam.itemRetrievalType == RetrievalType::MaxVisible) qry += " and wwrowcount = ";
            else qry += " and wwresolution = ";
            qry += std::to_string(aItemParam.retrievalValue);
            qry += " and datetime >= '";
            qry += aItemParam.aStart;
            qry += "' and datetime <= '";
            qry += aItemParam.aEnd;
            qry += "'";

            QAxObject *recordset = myDB->querySubObject("Execute(const QString&)", qry);

            if(recordset != nullptr && recordset->isNull() == false)
            {
                while(!recordset->property("EOF").toBool())
                {
                    QAxObject *fields = recordset->querySubObject("Fields");

                    QAxObject *valueField = fields->querySubObject("Item(const QVariant&)", "value");
                    QAxObject *dtmField = fields->querySubObject("Item(const QVariant&)", "DTM");

                    bool isValid = true;
                    if(valueField->property("Value").isNull()) isValid = false;

                    val = std::make_tuple(dtmField->property("Value").toString().toStdString(), valueField->property("Value").toDouble(), isValid);
                    outHistData.push_back(val);

                    delete valueField;
                    delete dtmField;
                    delete fields;

                    recordset->dynamicCall("MoveNext()");
                }
                recordset->dynamicCall("Close()");
                delete recordset;
            }
        }
        else
        {
            qry = "select value,convert(char(40), datetime, 121) as 'DTM' from v_discretehistory where tagname = '";
            qry += aItemParam.itemName;
            qry += "' and wwretrievalmode = 'Delta'";
            qry += " and datetime >= '";
            qry += aItemParam.aStart;
            qry += "' and datetime <= '";
            qry += aItemParam.aEnd;
            qry += "'";

            QAxObject *recordset = myDB->querySubObject("Execute(const QString&)", qry);

            if(recordset != nullptr && recordset->isNull() == false)
            {
                while(!recordset->property("EOF").toBool())
                {
                    QAxObject *fields = recordset->querySubObject("Fields");

                    QAxObject *valueField = fields->querySubObject("Item(const QVariant&)", "value");
                    QAxObject *dtmField = fields->querySubObject("Item(const QVariant&)", "DTM");

                    bool isValid = true;
                    if(valueField->property("Value").isNull()) isValid = false;

                    val = std::make_tuple(dtmField->property("Value").toString().toStdString(), valueField->property("Value").toDouble(), isValid);
                    outHistData.push_back(val);

                    delete valueField;
                    delete dtmField;
                    delete fields;

                    recordset->dynamicCall("MoveNext()");
                }
                recordset->dynamicCall("Close()");
                delete recordset;
            }
        }
    }

    return 1;
}

int DataProviderHistorianADODB::readItemLive(std::string aItemName, ItemType aItemType, DataProviderLiveValueType& outLiveData)
{
    if(!myDB_isOpen) connectDB();

    if(myDB_isOpen && !connectionError)
    {
        QString qry;

        if(aItemType == ItemType::Analog) qry = "select value from v_analoglive where tagname = '";
        else qry = "select value from v_discretelive where tagname = '";
        qry += aItemName;
        qry += "'";

        QAxObject *recordset = myDB->querySubObject("Execute(const QString&)", qry);

        if(recordset != nullptr && recordset->isNull() == false)
        {
            while(!recordset->property("EOF").toBool())
            {
                QAxObject *fields = recordset->querySubObject("Fields");

                QAxObject *valueField = fields->querySubObject("Item(const QVariant&)", "value");

                bool isValid = true;
                if(valueField->property("Value").isNull()) isValid = false;

                outLiveData = std::make_pair(valueField->property("Value").toDouble(), isValid);

                delete valueField;
                delete fields;

                recordset->dynamicCall("MoveNext()");
            }
            recordset->dynamicCall("Close()");
            delete recordset;
        }
    }

    return 1;
}

