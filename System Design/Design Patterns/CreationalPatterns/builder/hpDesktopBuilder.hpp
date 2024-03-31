#ifndef HPDESKTOPBUILDER_H
#define HPDESKTOPBUILDER_H
#include "desktopBuilder.hpp"

class HpDesktopBuilder : public DesktopBuilder
{
    void buildMonitor();
    void buildKeyboard();
    void buildMouse();
    void buildSpeaker();
    void buildRam();
    void buildProcessor();
    void buildMotherboard();
    
};

#endif /* HPDESKTOPBUILDER_H */