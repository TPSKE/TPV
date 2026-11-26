#include "DataProviderHistSim3D.h"
#include "DateTimeFnc.h"
#include <set>
#include <ctime>

constexpr double radius = 3.0;

IItemsProvider* CreateIDPInstance(std::string dllPath = "")
{
    (void)dllPath;
    return new DataProviderHistSim3D();
}

int DataProviderHistSim3D::getAllItems(std::vector<ItemDef>& outItems)
{
    outItems.clear();

    outItems.push_back({ "SecondWithMinuteAsZPos", "Second at given time moved in Z dimension by minute", "", 0.0, 59.0, ItemType::Analog });
    outItems.push_back({ "MinuteSpiralling", "3D spiral with full round every minute", "", 0.0, 59.0, ItemType::Analog });
    outItems.push_back({ "RandomYandZ", "Random 3D function.", "", 0.0, 30.0, ItemType::Analog });

    return 1;
}

int DataProviderHistSim3D::getProviderParams(ProviderParams& outParams)
{
    outParams.fullName = "Data provider for 3D historical trends simulation";
    outParams.isResolutionEnabled = true;
    outParams.isRowcountEnabled = false;

    return 1;
}

int DataProviderHistSim3D::readItemHist(HistDataItemReadingParams aItemParam, std::vector<DataProviderHistValueType3D>& outHistData)
{
    // Try to keep Z dimension in range of -15.0, 15.0 for better view area
    outHistData.clear();

    DataProviderHistValueType3D val;

    CDT cdtStart = CDT::StringtoCDT(aItemParam.aStart);
    CDT cdtEnd = CDT::StringtoCDT(aItemParam.aEnd);

    if(aItemParam.itemName != "RandomYandZ")
    {
        for (CDT cdt = cdtStart; cdt <= cdtEnd; cdt += aItemParam.retrievalValue)
        {
            if (aItemParam.itemName == "MinuteSpiralling")
            {
                double angle = (double)cdt.second * (6.28 / 59.0);
                double Y = (3.0 * radius) * cos(angle);
                double Z = radius * sin(angle);
                val = std::make_tuple(CDT::CDTtoString(cdt), Y + 15.0, Z, true);
            }
            if (aItemParam.itemName == "SecondWithMinuteAsZPos")
            {
                double zPos = (((double)cdt.minute + 1.0) / 2.0) - 15.0;
                val = std::make_tuple(CDT::CDTtoString(cdt), cdt.second, zPos, true);
            }

            outHistData.push_back(val);
        }
    }
    else
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        long long myRange = CDT::getDiffInMilliseconds(cdtEnd, cdtStart);

        std::set<int> randOffsets;
        double tmpVal = 30.0 * (static_cast<double>(std::rand()) / RAND_MAX);
        double zPos = 0.0;
        do {
            int offset = static_cast<int>(static_cast<double>(myRange) * (static_cast<double>(std::rand()) / RAND_MAX));
            randOffsets.insert(offset);
        } while (randOffsets.size() < 10);

        val = std::make_tuple(CDT::CDTtoString(cdtStart), tmpVal, zPos, true);
        outHistData.push_back(val);

        for (std::set<int>::iterator itr = randOffsets.begin(); itr != randOffsets.end(); itr++)
        {
            CDT nextTime = cdtStart + *itr;
            CDT tmpTime = nextTime - 2000;
            val = std::make_tuple(CDT::CDTtoString(tmpTime), tmpVal, zPos, true);
            outHistData.push_back(val);

            tmpTime = nextTime - 1000;
            tmpVal = 30.0 * (static_cast<double>(std::rand()) / RAND_MAX);
            val = std::make_tuple(CDT::CDTtoString(tmpTime), tmpVal, zPos, true);
            outHistData.push_back(val);

            zPos = 15.0 * (static_cast<double>(std::rand()) / RAND_MAX) - 7.0;
            val = std::make_tuple(CDT::CDTtoString(nextTime), tmpVal, zPos, true);
            outHistData.push_back(val);
        }
    }

    return 1;
}
