#ifndef DESKTOP_H
#define DESKTOP_H

#include <string>
using namespace std;

class Desktop
{
    string monitor;
    string keyboard;
    string mouse;
    string speaker;
    string ram;
    string processor;
    string motherboard;

public:
    void setMonitor(string pMonitor);
    void setKeyboard(string pKeyboard);
    void setMouse(string pMouse);
    void setSpeaker(string pSpeaker);
    void setRam(string pRam);
    void setProcessor(string pProcessor);
    void setMotherboard(string pMotherboard);
    void showSpecs();
};

#endif // DESKTOP_H