#ifndef DESKTOPDIRECTOR
#define DESKTOPDIRECTOR

#include "desktopBuilder.hpp"

class DesktopDirector
{
    private:
        DesktopBuilder* desktopBuilder;
    public:
        DesktopDirector(DesktopBuilder* pDesktopBuilder){
            desktopBuilder = pDesktopBuilder;
        }
        Desktop* getDesktop(){
            return desktopBuilder->getDesktop();
        }
        Desktop* BuildDesktop(){ 
            desktopBuilder->buildMonitor(); 
            desktopBuilder->buildKeyboard();
            desktopBuilder->buildMouse();
            desktopBuilder->buildSpeaker();
            desktopBuilder->buildRam();
            desktopBuilder->buildProcessor();
            desktopBuilder->buildMotherboard();
            return desktopBuilder->getDesktop();
            }
};

#endif /* DESKTOPDIRECTOR */