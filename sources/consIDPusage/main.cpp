#include <QCoreApplication>
#include "IDPContainer.h"
#include "TestingTrendWindows.h"
#include "TestingLogger.h"

#define ENTRY TestingLogger::log("////////////////////////////////////////////////////////////////////////////////////");

void Test2HistWin1LiveWinWith3TrendObjects()
{
    ENTRY;
    TestingLogger::log("======= Test2HistWin1LiveWinWith3TrendObjects =======");

    // Create historical trends windows, each window with own provider instances
    HistTrendWindow* win1 = new HistTrendWindow;
    HistTrendWindow* win2 = new HistTrendWindow;

    // Create live trends window
    LiveTrendWindow* win3 = new LiveTrendWindow(3);

    // Show items
    TestingLogger::log("======= Read available items =======");
    TestingLogger::log("--- win1 (Hist) ---");
    win1->show_available_items();

    TestingLogger::log("--- win2 (Hist) ---");
    win2->show_available_items();

    TestingLogger::log("--- win3 (Live) ---");
    win3->show_available_items();

    // Finish up
    delete win1;
    delete win2;
    delete win3;
}

void TestHistWinAndAdd2ItemsToTrend()
{
    ENTRY;
    TestingLogger::log("======= TestHistWinAndAdd2ItemsToTrend =======");

    // Create historical trends window
    HistTrendWindow* win = new HistTrendWindow;
    win->show_selected();

    // Adding items
    win->add_2different_and_1same_item();

    // First item must be selected
    win->show_selected();

    // Finish up
    delete win;
}

void TestLiveWinReadDataFor2Items()
{
    ENTRY;
    TestingLogger::log("======= TestLiveWinReadDataFor2Items =======");

    // Create live trends window
    LiveTrendWindow* win = new LiveTrendWindow(2);

    // Adding items
    win->add_2items_from_2providers();

    // Read data trend objects
    win->read_live_values();

    // Finish up
    delete win;
}

void TestHistWinReadDataFor3Items()
{
    ENTRY;
    TestingLogger::log("======= TestHistWinReadDataFor3Items =======");

    // Create hist trends window
    HistTrendWindow* win = new HistTrendWindow();

    // Adding items (1 provider only with hist. data)
    win->add_3items_with_different_intervals();

    TestingLogger::log("======= Historical values for interval from current time plus 10 seconds =======");
    CDT cdt_start, cdt_end;

    cdt_end = cdt_start + 10 * cdtSecond;

    // Read data trend objects
    win->read_historical_values(cdt_start, cdt_end);

    // Finish up
    delete win;
}

void TestHistWinReadDataForDiscreteItem()
{
    ENTRY;
    TestingLogger::log("======= TestHistWinReadDataForDiscreteItem =======");

    // Create hist trends window
    HistTrendWindow* win = new HistTrendWindow();

    // Adding digital item
    win->add_discrete_item();

    TestingLogger::log("======= Historical discrete values for last 10 minutes =======");
    CDT cdt_start, cdt_end;

    cdt_start = cdt_end - 10 * cdtMinute;
    TestingLogger::log(CDT::CDTtoString(cdt_start) + " -> " + CDT::CDTtoString(cdt_end));

    // Read data trend objects
    win->read_historical_values(cdt_start, cdt_end);

    // Finish up
    delete win;
}

void TestHistAnalogsAndDigitalReadCursors()
{
    ENTRY;
    TestingLogger::log("======= TestHistAnalogsAndDigitalReadCursors =======");

    // Create hist trends window
    HistTrendWindow* win = new HistTrendWindow();

    // Adding items
    win->add_analog_and_discrete_items();

    TestingLogger::log("======= Historical values for last 10 minutes =======");
    CDT cdt_start, cdt_end;

    cdt_start = cdt_end - 10 * cdtMinute;
    TestingLogger::log(CDT::CDTtoString(cdt_start) + " -> " + CDT::CDTtoString(cdt_end));

    // Read data trend objects
    win->read_historical_values(cdt_start, cdt_end);

    // Set cursors positions and read pointed values
    win->set_cursors(0, 0);
    win->set_cursors(1, 100);
    win->set_cursors(2, 5);

    // Finish up
    delete win;
}

void TestHistorianServerHistValues()
{
    ENTRY;
    TestingLogger::log("======= TestHistorianServerHistValues =======");

    // Create hist trends window
    HistTrendWindow* win = new HistTrendWindow();

    TestingLogger::log("======= Read available items =======");
    win->show_available_items();

    // Adding items
    win->add_3tags_from_historian();

    TestingLogger::log("======= Historical values for specified interval =======");
    CDT cdt_start(2010, 5, 1, 1, 0, 0, 0), cdt_end(2010, 5, 1, 6, 0, 0, 0);

    // Read data trend objects
    win->read_historical_values(cdt_start, cdt_end);

    // Finish up
    delete win;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.

    // Init dlls
    IDPContainer::GetContainerObject()->LoadProviders();

    // Tests
    Test2HistWin1LiveWinWith3TrendObjects();
    TestHistWinAndAdd2ItemsToTrend();
    TestLiveWinReadDataFor2Items();
    TestHistWinReadDataFor3Items();
    TestHistWinReadDataForDiscreteItem();
    TestHistAnalogsAndDigitalReadCursors();
    //TestHistorianServerHistValues();

    IDPContainer::GetContainerObject()->UnloadProviders();

    return 0;//a.exec();
}
