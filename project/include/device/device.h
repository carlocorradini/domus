
#ifndef _DEVICE_H
#define _DEVICE_H


typedef enum { false, true } Stato;
typedef enum { false, true } Interruttore;

typedef struct Device{
    Stato* stato;
    Interruttore* interruttore;
    void* registro;

};
#endif //_DEVICE_H
