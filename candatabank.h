#ifndef CANDATABANK_H
#define CANDATABANK_H

#include <QWidget>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QVector>
#include "candatabank.h"

namespace Ui {
class CANdatabank;
}

class CANdatabank : public QWidget
{
    Q_OBJECT

public:
    explicit CANdatabank(QWidget *parent = 0);
    ~CANdatabank();
private slots:
    void DBC_browsefile();
    quint16 BitsToUINT16(quint8 bitstart, quint8 bitend, quint64 data);
private:
    QString fileName;
    Ui::CANdatabank *ui;
    QSqlDatabase mydb =  QSqlDatabase::addDatabase("QSQLITE");
    int OKdataBankFlag;
public slots:
    QString CANFrameName(quint32 CANid);
    QVector<QStringList> CANSignalList(quint32 CANid, quint64 data, quint8 dlc);
    int returnOKdataBankFlag();
    quint32 returnCANIdfromName(QString name);
};

#endif // CANDATABANK_H
