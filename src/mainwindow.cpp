#include <QFileDialog>
#include <QSettings>
#include <windows.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    loadSettings();
    qLabelStatusBar = new QLabel("StatusBar");

    ui->statusBar->setSizeGripEnabled(false);
    ui->statusBar->addWidget(qLabelStatusBar);

    QObject::connect(&driverLoad, &DriverLoad::log,this,&MainWindow::addLog);

    driverLoad.reinitialize(ui->lineDriverName->text(),ui->lineEditDriverPath->text());
    driverLoad.openSCManager();
}



MainWindow::~MainWindow()
{
    saveSettings();
    delete qLabelStatusBar;
    delete ui;
}



void MainWindow::on_ButtonBrowseDriver_clicked()
{
    QDir dir(fileDriverPath);
    QString tempFileDriverPath = QDir::toNativeSeparators( QFileDialog::getOpenFileName(this, tr("Open File *.sys"), dir.absolutePath() , tr("Driver WDM (*.sys)")) );
    if(!tempFileDriverPath.isEmpty()) fileDriverPath = tempFileDriverPath;
    if (!fileDriverPath.isEmpty())
    {
        ui->lineEditDriverPath->setText(fileDriverPath);
        on_lineEditDriverPath_editingFinished();
    }
}

void MainWindow::addLog(QString qString)
{
    ui->textEditLog->moveCursor(QTextCursor::End);
    ui->textEditLog->insertPlainText (qString+"\n");
}


void MainWindow::loadSettings()
{
    QSettings qSettings(organization, application);
    fileDriverPath = qSettings.value("fileDriverPath","C:\\").toString();
    ui->lineEditDriverPath->setText( fileDriverPath );
    DriverName = qSettings.value("DriverName","").toString();
    ui->lineDriverName->setText( DriverName );
}

void MainWindow::saveSettings()
{
    QSettings qSettings(organization, application);
    qSettings.setValue("fileDriverPath",fileDriverPath);
    qSettings.setValue("DriverName",DriverName);
}


void MainWindow::on_lineEditDriverPath_editingFinished()
{

    fileDriverPath = ui->lineEditDriverPath->text();

    QFileInfo qFileInfo(fileDriverPath);
    DriverName = qFileInfo.completeBaseName();
    ui->lineDriverName->setText(DriverName);

    driverLoad.reinitialize(DriverName,fileDriverPath);
}

void MainWindow::on_lineDriverName_editingFinished()
{
    DriverName = ui->lineDriverName->text();
    driverLoad.reinitialize(DriverName,fileDriverPath);
}

void MainWindow::on_lineEditDriverPath_textEdited(const QString &arg1)
{
    QFileInfo qFileInfo(arg1);
    DriverName = qFileInfo.completeBaseName();
    ui->lineDriverName->setText(DriverName);
}

void MainWindow::on_ButtonRegisterDriver_clicked()
{
    if(!driverLoad.registerDriver())
    {
        if (driverLoad.getError() == 1073)
        {
            driverLoad.openServices();
        }
    }
}

void MainWindow::on_ButtonUnregisterDriver_clicked()
{
    if(!driverLoad.unregisterDriver())
    {
        driverLoad.openServices();
        driverLoad.unregisterDriver();
    }
}

void MainWindow::on_ButtonStart_clicked()
{
    if(!driverLoad.startDriver())
    {
        driverLoad.openServices();
        driverLoad.startDriver();
    }
}

void MainWindow::on_ButtonStop_clicked()
{
    if(!driverLoad.stopDriver())
    {
        driverLoad.openServices();
        driverLoad.stopDriver();
    }
}

void MainWindow::on_ButtonDeviceIoControl_clicked()
{
    DATA_IRP data_in, data_out;
    data_in.code = 784;
    data_out.code = 0;

    DWORD bufferSize;

    if(!driverLoad.deviceIoControl(IOCTL_TEST, &data_in, sizeof(data_in),&data_out, sizeof(data_out), &bufferSize)) return;

    addLog("bufferSize: " + QString::number(bufferSize));
    addLog("data_out.code: " + QString::number(data_out.code));
}
