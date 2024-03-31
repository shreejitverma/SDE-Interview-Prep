#include "hpDesktopBuilder.hpp"
#include "dellDesktopBuilder.hpp"
#include "desktopDirector.hpp"

int main(){
    HpDesktopBuilder* hpDesktopBuilder = new HpDesktopBuilder();
    DellDesktopBuilder* dellDesktopBuilder = new DellDesktopBuilder();

    DesktopDirector* director1 = new DesktopDirector(hpDesktopBuilder);
    DesktopDirector* director2 = new DesktopDirector(dellDesktopBuilder);

    Desktop* hpDesktop = director1->BuildDesktop();
    Desktop* dellDesktop = director2->BuildDesktop(); 

    hpDesktop->showSpecs();
    dellDesktop->showSpecs();
    return 0;
};