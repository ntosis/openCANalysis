#include "candatabank.h"
#include "ui_candatabank.h"
#include <QDebug>
#include <QSqlError>
CANdatabank::CANdatabank(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CANdatabank)
{
    ui->setupUi(this);
}

CANdatabank::~CANdatabank()
{

    mydb.commit();
    mydb.close();
    delete ui;
}

void CANdatabank::DBC_browsefile()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open CAN data bank file"),"/home",tr("sqlite (*.sqlite)"));
    fileName;
    mydb.setDatabaseName(fileName);

    if(!mydb.open()) { QMessageBox::warning(this,tr("ERROR with DataBase"),tr("CAN data bank don't work"));
     OKdataBankFlag=-1; }
    else {
    mydb.transaction();
    OKdataBankFlag =1;
    }
    //readfileDB();
}

quint16 CANdatabank::BitsToUINT16(quint8 bitstart, quint8 bitend, quint64 data)
{
    quint8 bits[64];

    // Convert from quint64 to Bit Array
    for(int i=0; i<64; ++i){
            int f=i; bool bit = (data&(1<<(63-i)));
            if(bit) bits[f]=1; else bits[f]=0;
        }
    quint16 dec=0;
    for(int i=bitstart; i<bitend+1;i++) {
        //dec += pow(2,i)*bits[i];
        dec |= ((bits[i])<<i);
    }
    return dec;
}

QString CANdatabank::CANFrameName(quint32 CANid)
{
    QString string;
    QString CANframeName=string.setNum(CANid,16).toUpper();
    QSqlQuery query(mydb);

    //db queries "+temp[0].toString()+"
     query.prepare("SELECT name FROM CAN_Frame WHERE CANID='"+string.setNum(CANid)+"' ");
     //query.bindValue(":id", 1001);
     //query.bindValue(":name", "first_name");
     //query.bindValue(":CANID", 150);
     if( !query.exec() )
        qDebug() << query.lastError();
      else
        qDebug() << "Table created!";
    while (query.next()) {

             CANframeName =query.value(0).toString();

            }
    return CANframeName;


}

QVector<QStringList> CANdatabank::CANSignalList(quint32 CANid, quint64 data, quint8 dlc)
{
    QString string;
    QString CANframeName=string.setNum(CANid,16);
    QVector<QStringList> temporary; temporary.reserve(20);
    QSqlQuery query(mydb);
    QStringList results,emptylist; emptylist<<" ";
    bool flag=false;

     query.prepare("SELECT * FROM Signals WHERE CANID='"+string.setNum(CANid)+"' ");
     if( !query.exec() )
        qDebug() << query.lastError();
      else
        qDebug() << "Table created!";
    while (query.next()) {
        flag=true;
        //******* DECLARATION
         float offset,faktor; quint16 bitofstart,bitofend; QString value; bool text_values;
         //******Store the infos from DB to variables
             results << query.value(1).toString(); //Name of signal in DB
             faktor=query.value(2).toFloat();
             offset=query.value(3).toFloat();
             bitofstart=query.value(4).toUInt();
             bitofend=query.value(5).toUInt();
             value=query.value(6).toString();
             text_values=query.value(7).toBool();
         //**************************//
         //******Execute the function for the Data value
             if(text_values) {/*psaxe xana sthn DB gia ta values  */}
             else { results.append(QString::number(BitsToUINT16(bitofstart,bitofend,data)));/* return the bitnumber*/ }
             results << value;
            temporary.push_back(results);
            }
    if(flag) return temporary;
    else  temporary.push_back(emptylist); return temporary;
}

int CANdatabank::returnOKdataBankFlag()
{
    return OKdataBankFlag;
}

quint32 CANdatabank::returnCANIdfromName(QString name)
{
     quint32 result=0;
     QSqlQuery query(mydb);
     //db queries "+temp[0].toString()+"
     query.prepare("SELECT CANID FROM CAN_Frame WHERE name='"+name+"' ");
     //query.bindValue(":id", 1001);
     //query.bindValue(":name", "first_name");
     //query.bindValue(":CANID", 150);
     if( !query.exec() )
        qDebug() << query.lastError();
      else
        qDebug() << "Table created!";
    while (query.next()) {

             result =query.value(0).toUInt();

            }
    return result;


}
