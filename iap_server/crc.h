#ifndef CRC_H
#define CRC_H
#include <QObject>
class crc8:public QObject{
    public:
    unsigned char VerifyCRC8Sub(char *vData,unsigned int vLong);
    void SetCRC8Sub(char *vData,unsigned int vLong);

};
#endif // CRC_H
