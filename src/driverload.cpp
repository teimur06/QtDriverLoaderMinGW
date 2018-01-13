#include "driverload.h"



DriverLoad::DriverLoad(QString DriverName, QString DriverPath):
    hSCManager(0),
    hService(0),
    wDriverName(NULL),
    wDriverPath(NULL),
    error(0)
{
    reinitialize(DriverName,DriverPath);
}

DriverLoad::DriverLoad():
    hSCManager(0),
    hService(0),
    wDriverName(NULL),
    wDriverPath(NULL),
    error(0)
{

}

DriverLoad::~DriverLoad()
{
    closeSCManager();
    closehService();
    delPtr(wDriverName);
    delPtr(wDriverPath);
}

bool DriverLoad::reinitialize(QString DriverName, QString DriverPath)
{
    driverName = DriverName;
    driverPath = DriverPath;


    if (!generateWCHARofQString(driverName,  &wDriverName)) return false;
    if (!generateWCHARofQString(driverPath,  &wDriverPath)) return false;

    if (!closehService()) return false;

    return true;
}

bool DriverLoad::openSCManager()
{
    closeSCManager();
    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (! hSCManager) {
        emit log("OpenSCManager error: "+ QString::number(error = GetLastError()));
        return false;
    }
    else emit log("OpenSCManager: Ok");

    return true;
}

bool DriverLoad::closeSCManager()
{
    if (hSCManager)
        if (!CloseServiceHandle(hSCManager)){
           emit log("CloseServiceHandle error: "+ QString::number(error = GetLastError()));
           return false;
        }

    hSCManager = NULL;
    return true;
}

bool DriverLoad::registerDriver()
{
    if (!closehService()) return false;
    if (!wDriverName) return false;
    if (!wDriverPath) return false;
    if (!hSCManager) return false;


    hService = CreateService(hSCManager, wDriverName,
                             wDriverName,
                             SERVICE_START | DELETE | SERVICE_STOP,
                             SERVICE_KERNEL_DRIVER,
                             SERVICE_DEMAND_START,
                             SERVICE_ERROR_IGNORE,
                             wDriverPath,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             NULL);


    if (!hService)
    {
        emit log("CreateService error: "+ QString::number(error = GetLastError()));
        return false;
    } else emit log("CreateService: Ok");

    return true;

}



bool DriverLoad::unregisterDriver()
{
    if (!hService) return false;
    if (DeleteService(hService) == FALSE)
    {
        emit log("Unregister! DeleteService error: "+ QString::number(error = GetLastError()));
        return false;
    } else emit log("DeleteService: Ok");


    closehService();
    return true;
}

bool DriverLoad::closehService()
{
    if (hService)
        if (CloseServiceHandle(hService) == FALSE) {
            emit log("CloseServiceHandle error: "+ QString::number(error = GetLastError()));
            return false;
        }
    hService = NULL;
    return true;
}


bool DriverLoad::generateWCHARofQString(QString &qString, WCHAR ** _wChar)
{
    delPtr((*_wChar));
    if (qString.isEmpty()) return false;

    (*_wChar) = new WCHAR[qString.length() + 1];
    (*_wChar)[qString.length()] = 0;
    qString.toWCharArray((*_wChar));
    return true;
}


bool DriverLoad::openServices()
{
    if (!closehService()) return false;
    if (!wDriverName) return false;
    if (!hSCManager) return false;
    hService = OpenService(hSCManager, wDriverName , SERVICE_START | DELETE | SERVICE_STOP);
    if (!hService)
    {
        emit log("OpenService error: "+ QString::number(error = GetLastError()));
        return false;
    } emit log("OpenService: OK");

    return true;
}

bool DriverLoad::startDriver()
{
    if (!hService) return false;
    if (StartService(hService, 0, NULL) == FALSE)
    {
       emit log("StartService error: "+ QString::number(error = GetLastError()));
       return false;
    } else emit log("Start Ok!");

    return true;
}

bool DriverLoad::stopDriver()
{
    if (!hService) return false;
    if (ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus) == 0)
    {
        emit log("Stop! ControlService error: "+ QString::number(error = GetLastError()));
        return false;
    } else emit log("Stop Ok!");
    return true;
}

bool DriverLoad::deviceIoControl(DWORD dwIoControlCode,LPVOID lpInBuffer,DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned)
{
   HANDLE hFile;

   WCHAR * drvName = NULL;
   QString temp = "\\\\.\\" + driverName;
   generateWCHARofQString(temp,&drvName);

   hFile = CreateFile(drvName,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);

     if (! hFile)
     {
        emit log("CreateFile error: "+ QString::number(error = GetLastError()));
        return false;
     }
     else emit log("CreateFile OK");


     if (
           DeviceIoControl(hFile,
                     dwIoControlCode,
                     lpInBuffer,
                     nInBufferSize,
                     lpOutBuffer,
                     nOutBufferSize,
                     lpBytesReturned,
                     NULL)
           == FALSE)
     {
         emit log("DeviceIoControl error: "+ QString::number(error = GetLastError()));
         return false;
     }
     else emit log("DeviceIoControl OK");

     CloseHandle(hFile);

     delPtr(drvName);
     return true;
}

inline QString DriverLoad::getDriverName(){ return driverName;}
inline QString DriverLoad::getDriverPath(){ return driverPath;}
inline SERVICE_STATUS DriverLoad::getServiceStatus(){ return serviceStatus;}
inline SC_HANDLE DriverLoad::gethSCManager() { return hSCManager;}
inline SC_HANDLE DriverLoad::gethService() { return hService;}


int DriverLoad::getError()
{
    return error;
}
