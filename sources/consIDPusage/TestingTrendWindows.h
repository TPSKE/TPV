#ifndef TESTINGTRENDWINDOWS_H
#define TESTINGTRENDWINDOWS_H

#include "TrendDispObject.h"
#include "TestingDrawContext.h"

// Class simulates child MDI window for historical trends
class HistTrendWindow
{
public:
    HistTrendWindow();
    void show_available_items();
    void add_2different_and_1same_item();
    void show_selected();
    void add_3items_with_different_intervals();
    void add_3tags_from_historian();
    void read_historical_values(CDT cdt_start, CDT cdt_end);
    void add_discrete_item();
    void add_analog_and_discrete_items();
    void set_cursors(int cursorId, int value);

private:
    TestingDrawContext tdc;
    std::vector<TrendObjectItem*> myItemsSelection;
    ItemsByProviderType myItemsMap;
    std::vector<GraphPointsSequence> gTrends;
    GraphPointsSequence gGrid;
    GraphPointsSequence gLeftCursor;
    GraphPointsSequence gRightCursor;
    int selectedItemIndex;
    HistTrendDispObject myHistTrendObject {&tdc, gTrends, gGrid, selectedItemIndex, gLeftCursor, gRightCursor};
};

struct GraphTrendData
{
    std::vector<GraphPointsSequence> gTrends;
    GraphPointsSequence gGrid;
};

// Class simulates child MDI window for live trends
class LiveTrendWindow
{
public:
    LiveTrendWindow(int aCountOfTrends);
    ~LiveTrendWindow();
    void show_available_items();
    void add_2items_from_2providers();
    void read_live_values();

private:
    std::vector<LiveTrendDispObject*> myLiveTrendObjects;
    TestingDrawContext tdc;
    std::vector<GraphTrendData> gGraphTrends;
};

#endif // TESTINGTRENDWINDOWS_H
