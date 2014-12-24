#include "sendform.h"
#include "ui_sendform.h"
//#include "mainwindow.h"

//#include <QTime>

QT_USE_NAMESPACE


SendForm::SendForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendForm)
{
    ui->setupUi(this);
    /*connect the signal to the slot and sets the timer overflow every 10 ms, the SLOT looks
    * if it's the time for a periodic message to sended.
    */
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(SendPeriodicCANmessages()));
    timer->start(10); //time specified in ms
    //reserve memory
    VectorPeriodicFrames.reserve(2000);
    //GUI optimization and settings
    //ui->pushButton_2->setEnabled(false);
    ui->treeWidget->setColumnCount(13);
    ui->treeWidget->setIconSize(QSize(100,100));
    ui->treeWidget->setMouseTracking( true );
    ui->treeWidget->viewport()->installEventFilter(this);
    // connect SIGNAL-SLOTS
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(doubleClickedThisItem(QTreeWidgetItem*,int)));
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(itemIsClicked(QTreeWidgetItem*,int)));
    //connect(mainwindow,SIGNAL(connectStatusForSendForm(QString)),this,SLOT(setConnectedStatus(QString)));

    // Set the headers and the width of the columns
    columnList << "Count" <<"Periodic" <<"id" << "Rmt"<<"Byte 1"<<"Byte 2"<<"Byte 3"<<"Byte 4"<<"Byte 5"<<"Byte 6"<<"Byte 7"<<"Byte 8"<<"";

    ui->treeWidget->setHeaderLabels(columnList);
    ui->treeWidget->setColumnWidth(count,60);
    ui->treeWidget->setColumnWidth(periodic,70);
    ui->treeWidget->setColumnWidth(remote,60);
    ui->treeWidget->setColumnWidth(id,60);
    ui->treeWidget->setColumnWidth(byte4,40);
    for(int i=4;i<13;i++) {
    ui->treeWidget->setColumnWidth(i,55);
    }
}

SendForm::~SendForm()
{
    delete ui;
}

void SendForm::on_pushButton_clicked()
{

    addTreeRoot();
}

void SendForm::addTreeRoot()
{
    //Disconnect the signal to avoid spam messages!!
   disconnect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(checkValues(QTreeWidgetItem*,int)));

   QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);

  for(int i=0; i<13;i++) {
  treeItem->setTextAlignment(i,Qt::AlignHCenter);  //Set Aligment
  }

  treeItem->setText(count, "0");
  treeItem->setText(id, "FA");
  treeItem->setText(periodic,"500");
  treeItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
  treeItem->setCheckState(periodic,Qt::Unchecked);
  treeItem->setCheckState(remote,Qt::Unchecked);
  treeItem->setIcon(sendbutton,QIcon(":/images/send_button.png"));
  for(int i=byte1;i<byte8+1;i++) {treeItem->setText(i,"00");}

  //connect again the SIGNAL to be functional
  connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(checkValues(QTreeWidgetItem*,int)));

}


void SendForm::delay(int ms)
{
    QTime timer;
    timer.start();
    int i;
    int time = timer.elapsed();
    time;
    while(timer.elapsed()<ms) {
    i++;
    }
    return;
}

QByteArray SendForm::CollectCANFrameandSendit(QTreeWidgetItem *item)
{
    bool ok;
    QByteArray message;
    message.append("#");
    int a = 12;
    QString CANID= QString("%1").arg(item->text(2).toUInt(&ok,16),8,16,QLatin1Char('0')).toUpper();
   // QString CANID= QString("%1").arg(item->text(2),8,16,0);
    if(CANID.toUInt(&ok,16)<=0x7FF) {
    message.append(CANID);
    message.append("00");
    message.append("01"); //standart bit set
    }
    else {message.append(CANID);
        message.append("01"); //extend bit set
        message.append("00");
    }
    QString Rmt;
       if(item->checkState(3)==Qt::Checked) { Rmt="01"; }
       else { Rmt="00"; }
        message.append(Rmt);
        message.append("08"); //DLC always 8 byte
    QString Byte1= QString("%1").arg(item->text(4).toUInt(&ok,16),2,16,QLatin1Char('0')).toUpper();
        message.append(Byte1);
    QString Byte2= QString("%1").arg(item->text(5).toUInt(&ok,16),2,16,QLatin1Char('0')).toUpper();
        message.append(Byte2);
    QString Byte3= QString("%1").arg(item->text(6).toUInt(&ok,16),2,16,QLatin1Char('0')).toUpper();
        message.append(Byte3);
    QString Byte4= QString("%1").arg(item->text(7).toUInt(&ok,16),2,16,QLatin1Char('0')).toUpper();;
        message.append(Byte4);
    QString Byte5= QString("%1").arg(item->text(8).toUInt(&ok,16),2,16,QLatin1Char('0')).toUpper();
        message.append(Byte5);
    QString Byte6= QString("%1").arg(item->text(9).toUInt(&ok,16),2,16,QLatin1Char('0')).toUpper();
        message.append(Byte6);
    QString Byte7= QString("%1").arg(item->text(10).toUInt(&ok,16),2,16,QLatin1Char('0')).toUpper();
        message.append(Byte7);
    QString Byte8= QString("%1").arg(item->text(11).toUInt(&ok,16),2,16,QLatin1Char('0')).toUpper();
        message.append(Byte8);
        //---
    message.append("*");
    return message;
}

QByteArray SendForm::convertCANmsgfromStructtoByteArray(CAN_Frame_periodic *msg)
{
    bool ok;
    QByteArray message;
    message.append("#");
    QString CANID= QString("%1").arg(msg->id,8,16,QLatin1Char('0')).toUpper();
   // QString CANID= QString("%1").arg(item->text(2),8,16,0);
    if(CANID.toUInt(&ok,16)<=0x7FF) {
    message.append(CANID);
    message.append("00");
    message.append("01"); //standart bit set
    }
    else {message.append(CANID);
        message.append("01"); //extend bit set
        message.append("00");
    }
        QString Rmt;
       if(msg->remote==TRUE) { Rmt="01"; }
       else { Rmt="00"; }
        message.append(Rmt);
        message.append("08"); //DLC always 8 byte
    QString Byte1= QString("%1").arg(msg->Data[0],2,16,QLatin1Char('0')).toUpper();
        message.append(Byte1);
    QString Byte2= QString("%1").arg(msg->Data[1],2,16,QLatin1Char('0')).toUpper();
        message.append(Byte2);
    QString Byte3= QString("%1").arg(msg->Data[2],2,16,QLatin1Char('0')).toUpper();
        message.append(Byte3);
    QString Byte4= QString("%1").arg(msg->Data[3],2,16,QLatin1Char('0')).toUpper();;
        message.append(Byte4);
    QString Byte5= QString("%1").arg(msg->Data[4],2,16,QLatin1Char('0')).toUpper();
        message.append(Byte5);
    QString Byte6= QString("%1").arg(msg->Data[5],2,16,QLatin1Char('0')).toUpper();
        message.append(Byte6);
    QString Byte7= QString("%1").arg(msg->Data[6],2,16,QLatin1Char('0')).toUpper();
        message.append(Byte7);
    QString Byte8= QString("%1").arg(msg->Data[7],2,16,QLatin1Char('0')).toUpper();
        message.append(Byte8);
        //---
    message.append("*");
    return message;
}

void SendForm::checkValues(QTreeWidgetItem *item, int column)  // SLOT emited from SIGNAL itemchanged()
{

//if(column==4) checkByte(item->text(column),column,item);

switch(column) {
case periodic: periodicsCheckBoxReaction(item,column); break;
case id: checkId(item->text(column),column,item);     break;
case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: checkByte(item->text(column),column,item);     break;
case 3: remoteCheckBoxReaction(column,item); break;
default: break;
}


}

void SendForm::doubleClickedThisItem(QTreeWidgetItem *item, int column)  //SLOT emited from itemDoubledClicked()
{
       Qt::ItemFlags tmp = item->flags();
       ui->treeWidget->blockSignals(true);

       if (isEditableTheColumn(column)&&(item->checkState(3)==Qt::Unchecked)) {
           item->setFlags(tmp | Qt::ItemIsEditable);
       }
       else if(isEditableTheColumn(column)&&(item->checkState(3)==Qt::Checked)&&(column==id)) {
               item->setFlags(tmp | Qt::ItemIsEditable);}
       else if (tmp & Qt::ItemIsEditable) {
           item->setFlags(tmp ^ Qt::ItemIsEditable);
       }

       ui->treeWidget->blockSignals(false);

}

void SendForm::periodicsCheckBoxReaction(QTreeWidgetItem *item, int column)
{
    if((item->checkState(column)==Qt::Checked)&&(column==periodic)) {
        putItemsInfosInStruct(item);
    }
    else if((item->checkState(column)==Qt::Unchecked)&&(column==periodic))
    {
        removeItemFromStruct(item);
    }
}
//23-11
void SendForm::itemIsClicked(QTreeWidgetItem *item, int column)
{

    if(column==12) { emit CANmessageToSend(CollectCANFrameandSendit(item));  }
}

bool SendForm::isEditableTheColumn(int column)
{
    switch(column) {
    case periodic: case byte1: case byte2: case byte3: case byte4: case byte5: case byte6: case byte7: case byte8: case id: return true;
    case remote: case sendbutton: case count: return false;
    }

}

QString SendForm::whatsIsTheColumn(int column)
{
    switch(column)
    case byte1: return "byte";

}

void SendForm::checkByte(QString byte, int column, QTreeWidgetItem* item)
{
    bool ok;
    int bytetoint = byte.toUInt(&ok,16);
    if(bytetoint>255) {
        QMessageBox::warning(this,"ERROR","Byte"+byte.setNum(column-3)+" it's higher than 255 or FF");
        item->setText(column,"FF");
    }
    else if(!ok) {
        QMessageBox::warning(this,"ERROR","Byte"+byte.setNum(column-3)+" it's lot valid");
        item->setText(column,"FF");
    }
    else {item->setText(column,byte.toUpper()); } //Update to upercase for same format

}

void SendForm::checkId(QString Id, int column, QTreeWidgetItem *item)
{
    bool ok;
    int idtoint = Id.toUInt(&ok,16);
    if(idtoint>0x1FFFFFFF) {
        QMessageBox::warning(this,"ERROR","Id it's overflowed");
        item->setText(column,"FA");
    }
    else if(!ok) {
        QMessageBox::warning(this,"ERROR","Id it's lot valid");
        item->setText(column,"FA");
    }
    else if(idtoint<=0) {
        QMessageBox::warning(this,"ERROR","Id it's negative or equal to NULL");
        item->setText(column,"FA");
    }
    else {item->setText(column,Id.toUpper()); }

}

void SendForm::remoteCheckBoxReaction(int column, QTreeWidgetItem *item)
{
    //This action is implemeted in function CollectCANFrameandSendit
}

void SendForm::SendPeriodicCANmessages()
{
    if(VectorPeriodicFrames.size()>0) {
        for(int i=0; i<VectorPeriodicFrames.size(); i++) {
            CAN_Frame_periodic temp=VectorPeriodicFrames[i];
            quint32 dt = QDateTime::currentMSecsSinceEpoch() - temp.time_stamp;
            //qDebug() << dt%temp.period;
            if(((dt%temp.period)==0)&&(temp.periodic==TRUE)) {
                //qDebug() << "ok";
                emit periodicMsgSend_CHRONO(convertCANmsgfromStructtoByteArray(&temp)); }
        }
    }
}

void SendForm::putItemsInfosInStruct(QTreeWidgetItem *item)
{
    CAN_Frame_periodic temp; bool ok;
    temp.id =item->text(id).toUInt(&ok,16);
    if(temp.id<=0x7FF) {temp.extended=FALSE; temp.standart=TRUE;}
    else {temp.extended=TRUE; temp.standart=FALSE;}
    if(item->checkState(remote)==Qt::Checked) { temp.remote=TRUE; }
    else { temp.remote=FALSE; }
    temp.period = item->text(periodic).toUInt(&ok,10);
    temp.periodic = TRUE;
    temp.DLC=8;
    for(int i=0; i<temp.DLC;i++)  temp.Data[i]=item->text(byte1+i).toUInt(&ok,16);
    temp.time_stamp=QDateTime::currentMSecsSinceEpoch();
    VectorPeriodicFrames.push_back(temp);

}

void SendForm::removeItemFromStruct(QTreeWidgetItem *item)
{
    if(VectorPeriodicFrames.size()>0){
        bool ok;
        int msgtoremove =item->text(id).toUInt(&ok,16);
        int idx=ReturnIndexOfSearch(msgtoremove);
        if(idx>-1) VectorPeriodicFrames.remove(idx);
        else if(idx==-1) return;
    }
}

int SendForm::ReturnIndexOfSearch(quint32 id)
{
    if(VectorPeriodicFrames.size()>0) {
        for(int i=0;i<VectorPeriodicFrames.size();i++) {
                CAN_Frame_periodic temp;
                temp = VectorPeriodicFrames[i];

                     if(id==temp.id) { return i; break;}

                     else if(i==(VectorPeriodicFrames.size()-1)) { return -1;}
                 }
}
}

void SendForm::setConnectedStatus(const QString &status)
{
    ui->statusLabel->setText(status);
}
void SendForm::closeEvent ( QCloseEvent * event )
    {

        event->ignore();
        if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation?",
                              "Are you sure you want to close this window?",
                              QMessageBox::Yes|QMessageBox::No))
        {
        event->accept();
        }
    }

/*
bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    qDebug() << ev->type();
    if (ev->type() == QEvent::MouseButtonRelease )
        {
            if ( obj == ui->treeWidget->viewport() ) // changed here
            {
                QTreeWidgetItem *item = ui->treeWidget->currentItem();
                item->setIcon(12,QIcon("send_off.png"));
            }
        }

}
*/
