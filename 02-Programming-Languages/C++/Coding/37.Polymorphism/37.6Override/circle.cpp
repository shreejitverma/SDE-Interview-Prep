/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

#include "circle.h"

Circle::Circle(double radius , std::string_view description) 
    : Oval(radius,radius,description)
{
}

Circle::~Circle()
{
}

