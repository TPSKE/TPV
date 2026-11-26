#include "DataProviderHistorian.h"
#include <QSqlQuery>
#include <QDomDocument>
#include <QFile>

IItemsProvider* CreateIDPInstance(std::string dllPath = "")
{
    return new DataProviderHistorian(dllPath);
}

void DataProviderHistorian::connectDB()
{
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
        myConnName = "HistorianConnection" + QString::number((long long)this);
        myDB = QSqlDatabase::addDatabase("QODBC", myConnName);
        qDebug() << "Added connection : " << myConnName;

        myDB.setDatabaseName(myDSNname);
        myDB.setUserName(myUserName);
        myDB.setPassword(myUserPassword);

        myDB.open();
    }
}

void DataProviderHistorian::disconnectDB()
{
    if(myDB.isOpen())
    {
        myDB.close();
        myDB.removeDatabase(myConnName);
        qDebug() << "Disconnected connection : " << myConnName;
    }
}

DataProviderHistorian::DataProviderHistorian(std::string dllPath)
{
    myDllPath = dllPath;
    connectDB();
}

DataProviderHistorian::~DataProviderHistorian()
{
    disconnectDB();
}

int DataProviderHistorian::getAllItems(std::vector<ItemDef>& outItems)
{
    outItems.clear();

    if(!myDB.isOpen()) connectDB();

    if(myDB.isOpen())
    {
        QSqlQuery sel(myDB);
        QString qry;

        //-- Analog + Digital tags
        //select a.tagtype, a.tagname, a.description, b.computername, b.applicationname, c.name, a.itemname
        //from tag a, ioserver b,topic c where a.ioserverkey = b.ioserverkey and a.topickey = c.topickey

        qry = "select a.tagtype, a.tagname, a.description, b.computername, b.applicationname, c.name, a.itemname ";
        qry += "from tag a, ioserver b,topic c where a.ioserverkey = b.ioserverkey and a.topickey = c.topickey";

        sel.prepare(qry);
        sel.exec();

        std::map<std::string, int> analogIdxs;
        std::map<std::string, int> discreteIdxs;

        while(sel.next())
        {
            ItemDef newItem;

            newItem.name = sel.value(1).toString().toStdString();
            newItem.description = sel.value(2).toString().toStdString();
            newItem.dataSource = sel.value(3).toString().toStdString();
            newItem.dataSource += "|";
            newItem.dataSource += sel.value(4).toString().toStdString();
            newItem.dataSource += "|";
            newItem.dataSource += sel.value(5).toString().toStdString();
            newItem.dataSource += "|";
            newItem.dataSource += sel.value(6).toString().toStdString();

            if(sel.value(0).toInt() == 1)
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
        }

        readAnalogTagParams(analogIdxs, outItems);
        readDiscreteTagParams(discreteIdxs, outItems);
    }

    return 1;
}

void DataProviderHistorian::readAnalogTagParams(std::map<std::string, int>& idxs, std::vector<ItemDef>& outItems)
{
    //-- Analog tags params
    //select a.tagname, a.mineu, a.maxeu, b.unit
    //from analogtag a, engineeringunit b where a.eukey = b.eukey and a.tagname = 'xyz'

    QSqlQuery sel(myDB);
    QString qry;

    qry = "select a.tagname, a.mineu, a.maxeu, b.unit ";
    qry += "from analogtag a, engineeringunit b where a.eukey = b.eukey";

    sel.prepare(qry);
    sel.exec();
    while(sel.next())
    {
        int outIndex = idxs[sel.value(0).toString().toStdString()];

        outItems[outIndex].rangeMin = sel.value(1).toDouble();
        outItems[outIndex].rangeMax = sel.value(2).toDouble();
        outItems[outIndex].description += " [";
        outItems[outIndex].description += sel.value(3).toString().toStdString();
        outItems[outIndex].description += "]";
    }
}

void DataProviderHistorian::readDiscreteTagParams(std::map<std::string, int>& idxs, std::vector<ItemDef>& outItems)
{
    //-- Discrete tags params
    //select a.tagname, b.message0, b.message1
    //from discretetag a, message b where a.messagekey = b.messagekey and a.tagname = 'xyz'

    QSqlQuery sel(myDB);
    QString qry;

    qry = "select a.tagname, b.message0, b.message1 ";
    qry += "from discretetag a, message b where a.messagekey = b.messagekey";

    sel.prepare(qry);
    sel.exec();
    while(sel.next())
    {
        int outIndex = idxs[sel.value(0).toString().toStdString()];

        outItems[outIndex].rangeMin = -1.0;
        outItems[outIndex].rangeMax = 2.0;
        outItems[outIndex].description += " [";
        outItems[outIndex].description += sel.value(1).toString().toStdString();
        outItems[outIndex].description += "/";
        outItems[outIndex].description += sel.value(2).toString().toStdString();
        outItems[outIndex].description += "]";
    }
}

int DataProviderHistorian::getProviderParams(ProviderParams& outParams)
{
    outParams.fullName = "Data provider for Historian server";
    outParams.isResolutionEnabled = true;
    outParams.isRowcountEnabled = true;

    return 1;
}

int DataProviderHistorian::readItemHist(HistDataItemReadingParams aItemParam, std::vector<DataProviderHistValueType>& outHistData)
{
    outHistData.clear();

    DataProviderHistValueType val;

    if(!myDB.isOpen()) connectDB();

    if(myDB.isOpen())
    {
        QSqlQuery sel(myDB);
        QString qry;

        if (aItemParam.itemType == ItemType::Analog)
        {
            // Analogs
            qry = "select value,convert(char(40), datetime, 121) from v_analoghistory where tagname = '";
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

            sel.prepare(qry);
            sel.exec();

            while(sel.next())
            {
                bool isValid = true;
                if(sel.value(0).isNull()) isValid = false;

                val = std::make_tuple(sel.value(1).toString().toStdString(), sel.value(0).toDouble(), isValid);
                outHistData.push_back(val);
            }
        }
        else
        {
            // Digitals
            qry = "select value,convert(char(40), datetime, 121) from v_discretehistory where tagname = '";
            qry += aItemParam.itemName;
            qry += "' and wwretrievalmode = 'Delta'";
            qry += " and datetime >= '";
            qry += aItemParam.aStart;
            qry += "' and datetime <= '";
            qry += aItemParam.aEnd;
            qry += "'";

            sel.prepare(qry);
            sel.exec();

            while(sel.next())
            {
                bool isValid = true;
                if(sel.value(0).isNull()) isValid = false;

                val = std::make_tuple(sel.value(1).toString().toStdString(), sel.value(0).toDouble(), isValid);
                outHistData.push_back(val);
            }
        }
    }

    return 1;
}

int DataProviderHistorian::readItemLive(std::string aItemName, ItemType aItemType, DataProviderLiveValueType& outLiveData)
{
    if(!myDB.isOpen()) connectDB();

    if(myDB.isOpen())
    {
        QSqlQuery sel(myDB);
        QString qry;

        if(aItemType == ItemType::Analog) qry = "select value from v_analoglive where tagname = '";
        else qry = "select value from v_discretelive where tagname = '";
        qry += aItemName;
        qry += "'";

        sel.prepare(qry);
        sel.exec();

        if(sel.next())
        {
            bool isValid = true;
            if(sel.value(0).isNull()) isValid = false;

            outLiveData = std::make_pair(sel.value(0).toDouble(), isValid);
        }
    }

    return 1;
}
