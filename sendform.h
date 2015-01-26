#ifndef SENDFORM_H
#define SENDFORM_H

#include <QWidget>
#include <QMainWindow>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QTime>
#include <QTimer>

//#include "mainwindow.h"

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE



namespace Ui {
class SendForm;
}
//class MainWindow;
class SendForm : public QWidget
{
    Q_OBJECT
    enum columns {
        count = 0,
        periodic=1,
        id=2,
        remote=3,
        byte1=4,
        byte2=5,
        byte3=6,
        byte4=7,
        byte5=8,
        byte6=9,
        byte7=10,
        byte8=11,
        sendbutton=12
    };
    struct CAN_Frame_periodic {

            quint32 id;
            bool extended;
            bool standart;
            quint8 DLC;
            quint8 Data[8];
            bool remote;
            quint64 time_stamp=0;
            quint32 period=0;
            bool periodic;
            //quint32 counter=0;

            bool operator<(const CAN_Frame_periodic &a) const
                {
                    return id < a.id;
                }
     };
public:
    explicit SendForm(QWidget *parent = 0);
    ~SendForm();
private slots:
    void on_pushButton_clicked();
    void addTreeRoot();

    void delay(int ms);
    QByteArray CollectCANFrameandSendit(QTreeWidgetItem*item);
    QByteArray convertCANmsgfromStructtoByteArray(CAN_Frame_periodic *msg);
    void checkValues( QTreeWidgetItem * item, int column);
    void doubleClickedThisItem(QTreeWidgetItem * item, int column);
    void periodicsCheckBoxReaction(QTreeWidgetItem * item, int column);
    void itemIsClicked(QTreeWidgetItem * item, int column);
    bool isEditableTheColumn(int column);
    QString whatsIsTheColumn(int column);
    void checkByte(QString byte, int column, QTreeWidgetItem *item);
    void checkId(QString Id,int column, QTreeWidgetItem* item);
    void remoteCheckBoxReaction(int column, QTreeWidgetItem* item);
    void SendPeriodicCANmessages();
    void putItemsInfosInStruct(QTreeWidgetItem* item);
    void removeItemFromStruct(QTreeWidgetItem* item);
    int ReturnIndexOfSearch(quint32 id);
private:
    Ui::SendForm *ui;
    QStringList columnList;
    void closeEvent ( QCloseEvent * event );
    QVector<CAN_Frame_periodic> VectorPeriodicFrames;    //MainWindow *mainwindow;
    quint32 timeBefore;
signals:
    //23-11
    void CANmessageToSend(QByteArray);
    void periodicMsgSend_CHRONO(QByteArray);
public:
    void setConnectedStatus(const QString &status);


};

#endif // SENDFORM_H

