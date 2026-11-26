#include "TestingLogger.h"
#include "DateTimeFnc.h"
#include <iostream>
#include <fstream>

void TestingLogger::log(std::string aLog)
{
    std::string finLog, dtm, logFileName;

    CDT dtf;
    dtm = dtf.now();

    finLog = dtm + " " + aLog;
    std::cout << finLog << std::endl;

    logFileName = "logs/" + dtm.substr(0, 10) + "_testing.log";
    std::ofstream f;
    f.open(logFileName, std::ofstream::app);
    f << finLog << std::endl;
    f.close();
}
