#ifndef COMMONFUNC_H
#define COMMONFUNC_H
#include <QTextEdit>
#include <QObject>
#include <mutex>

class MainWindow;
class commonfunc
{
public:
    static void GetCRC16(const char * pSrc, int nSrcLen, char * pDst);
    static unsigned short Modbus_AppCRC16(unsigned char *puchMsg , unsigned short usDataLen);

    void WriteLog(const char * strLog, bool bTime = true);
    void WriteLog(QString strLog, bool bTime = true);
    static commonfunc & Inst();
    void SetLogTextEdit(MainWindow * mainWindow);
    std::vector<unsigned char> GetRandomData(int nCount);
public:
    MainWindow * m_MainWindow;
private:
    std::mutex m_mutex;
private:
    commonfunc() { srand(time(nullptr)); }
    virtual ~commonfunc(){}
};

struct DbSubStation
{
    int id;
    QString f_ip;
    int f_port;
};

struct DbSensor
{
    int id;
    int c_fenzhan;
    int c_channel;
    int c_number;
};

#endif // COMMONFUNC_H
