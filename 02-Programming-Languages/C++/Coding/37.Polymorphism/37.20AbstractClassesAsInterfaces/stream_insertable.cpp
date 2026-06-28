/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

#include "stream_insertable.h"

std::ostream& operator<< (std::ostream& out,const StreamInsertable& operand){
    operand.stream_insert(out);
    return out;
}

