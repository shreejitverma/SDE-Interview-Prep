/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

#ifndef STREAM_INSERTABLE_H
#define STREAM_INSERTABLE_H
#include <iostream>

class StreamInsertable{
    friend std::ostream& operator<< (std::ostream& out, const StreamInsertable& operand);
    
public : 
    virtual void stream_insert(std::ostream& out)const =0;
};

#endif //STREAM_INSERTABLE_H