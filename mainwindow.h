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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QTreeWidget>
#include <QMainWindow>

#include <QtSerialPort/QSerialPort>

#define startofCANframe '#'
#define endofCANframe '*'

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

//class Console;
class SettingsDialog;
class SendForm;
class CANdatabank;
//This sturct represent all the data from a CAN Message and some aditional things.
struct CAN_Frames {

        quint32 id;
        bool extended;
        bool standart;
        quint8 DLC;
        quint8 Data[8];
        bool remote;
        quint64 time_stamp=0;
        quint32 Delta_time=0;
        quint32 counter=0;
        //to sort the Vector with id
        bool operator<(const CAN_Frames &a) const
            {
                return id < a.id;
            }
        //calculates the dt of the frame from the previous sended time until now
        quint32 period(const quint64 tm,const CAN_Frames &before);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void DatatoString(QByteArray &data);
    void handleError(QSerialPort::SerialPortError error);
    void MoveFramesFromCAN_MessagestoStuct();
    void SortTheFrameToStructure(QStringList list);
    void ResultsToTreeview();
    void addTreeRoot(CAN_Frames B);
    void addTreeChild(QTreeWidgetItem *parent,
                      CAN_Frames B);
    int ReturnIndexOfSearch(quint32 id);
    void changeBaudRate(int baudrate);


private:
    void initActionsConnections();
    void closeEvent ( QCloseEvent * event );
signals:
    void CAN_MessagesIsNotNULL();
    void connectStatusForSendForm(QString);

private:
    Ui::MainWindow *ui;
    //Console *console;
    SettingsDialog *settings;
    SendForm *sendform;
    QSerialPort *serial;
    CANdatabank *candatabank;
    //TOSIS Variables
    QString CAN_Message_received;
    QVector<QString> CAN_Messages;
    QVector<CAN_Frames> collected_frames;
    bool flag=false;
    quint16 k=0;
    bool SerialOpen;
public:
    bool isSerialOpen();
};

#endif // MAINWINDOW_H
