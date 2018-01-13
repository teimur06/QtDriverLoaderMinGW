#ifndef DRIVERLOAD_H
#define DRIVERLOAD_H

#include <QObject>
#include <windows.h>
#include "names.h"

#define delPtr(ptr) if (ptr) delete ptr


class DriverLoad: public QObject
{
        Q_OBJECT

public:
    DriverLoad(QString DriverName, QString DriverPath);
    DriverLoad();
    ~DriverLoad();
    bool reinitialize(QString DriverName, QString DriverPath);
    bool openSCManager();
    bool closeSCManager();
    bool registerDriver();
    bool unregisterDriver();
    bool startDriver();
    bool stopDriver();
    bool openServices();
    bool deviceIoControl(DWORD dwIoControlCode,LPVOID lpInBuffer,DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);
    bool generateWCHARofQString(QString &qString, WCHAR ** _wChar);
    int getError();
    QString getDriverName();
    QString getDriverPath();
    SERVICE_STATUS getServiceStatus();
    SC_HANDLE gethSCManager();
    SC_HANDLE gethService();

private:
    bool closehService();

private:
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    WCHAR * wDriverName;
    WCHAR * wDriverPath;
    int error;
    QString driverName;
    QString driverPath;
    SERVICE_STATUS serviceStatus;

signals:
    void log(QString);
};

#endif // DRIVERLOAD_H
