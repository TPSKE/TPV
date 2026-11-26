#include "TestingTrendWindows.h"
#include "TestingLogger.h"
#include <thread>
#include <chrono>

// CTOR
HistTrendWindow::HistTrendWindow()
{
    myHistTrendObject.resetDrawingInterfaceSize(10, 50);
}

// Get all available items grouped by provider for adding to selection
void HistTrendWindow::show_available_items()
{
    myHistTrendObject.getAvailableItems(myItemsMap);

    for (auto ipr : myItemsMap)
    {
        TestingLogger::log("Items from hist data provider - " + ipr.first);
        for (ItemDef idef : myItemsMap[ipr.first])
        {
            TestingLogger::log("Name: " + idef.name + ", type: " + std::to_string(static_cast<int>(idef.type))
                               + ", source: " + idef.dataSource + ", desc.: " + idef.description
                               + ", min: " + std::to_string(idef.rangeMin) + ", max: " + std::to_string(idef.rangeMax));
        }
    }
}

// Adding 2 items with default parameters defined by provider
void HistTrendWindow::add_2different_and_1same_item()
{
    ItemDef tmpItem;

    TestingLogger::log("======= Add 2 items =======");
    tmpItem.name = "Hour";
    myHistTrendObject.addItem(tmpItem, "RealTimeFncs");
    tmpItem.name = "Minute";
    myHistTrendObject.addItem(tmpItem, "RealTimeFncs");
    myHistTrendObject.addItem(tmpItem, "RealTimeFncs");//Item with same name from same provider - No action
    myHistTrendObject.selectItem(tmpItem.name, "RealTimeFncs");
}

// Display currently selected item
void HistTrendWindow::show_selected()
{
    TrendObjectItem* selectedItem = nullptr;

    TestingLogger::log("Selected item : ");

    selectedItem = myHistTrendObject.getSelectedItem();
    if (selectedItem != nullptr)
    {
        TestingLogger::log("Name : " + selectedItem->defParam.name
                           + ", provider : " + selectedItem->providerName);
    }
    else TestingLogger::log("nullptr");
}

// Add items, set retrieval mode and add same item with own interval
void HistTrendWindow::add_3items_with_different_intervals()
{
    ItemDef tmpItem;

    tmpItem.name = "Hour";
    tmpItem.rangeMin = 0.0;
    tmpItem.rangeMax = 24.0;
    tmpItem.type = ItemType::Analog;
    myHistTrendObject.addItem(tmpItem, "RealTimeFncs");
    tmpItem.name = "Minute";
    tmpItem.rangeMax = 60.0;
    myHistTrendObject.addItem(tmpItem, "RealTimeFncs");
    tmpItem.name = "Second";
    myHistTrendObject.addItem(tmpItem, "RealTimeFncs");

    // Setting parameters for each trend
    TrendObjectItem* selItem = myHistTrendObject.selectItem("Hour", "RealTimeFncs");
    if (selItem != nullptr)
    {
        selItem->retrievalType = RetrievalType::Resolution;
        selItem->retrievalValue = 1000;
    }
    selItem = myHistTrendObject.selectItem("Minute", "RealTimeFncs");
    if (selItem != nullptr)
    {
        selItem->retrievalType = RetrievalType::Resolution;
        selItem->retrievalValue = 1000;
    }
    selItem = myHistTrendObject.selectItem("Second", "RealTimeFncs");
    if (selItem != nullptr)
    {
        selItem->retrievalType = RetrievalType::Resolution;
        selItem->retrievalValue = 10000;
    }

    // Set own interval and add same item
    tmpItem.name = "Minute";
    TrendObjectItem* resAddItem = myHistTrendObject.addItem(tmpItem, "RealTimeFncs");
    if (resAddItem == nullptr) TestingLogger::log("Error: Item Minute without own interval already added!");
    else TestingLogger::log("Error: Multiple item Minute without own interval added.");
    selItem = myHistTrendObject.selectItem("Minute", "RealTimeFncs");
    CDT ownStart, ownEnd;
    if (selItem != nullptr)
    {
        ownStart.month = 1;
        ownEnd = ownStart + 10000;

        TrendObjectItem* itemWithSameOwnInt = myHistTrendObject.selectItem(tmpItem.name, "RealTimeFncs", true, ownStart, ownEnd);

        if (itemWithSameOwnInt == nullptr)
        {
            selItem->ownInterval = true;
            selItem->ownStart = ownStart;
            selItem->ownEnd = ownEnd;
        }
        else TestingLogger::log("Error: Item Minute with same own interval already added!");
    }

    resAddItem = myHistTrendObject.addItem(tmpItem, "RealTimeFncs");
    if (resAddItem == 0) TestingLogger::log("Error: Item Minute without own interval already added!!");
    else TestingLogger::log("Item Minute without own interval added.");

    selItem = myHistTrendObject.selectItem("Minute", "RealTimeFncs");
    if (selItem != nullptr)
    {
        selItem->retrievalType = RetrievalType::Resolution;
        selItem->retrievalValue = 1000;
    }
}

// Add items as defined in DB
void HistTrendWindow::add_3tags_from_historian()
{
    for(ItemDef x : myItemsMap["InSQLclient"])
    {
        if(x.name == "ZPO1_RYCHLOST_P1" || x.name == "ZPO1_L2_ODBRADOVAC_KONIEC_CISTENIA" || x.name == "ZPO1_HMOTNOST_PANVE")
        {
            TrendObjectItem* newItem = myHistTrendObject.addItem(x, "InSQLclient");

            newItem->retrievalType = RetrievalType::RowCount;
            newItem->retrievalValue = 10;
        }
    }
}

// Reload data from all selected items for defined time interval and display new values
void HistTrendWindow::read_historical_values(CDT cdt_start, CDT cdt_end)
{
    // Read data trend objects
    myHistTrendObject.readHistTrendsData(cdt_start, cdt_end);
    while(!myHistTrendObject.readHistTrendsData(cdt_start, cdt_end))
    {
        TestingLogger::log("try next reading");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for(int i = 0; i < (int)gTrends.size(); i++)
    {
        TestingLogger::log("Values for item index : " + std::to_string(i));
        for(int j = 0; j < (int)gTrends[i].lines.size(); j++)
        {
            TestingLogger::log(" [" + std::to_string(gTrends[i].lines[j].x1) + ", " + std::to_string(gTrends[i].lines[j].y1) + "]");
        }
    }
}

// Adding discrete item - delta retrieval testing
void HistTrendWindow::add_discrete_item()
{
    ItemDef tmpItem;

    tmpItem.name = "SomeAlarm";
    tmpItem.rangeMin = -1.0;
    tmpItem.rangeMax = 2.0;
    tmpItem.type = ItemType::Discrete;
    myHistTrendObject.addItem(tmpItem, "RealTimeFncs");
}

// Adding analog and discrete items
void HistTrendWindow::add_analog_and_discrete_items()
{
    ItemDef tmpItem;

    tmpItem.name = "Minute";
    tmpItem.rangeMin = 10.0;
    tmpItem.rangeMax = 40.0;
    tmpItem.type = ItemType::Analog;
    myItemsSelection.push_back(myHistTrendObject.addItem(tmpItem, "RealTimeFncs"));
    tmpItem.name = "SomeAlarm";
    tmpItem.rangeMin = -1.0;
    tmpItem.rangeMax = 2.0;
    tmpItem.type = ItemType::Discrete;
    myItemsSelection.push_back(myHistTrendObject.addItem(tmpItem, "RealTimeFncs"));
    tmpItem.name = "Second";
    tmpItem.rangeMin = 0.0;
    tmpItem.rangeMax = 60.0;
    tmpItem.type = ItemType::Analog;
    myItemsSelection.push_back(myHistTrendObject.addItem(tmpItem, "RealTimeFncs"));

    // Setting parameters for each trend
    TrendObjectItem* selItem = myHistTrendObject.selectItem("Minute", "RealTimeFncs");
    if (selItem != nullptr)
    {
        selItem->retrievalType = RetrievalType::Resolution;
        selItem->retrievalValue = 60000;
    }
    selItem = myHistTrendObject.selectItem("Second", "RealTimeFncs");
    if (selItem != nullptr)
    {
        selItem->retrievalType = RetrievalType::Resolution;
        selItem->retrievalValue = 70000;
    }
}

void HistTrendWindow::set_cursors(int cursorId, int value)
{
    if (cursorId == 1) myHistTrendObject.moveLeftCursor(value);
    if (cursorId == 2) myHistTrendObject.moveRightCursor(value);

    std::pair<int, int> curPos = myHistTrendObject.getCursorsPositions();

    TestingLogger::log("Current cursors positions = (" + std::to_string(curPos.first) + ", " + std::to_string(curPos.second) + ")");
    for (int i = 0; i < static_cast<int>(myItemsSelection.size()); i++)
    {
        std::pair<DataValueType, DataValueType> cursorValue = myHistTrendObject.getItemValuesFromCursors(myItemsSelection[i]);

        TestingLogger::log("Item name: " + myItemsSelection[i]->defParam.name +
                           " Left='" + CDT::CDTtoString(cursorValue.first.cdt) + "' (value=" + std::to_string(cursorValue.first.value) + ")" +
                           " Right='" + CDT::CDTtoString(cursorValue.second.cdt) + "' (value=" + std::to_string(cursorValue.second.value) + ")");

        std::string diff = CDT::getDiffInString(cursorValue.second.cdt, cursorValue.first.cdt);
        TestingLogger::log("Selected item cursor diff: " + diff);
    }

    //myHistTrendObject.calcAutoRanges();
}

// CTOR
LiveTrendWindow::LiveTrendWindow(int aCountOfTrends)
{
    for (int i = 0; i < aCountOfTrends; i++) gGraphTrends.push_back({});

    // Add providers with live data
    for (int i = 0; i < aCountOfTrends; i++)
    {
        LiveTrendDispObject* tdo = new LiveTrendDispObject(&tdc, gGraphTrends[i].gTrends, gGraphTrends[i].gGrid, TrendsDrawMode::ContinuousFlow);
        //TrendDispObject* tdo = new TrendDispObject(&tdc, TrendsDrawMode::CyclicRedraw);
        //TrendDispObject* tdo = new TrendDispObject(&tdc, TrendsDrawMode::RepeatFlow);

        tdo->resetDrawingInterfaceSize(5, 5);

        myLiveTrendObjects.push_back(tdo);
    }
}

// DTOR
LiveTrendWindow::~LiveTrendWindow()
{
    for (int i = 0; i < static_cast<int>(myLiveTrendObjects.size()); i++)
    {
        delete myLiveTrendObjects[i];
    }

    myLiveTrendObjects.clear();
}

// Get all available items grouped by provider for adding to selection
void LiveTrendWindow::show_available_items()
{
    std::map<std::string, std::vector<ItemDef>> myItemsMap;

    int counter = 0;
    for (auto tdo : myLiveTrendObjects)
    {
        TestingLogger::log("-> Trend disp object - " + std::to_string(counter++));

        myItemsMap.clear();
        tdo->getAvailableItems(myItemsMap);

        for (auto ipr : myItemsMap)
        {
            TestingLogger::log("Items from live data provider - " + ipr.first);
            for (ItemDef idef : myItemsMap[ipr.first])
            {
                TestingLogger::log("Name: " + idef.name + ", type: " + std::to_string(static_cast<int>(idef.type)));
            }
        }
    }
}

// Adding 2 different items into 2 trend objects
void LiveTrendWindow::add_2items_from_2providers()
{
    ItemDef tmpItem;

    // Adding items
    tmpItem.name = "Sine";
    tmpItem.rangeMin = 0.0;
    tmpItem.rangeMax = 30.0;
    tmpItem.type = ItemType::Analog;
    myLiveTrendObjects[0]->addItem(tmpItem, "SimFncs");
    tmpItem.name = "Minute";
    tmpItem.rangeMax = 60.0;
    myLiveTrendObjects[1]->addItem(tmpItem, "RealTimeFncs");
}

// Read current values from selected items from all trends objects and redraw trend object
void LiveTrendWindow::read_live_values()
{
    for (int x = 0; x < 10; x++)
    {
        for (int i = 0; i < static_cast<int>(myLiveTrendObjects.size()); i++)
        {
            TestingLogger::log("======= Live values for trend object nr.: " + std::to_string(i) + " =======");
            myLiveTrendObjects[i]->readLiveTrendsData();

            /*TrendObjectItem* selItem = myLiveTrendObjects[i]->getSelectedItem();
            TestingLogger::log("Values for item '" + selItem->defParam.name + "' (Provider: " + selItem->providerName + ") Own interval="
                               + std::to_string(selItem->ownInterval) + " : ");*/

            for(int valIndex = 0; valIndex < (int)gGraphTrends[i].gTrends[0].lines.size(); valIndex++)
            {
                TestingLogger::log(" [" + std::to_string(gGraphTrends[i].gTrends[0].lines[valIndex].x1) + ", " + std::to_string(gGraphTrends[i].gTrends[0].lines[valIndex].y1) + "]");
            }
        }
    }
}
