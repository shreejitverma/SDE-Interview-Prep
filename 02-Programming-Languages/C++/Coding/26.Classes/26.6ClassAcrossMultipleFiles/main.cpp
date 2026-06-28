/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

#include <iostream>
#include "cylinder.h"

int main(){
    Cylinder cylinder1(10,10);
    std::cout << "volume : " << cylinder1.volume() << std::endl;
    
    return 0;
}