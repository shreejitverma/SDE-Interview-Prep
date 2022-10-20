#ifndef logger_h
#define logger_h

#include <string>
#include <mutex>
using namespace std;

class Logger{
    static int ctr;
    static Logger *loggerInstance;
    static mutex mtx;
    Logger();
    Logger(const Logger &);
    //Logger(const Logger &) = delete; // disable copy constructor, delete is used to be called in public
    Logger operator=(const Logger &);
    // Logger& operator=(const Logger&) = delete; // disable assignment operator overloading assignment

public:
    static Logger *getLogger();
    void Log(string msg);
};

#endif /* logger_h */