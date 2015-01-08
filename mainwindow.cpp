/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "console.h"
#include "settingsdialog.h"
#include "sendform.h"
#include "candatabank.h"

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QVector>
#include <QDebug>
 #include <QDateTime>
#include <QTime>
#include <QBitArray>
//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//! [0]
    ui->setupUi(this);
   // console = new Console;
    //console->setEnabled(false);
    //setCentralWidget(console);
//! [1]
    serial = new QSerialPort(this);
    settings = new SettingsDialog;
    sendform = new SendForm;
    candatabank = new CANdatabank;
   // emit connectStatusForSendForm("Disconnected");
//! [1]

    ui->treeWidget->setColumnCount(4);
    ui->statusBar->showMessage("Disconnected");


    sendform->setConnectedStatus("<font color='red'>Disconnected</font>");
    // Set the headers
    ui->baudRateComboBox->addItem("100");
    ui->baudRateComboBox->addItem("250");
    ui->baudRateComboBox->addItem("500");
    ui->baudRateComboBox->addItem("1M");
    ui->baudRateComboBox->setEnabled(false);

    ui->treeWidget->setHeaderLabels(QStringList() << "Identifier" << "counter" << "extend"<<"remote" << "data" <<"period(ms)");

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    initActionsConnections();

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

//! [2]
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
//! [2]
   // connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

    connect(this, SIGNAL(CAN_MessagesIsNotNULL()), this, SLOT(MoveFramesFromCAN_MessagestoStuct()));
//! [3]
//!
    connect(sendform,SIGNAL(CANmessageToSend(QByteArray)),this,SLOT(writeData(QByteArray)));
    connect(sendform,SIGNAL(periodicMsgSend_CHRONO(QByteArray)),this,SLOT(writeData(QByteArray)));

    connect(ui->baudRateComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changeBaudRate(int)));
    connect(ui->treeWidget,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(setIfThisItemIsExpanded(QTreeWidgetItem*)));
    connect(ui->treeWidget,SIGNAL(itemCollapsed(QTreeWidgetItem*)),this,SLOT(setIfThisItemIsCollapsed(QTreeWidgetItem*)));
//!

}
//! [3]
MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

//! [4]
//! //Open manually the Serial port to communicate with the usb dongle.
void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
          //  console->setEnabled(true);
          //  console->setLocalEchoEnabled(p.localEchoEnabled);
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actionConfigure->setEnabled(false);
            ui->baudRateComboBox->setEnabled(true);
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
           sendform->setConnectedStatus("<font color='green'>Connected</font>");
           SerialOpen = true;

    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));

        sendform->setConnectedStatus("<font color='red'>Open error</font>");
        SerialOpen = false;

    }
}
//! [4]

//! [5]
// closses manually the Serial port
void MainWindow::closeSerialPort()
{
    serial->close();
    //console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));
    sendform->setConnectedStatus("<font color='red'>Disconnected</font>");
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

//! [6]
// This function passes the data from the Mainwindow Class to serial class and writes the data to buffer.
void MainWindow::writeData(const QByteArray &data)
{

   if(isSerialOpen()) { serial->write(data); }
   else QMessageBox::critical(this, tr("Error"), tr("Hardware not found. Please connect one! "));
}
//! [6]

//! [7]
// When data arrives then emitted this Slot from Signal and the Programm reads the bytes from the buffer
void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    //calls the function to sort the bytes into a string
    DatatoString(data);
    QString test; //debug stuff
    //calls function to proccess the incomming data
    MoveFramesFromCAN_MessagestoStuct();
    if(collected_frames.size()>0) {ResultsToTreeview();}
    /* if(collected_frames.size()>0) {//debug stuff
    test = test.setNum(collected_frames[0].id,16);
    test.append('\n');
    console->putData(test); */

    //represents the data into Treeview
    //MoveFramesFromCAN_MessagestoStuct();
}
//! [7]

//! [8]
// Function to return an error if exists
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}
//! [8]
//Gui actions
void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionSend, SIGNAL(triggered()), sendform, SLOT(show()));
    connect(ui->actionClear, SIGNAL(triggered()), candatabank, SLOT(DBC_browsefile()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}
//# DatatoString(QByteArray) this function is used to read the incomming data from serial buffer,
//# We have a string CAN_Message_received and we sort the byte into the string with the correct follow,
//# always the start charachter of this String is the ASCII '#' and the last charachter is the ASCII '*'.
//# and at the end we put the String into a Varctor CAN_Messages for later proccess.
void MainWindow::DatatoString(QByteArray &data)
{
      //moved flag to global
     for(int i=0; i<data.length(); i++)
     {
         QChar f= QChar(data.at(i));


         if (f ==(startofCANframe)) {CAN_Message_received.clear(); flag = true; CAN_Message_received.push_back(startofCANframe);}  //need to be like #define startofframe #

         else if (f ==(endofCANframe)&& CAN_Message_received.length()>2) {CAN_Message_received.push_back(endofCANframe); CAN_Messages.push_back(CAN_Message_received); flag == false;}

         if(flag==true && data.at(i)!=startofCANframe ) { CAN_Message_received.push_back(data.at(i)); }
     }
}
//# MovesFramesFromCAN_MessagestoStruct();
//# This function reads the Vector CAN_Messages from the start until the end,
//# removes the '#' and '*' ASCII characters from the String,splits the string into a List.
//# removes the String from the Vector and calls SortTheFrameToStructure(CAN_Frame_splitted)
//# for an other proccess.
void MainWindow::MoveFramesFromCAN_MessagestoStuct()
{
    QStringList CAN_Frame_splitted;

    for (int i=0; i<CAN_Messages.size(); i++) {

     QString temporary = CAN_Messages[i];


     temporary.remove(QChar('#'), Qt::CaseInsensitive);
     temporary.remove(QChar('*'), Qt::CaseInsensitive);

     CAN_Frame_splitted = temporary.split(QRegExp("\\s+"), QString::SkipEmptyParts);

     CAN_Messages.remove(i);

     SortTheFrameToStructure(CAN_Frame_splitted);

    }

}
//# Move the infos to a Structure for better represantation and proccess.
void MainWindow::SortTheFrameToStructure(QStringList list) {

    k++;
    CAN_Frames R;
    bool ok;
    R.id = list[0].toULong(&ok,16);
    //---------Search the position of the frame into Vector
    if(collected_frames.size()>0) {
        qint8 idx;
        idx = ReturnIndexOfSearch(R.id);
        CAN_Frames temp;
        if(idx>-1) {
            temp=collected_frames.at(idx);
            // auxise ton counter
            R.counter=temp.counter+1;
            //Calculate the dt of this frame
            R.period(QDateTime::currentMSecsSinceEpoch(),temp);
            //Store the previous state of the item is expanded or not for the GUI.
            R.forGUI_isThisItemExpanded=temp.forGUI_isThisItemExpanded;
        }
    } else {R.counter=0;}

    //------------- fill the infos into Structure
    R.extended = list[1].toShort(&ok,10);
    R.standart = list[2].toShort(&ok,10);
    R.remote = list[3].toShort(&ok,10);
    R.DLC = list[4].toShort(&ok,10);
    for (int i=0; i<R.DLC;i++) {R.Data[i] = list[5+i].toUInt(&ok,16);}
    if(R.DLC<8) {for(int k=R.DLC;k<8;k++) R.Data[k]=0;} //Work around  NEED to TESTED
    //--------------
    //---find again the possition of the frame into vector and replace it if exist or add a new one and sort the vector
    if(collected_frames.size()>0) {
        qint8 idx;
        idx = ReturnIndexOfSearch(R.id);
       if(idx>-1) { collected_frames.replace(idx,R);  }
       else if(idx==-1) { collected_frames.push_back(R); qSort(collected_frames.begin(),collected_frames.end()); }
    }

     else if(collected_frames.size()==0) { collected_frames.push_back(R); }
}
//# For every CAN frame in the Vector collected_frames call the function addTreeRoot,
//# this function will rendering the received data on the GUI Tree View.
void MainWindow::ResultsToTreeview() {

ui->treeWidget->clear();

if (collected_frames.size()>0)
    {
                for(int i=0;i<collected_frames.size(); i++) {
                   CAN_Frames temp;
                   temp = collected_frames[i];
                  // QString a,b; a.setNum(temp.id,16); b.setNum(temp.Delta_time,16);

                   addTreeRoot(temp);
                }
    }

}


void MainWindow::addTreeRoot(CAN_Frames B)
{
//add a new row into GUI treeWidget
QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);
//some temporary strings to process the infos from the Structure.
QString ID,counter,ext,data,DeltaTime,rmt,d;
QStringList f;

   if(candatabank->returnOKdataBankFlag()==1) {
   treeItem->setTextAlignment(0,Qt::AlignRight);
   treeItem->setText(0, candatabank->CANFrameName(B.id));

   }
   else {
   ID.setNum(B.id,16); //ID to HEX
   treeItem->setTextAlignment(0,Qt::AlignRight);
   treeItem->setText(0, ID.toUpper());
    }

   counter.setNum(B.counter,10); //Counter in Decimal
   ext.setNum(B.extended,2); //Extend in binary
   rmt.setNum(B.remote,2); //remote in binary
   for(int i=0;i<B.DLC;i++) {f << d.setNum(B.Data[i],16);} //DLC Data to HEX
   data= f.join(" "); //puts all the DLC data in a String and seperates the bytes with a space
   DeltaTime.setNum(B.Delta_time,10); //dt to decimal
  // add the infos in the GUI
  treeItem->setText(1, counter.toUpper());
  treeItem->setText(2,ext);
  treeItem->setText(3,rmt);
  treeItem->setText(4,data.toUpper());
  treeItem->setText(5,DeltaTime.toUpper());
  //some graphig optimization
  ui->treeWidget->setColumnWidth(1,60);
  ui->treeWidget->setColumnWidth(2,55);
  ui->treeWidget->setColumnWidth(3,55);
  ui->treeWidget->setColumnWidth(4,200);

  if(candatabank->returnOKdataBankFlag()==1) addTreeChild(treeItem,B);
  //Set this item as expanded if the User has expanded this item before
  treeItem->setExpanded(B.forGUI_isThisItemExpanded);
}

void MainWindow::addTreeChild(QTreeWidgetItem *parent,
                  CAN_Frames B)
{
    int s =sizeof(B.Data);
    // Create a bit array of the appropriate size
    quint8 bits[64]; //here was a bug ATTENTION!

    // Convert from QByteArray to QBitArray
    for(int i=0; i<s; ++i){
        for(int b=0; b<8; ++b) {
            int f=i*8+b; bool bit = (B.Data[i]&(1<<(7-b)));
            if(bit) bits[f]=1; else bits[f]=0;
        }}
    quint64 CANDataFrameAs64bitUint=0;
    for(int i=0; i<B.DLC*8;i++) {
        //dec += pow(2,i)*bits[i];
        CANDataFrameAs64bitUint |= ((bits[i])<<i);
    }

    results  = candatabank->CANSignalList(B.id,CANDataFrameAs64bitUint,B.DLC);
    if(results.size()>0) {
        for(int i=0; i<results.size();i++) {
            QStringList temp= results[i];
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(temp);
            parent->addChild(treeItem);
        }

    }
}

int MainWindow::ReturnIndexOfSearch(quint32 id) {

    //qint8 idx;

    if(collected_frames.size()>0) {
        for(int i=0;i<collected_frames.size();i++) {
                CAN_Frames temp;
                temp = collected_frames[i];

                     if(id==temp.id) { return i; break;}

                     else if(i==(collected_frames.size()-1)) { return -1;}
                 }

    }
}

void MainWindow::changeBaudRate(int baudrate)
{
    QByteArray command;
    switch(baudrate) {
    case 0: command = "$BAUDRATE=100*";  break;//100 Kbits
    case 1: command = "$BAUDRATE=250*";  break;//250 Kbits
    case 2: command = "$BAUDRATE=500*"; break;//500 Kbits
    case 3: command = "$BAUDRATE=1000*"; break;//1 Mbits
}
    writeData(command);
}

void MainWindow::setIfThisItemIsExpanded(QTreeWidgetItem *item)
{
CAN_Frames B; bool ok;
int i = ReturnIndexOfSearch(item->text(0).toUInt(&ok,16));
if(!ok) {
        i = ReturnIndexOfSearch(candatabank->returnCANIdfromName(item->text(0)));
}
if(i>-1) {
    B=collected_frames[i];
    B.forGUI_isThisItemExpanded = true;
    collected_frames.replace(i,B);
}

}

void MainWindow::setIfThisItemIsCollapsed(QTreeWidgetItem *item)
{
CAN_Frames B; bool ok;
int i = ReturnIndexOfSearch(item->text(0).toUInt(&ok,16));
if(!ok) {
i = ReturnIndexOfSearch(candatabank->returnCANIdfromName(item->text(0)));
qDebug() <<"i="<<i;
}
if(i>-1) {
B=collected_frames[i];
B.forGUI_isThisItemExpanded = false;
collected_frames.replace(i,B);
     }

}

void MainWindow::closeEvent ( QCloseEvent * event )
    {
        if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation?",
                              "Are you sure you want to exit?",
                              QMessageBox::Yes|QMessageBox::No))
        {
        event->accept();
        sendform->close();
        } else         event->ignore();

}

bool MainWindow::isSerialOpen()
{
  return SerialOpen;
}

quint32 CAN_Frames::period(const quint64 tm, const CAN_Frames &before) {
    if(before.time_stamp>0) {this->Delta_time=tm-before.time_stamp;}
    this->time_stamp=tm;
}
