#ifndef __CCRYPTO_H__
#define __CCRYPTO_H__

#include <openssl/aes.h>

class CAes
{
    
public:
    
    CAes();
    
    ~CAes();
    
    int init(const unsigned char* userKey, int len);
    
    char* encrypt(const char* input, int inputlen, int& outputlen);
    
    char* decrypt(const char* input, int inputlen, int&outputlen);
    
    void resize(int size);
    
public:
    
    AES_KEY _enKey;
    
    AES_KEY _deKey;
    
    char* _input;
    char* _output;
    
    int _inputlen;
    int _outputlen;
};


#endif // __CCRYPTO_H__
