#include <iostream>
#include "logger.hpp"
#include <thread>
using namespace std;
void user1logs(){
    Logger *logger1 = Logger::getLogger();
    logger1->Log("This message is from user 1");
}

void user2logs(){
    Logger *logger2 = Logger::getLogger();
    logger2->Log("This message is from user 2");
}
int main() {

    // // Logger *logger1 = new Logger();
    // Logger *logger1 = Logger::getLogger();
    // logger1->Log("This message is from user 1");
    
    // // Logger *logger2 = new Logger();
    // Logger *logger2 = Logger::getLogger();
    // logger2->Log("This message is from user 2");
    thread t1(user1logs);
    thread t2(user2logs);  
    t1.join();
    t2.join();
    
    return 0;
}