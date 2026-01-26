/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

#ifndef ENCRYPT_H
#define ENCRYPT_H

class Encrypt
{
public:
    char operator()( const char& param){
         return static_cast<char> (param + 3);
    }
};

#endif // ENCRYPT_H
