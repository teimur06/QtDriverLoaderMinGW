#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

#include <windows.h>
#include "driverload.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void loadSettings();
    void saveSettings();


private slots:
    void on_ButtonBrowseDriver_clicked();

    void on_ButtonRegisterDriver_clicked();

    void on_lineEditDriverPath_textEdited(const QString &arg1);

    void on_ButtonUnregisterDriver_clicked();

    void on_ButtonStart_clicked();

    void on_ButtonStop_clicked();

    void on_ButtonDeviceIoControl_clicked();

    void addLog(QString qString);

    void on_lineEditDriverPath_editingFinished();

    void on_lineDriverName_editingFinished();

private:
    Ui::MainWindow *ui;
    const QString organization = "CJ Corporation";
    const QString application = "DriverLoaderMinGW";
    QString fileDriverPath;
    QString DriverName;
    QLabel * qLabelStatusBar;
    DriverLoad driverLoad;

};

#endif // MAINWINDOW_H
