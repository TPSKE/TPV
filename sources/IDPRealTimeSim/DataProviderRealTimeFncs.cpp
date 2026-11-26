#include "DataProviderRealTimeFncs.h"
#include <thread>
#include <set>
#include <cstdlib>
#include "DateTimeFnc.h"

IItemsProvider* CreateIDPInstance(std::string dllPath)
{
    (void)dllPath;
    return new DataProviderRealTimeFncs;
}

int DataProviderRealTimeFncs::getAllItems(std::vector<ItemDef>& outItems)
{
    outItems.clear();

    outItems.push_back({ "Year", "Year at given date and time point", "", 1000.0, 3000.0, ItemType::Analog });
    outItems.push_back({ "Month", "Month at given date and time point", "", 0.0, 11.0, ItemType::Analog });
    outItems.push_back({ "Day", "Day at given date and time point", "", 0.0, 31.0, ItemType::Analog });
    outItems.push_back({ "Hour", "Hour at given date and time point", "", 0.0, 23.0, ItemType::Analog });
    outItems.push_back({ "Minute", "Minute at given date and time point", "", 0.0, 59.0, ItemType::Analog});
    outItems.push_back({ "Second", "Second at given date and time point", "", 0.0, 59.0, ItemType::Analog});
    outItems.push_back({ "MSec", "Milisecond at given date and time point", "", 0.0, 1000.0, ItemType::Analog});
    outItems.push_back({ "SomeAlarm", "...for testing discrete history", "", -1.0, 2.0, ItemType::Discrete });

    return 1;
}

int DataProviderRealTimeFncs::getProviderParams(ProviderParams& outParams)
{
    outParams.fullName = "";
    outParams.isResolutionEnabled = true;
    outParams.isRowcountEnabled = false;

    return 1;
}

int DataProviderRealTimeFncs::readItemHist(HistDataItemReadingParams aItemParam, std::vector<DataProviderHistValueType>& outHistData)
{
    outHistData.clear();

    DataProviderHistValueType val;

    CDT cdtStart = CDT::StringtoCDT(aItemParam.aStart);
    CDT cdtEnd = CDT::StringtoCDT(aItemParam.aEnd);

    if (aItemParam.itemType == ItemType::Analog)
    {
        // Analogs
        //int counterForDSTswitch = 0;//Counter for testing DST switch summer -> winter
        for (CDT cdt = cdtStart; cdt <= cdtEnd; cdt += aItemParam.retrievalValue)
        {
            if (aItemParam.itemName == "Year") val = std::make_tuple(CDT::CDTtoString(cdt), cdt.year, true);
            if (aItemParam.itemName == "Month") val = std::make_tuple(CDT::CDTtoString(cdt), cdt.month, true);
            if (aItemParam.itemName == "Day") val = std::make_tuple(CDT::CDTtoString(cdt), cdt.day, true);
            if (aItemParam.itemName == "Hour") val = std::make_tuple(CDT::CDTtoString(cdt), cdt.hour, true);
            if (aItemParam.itemName == "Minute") val = std::make_tuple(CDT::CDTtoString(cdt), cdt.minute, true);
            if (aItemParam.itemName == "Second") val = std::make_tuple(CDT::CDTtoString(cdt), cdt.second, true);
            if (aItemParam.itemName == "MSec") val = std::make_tuple(CDT::CDTtoString(cdt), cdt.msecond, true);
            /*if (counterForDSTswitch > 4)
            {
                std::get<0>(val) = CDT::CDTtoString(cdtStart + (counterForDSTswitch - 4) * aItemParam.retrievalValue);
            }
            counterForDSTswitch++;*/
            outHistData.push_back(val);
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));//Just for testing mutex
        }
    }
    else
    {
        // Digitals
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        long long myRange = CDT::getDiffInMilliseconds(cdtEnd, cdtStart);

        std::set<int> randOffsets;
        bool tmpVal = false;
        do {
            int offset = static_cast<int>(static_cast<double>(myRange) * (static_cast<double>(std::rand()) / RAND_MAX));
            randOffsets.insert(offset);
        } while (randOffsets.size() < 10);

        val = std::make_tuple(CDT::CDTtoString(cdtStart), tmpVal, false);
        outHistData.push_back(val);

        for (std::set<int>::iterator itr = randOffsets.begin(); itr != randOffsets.end(); itr++)
        {
            CDT nextTime = cdtStart + *itr;
            val = std::make_tuple(CDT::CDTtoString(nextTime), tmpVal, true);
            outHistData.push_back(val);
            tmpVal = !tmpVal;
        }
    }

    return 1;
}

int DataProviderRealTimeFncs::readItemLive(std::string aItemName, ItemType aItemType, DataProviderLiveValueType& outLiveData)
{
    CDT dtf;

    (void)aItemType;

    if (aItemName == "Year") outLiveData = std::make_pair(dtf.year, true);
    if (aItemName == "Month") outLiveData = std::make_pair(dtf.month, true);
    if (aItemName == "Day") outLiveData = std::make_pair(dtf.day, true);
    if (aItemName == "Hour") outLiveData = std::make_pair(dtf.hour, true);
    if (aItemName == "Minute") outLiveData = std::make_pair(dtf.minute, true);
    if (aItemName == "Second") outLiveData = std::make_pair(dtf.second, true);
    if (aItemName == "MSec") outLiveData = std::make_pair(dtf.msecond, true);
    if (aItemName == "SomeAlarm") outLiveData = std::make_pair(0.0, true);

    return 1;
}
