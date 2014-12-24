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
    delete ui;
    mydb.commit();
    mydb.close();
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

QString CANdatabank::CANFrameName(quint32 CANid)
{
    QString string;
    QString CANframeName=string.setNum(CANid,16);
    QSqlQuery query(mydb);

    //db queries "+temp[0].toString()+"
    int t=1500;
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

QVector<QStringList> CANdatabank::CANSignalList(quint32 CANid)
{
    QString string;
    QString CANframeName=string.setNum(CANid,16);
    QVector<QStringList> temporary; temporary.reserve(20);
    QSqlQuery query(mydb);

     query.prepare("SELECT * FROM Signals WHERE CANID='"+string.setNum(CANid)+"' ");
     if( !query.exec() )
        qDebug() << query.lastError();
      else
        qDebug() << "Table created!";
    while (query.next()) {

             QStringList results;
             results << query.value(1).toString();
            temporary.push_back(results);
            }
    return temporary;
}

int CANdatabank::returnOKdataBankFlag()
{
    return OKdataBankFlag;
}
