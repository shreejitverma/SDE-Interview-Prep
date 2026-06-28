/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

#include "rectangle.h"

Rectangle::Rectangle(double width , double height,std::string_view  description)
    : Shape(description), m_width(width), m_height(height)
{
}



