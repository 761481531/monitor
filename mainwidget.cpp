#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "patientmodel.h"
#include "wave.h"
#include "spo2paint.h"
#include "ibppaint.h"
#include "ecghistory.h"
#include <QLayout>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QVariant>
#include <QMessageBox>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QDateTime>
#include <QHostAddress>
#include <QTime>
#include <QTimer>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    // 1.加载数据库驱动
    db = QSqlDatabase::addDatabase("QSQLITE");
    // 2.设置数据库名
    db.setDatabaseName("../Project/database_cloude.db");
    // 3.打开数据库
    if (!db.open())
    {
        qDebug() << "打开失败";
    }

   patientComboBoxModel = new QSqlQueryModel(this);
   deviceComboBoxModel = new QSqlQueryModel(this);

   ui->patientComboBox->setModel(patientComboBoxModel);
   ui->deviceCombox->setModel(deviceComboBoxModel);

   patientComboBoxModel->setQuery("select patientId from patient where patientId not in (select patientId from patient_device);", db);
   deviceComboBoxModel->setQuery("select dev_serial from device where dev_id not in (select devId from patient_device);", db);


   // 绑定列表显示
   patientDeviceModel = new QSqlRelationalTableModel(this,db);
   patientDeviceModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
   patientDeviceModel->setTable("patient_device");  // 设置模型操作的表。
   patientDeviceModel->setRelation(1, QSqlRelation("patient", "patientId", "name"));
   patientDeviceModel->setRelation(2, QSqlRelation("device", "dev_id", "dev_serial"));
   patientDeviceModel->select();
   ui->patient_device_tableView->setModel(patientDeviceModel);
   // 设置代理
   ui->patient_device_tableView->setItemDelegate(new QSqlRelationalDelegate(ui->patient_device_tableView));


   //病人列表
   patientmodel = new PatientModel(this);
   //关联模型与表格
   patientmodel->setQuery("select * from patient", db);

   //关联模型与视图
   ui->patienttableView->setModel(patientmodel);

   //修改视图的显示方式
   patientmodel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID号"));
   patientmodel->setHeaderData(1, Qt::Horizontal, QObject::tr("姓名"));
   patientmodel->setHeaderData(2, Qt::Horizontal, QObject::tr("性别"));
   patientmodel->setHeaderData(3, Qt::Horizontal, QObject::tr("年龄"));

   //设备列表
   devicemodel = new QSqlQueryModel(this);
   devicemodel->setQuery("select * from device", db);
   ui->devicetableView->setModel(devicemodel);
   //修改视图的显示方式
   devicemodel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID号"));
   devicemodel->setHeaderData(1, Qt::Horizontal, QObject::tr("设备名"));
   devicemodel->setHeaderData(2, Qt::Horizontal, QObject::tr("刷新时间"));

   //波形显示
   Wave *pw= new Wave(ui->Wavewidget);
   QVBoxLayout *Wavelayout = new QVBoxLayout(ui->Wavewidget);
   Wavelayout->addWidget(pw);
   Wavelayout->setSpacing(0);
   Wavelayout->setContentsMargins(0,0,0,0);

   SpO2paint *pw2 = new SpO2paint(ui->Spo2widget);
   QVBoxLayout *spo2Wavelayout = new QVBoxLayout(ui->Spo2widget);
   spo2Wavelayout->addWidget(pw2);
   spo2Wavelayout->setSpacing(0);
   spo2Wavelayout->setContentsMargins(0,0,0,0);

   Ibppaint *pw3 = new Ibppaint(ui->Ibpwidget);
   QVBoxLayout *IbpWavelayout = new QVBoxLayout(ui->Ibpwidget);
   IbpWavelayout->addWidget(pw3);
   IbpWavelayout->setSpacing(0);
   IbpWavelayout->setContentsMargins(0,0,0,0);

   QTimer *timedisplay = new QTimer(this);
   connect(timedisplay,SIGNAL(timeout()),this,SLOT(timedisplay()));
   timedisplay->start(10);

   //历史波形数据插入
   historymodel = new QSqlQueryModel(this);
   historymodel->setQuery("select * from sample", db);
   ui->ecgdatainsert_tableView->setModel(historymodel);
   historymodel->setHeaderData(0, Qt::Horizontal, QObject::tr("历史ID"));
   historymodel->setHeaderData(1, Qt::Horizontal, QObject::tr("设备ID"));
   historymodel->setHeaderData(2, Qt::Horizontal, QObject::tr("数据"));
   historymodel->setHeaderData(3, Qt::Horizontal, QObject::tr("刷新时间"));

   //连接设备
   ui->port_lineEdit->setText("8899");
   ui->IP_lineEdit->setText("127.0.0.1");
   udpsocket = new QUdpSocket(this);

   connect(ui->sendButton, &QPushButton::clicked,
           this, &MainWidget::sendMessage);

   connect(udpsocket, &QUdpSocket::readyRead,
           this, &MainWidget::readMessage);
   if(!udpsocket->bind(QHostAddress::Any, 8888))
   {
       qDebug() << "绑定失败";
   }
   else
   {
       qDebug() << "绑定成功";
   }


}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::readMessage()
{
    QHostAddress peerAddress;
    quint16 peerPort;

    QByteArray ba;

    ba.resize(udpsocket->pendingDatagramSize());

    qint64 nread = udpsocket->readDatagram(ba.data(), ba.size(), &peerAddress, &peerPort);
    if(nread!=-1)
    {
        ui->recv_lineEdit->setText(QString::fromUtf8(ba));
        qDebug() << peerAddress;
        qDebug() << peerPort;
    }
    else
    {
        qDebug() << "接收失败";
    }
}

void MainWidget::sendMessage()
{
    quint16 port = ui->port_lineEdit->text().toUInt();
    QString strIP = ui->IP_lineEdit->text();

    QString content = ui->send_lineEdit->text();
    QByteArray ba = content.toUtf8();

    qint64 nwrite = udpsocket->writeDatagram(ba.data(), ba.size(), QHostAddress(strIP), port);

    if(nwrite!= -1)
    {
        qDebug() << "发送成功";
    }
    else
    {
        qDebug() << "发送失败";
    }
}

void MainWidget::timedisplay()
{
    QTime time=QTime::currentTime();
    ui->timelabel->setText(time.toString("AP hh:mm:ss"));
    ui->timelabel->setStyleSheet("color: rgb(255, 255, 255);");
}


void MainWidget::on_patientButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}

void MainWidget::on_deviceButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWidget::on_bindButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWidget::on_realtimeButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWidget::on_historyButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);

}


// 绑定
void MainWidget::on_bindOpButton_clicked()
{
    // 病人id
    int patientId = ui->patientComboBox->currentText().toInt();


    // 设备id
    int devId;
    QString serial = ui->deviceCombox->currentText();
    QSqlQuery query(db);
    query.prepare("select dev_id from device where dev_serial = :serial;");
    query.bindValue(":serial", QVariant(serial));
    if (!query.exec())
    {
        qDebug() << "绑定失败！";
        QMessageBox::warning(this, "提示", "绑定失败", QMessageBox::Ok);
        return;
    }
    if (query.next())
    {
        devId = query.value(0).toInt();
    }

    // 绑定
    query.prepare("insert into patient_device values(null, :patientId, :devId);");
    query.bindValue(":patientId", patientId);
    query.bindValue(":devId", devId);
    if (!query.exec())
    {
        qDebug() << "绑定失败";
        return ;
    }
    qDebug() << "绑定成功";

    // 刷新绑定列表模型

    // 刷新病人comboBox模型 // 刷新设备comboBox模型
    patientComboBoxModel->setQuery("select patientId from patient where patientId not in (select patientId from patient_device);", db);
    deviceComboBoxModel->setQuery("select dev_serial from device where dev_id not in (select devId from patient_device);", db);

}

void MainWidget::on_pushButton_4_clicked()
{
    patientDeviceModel->submitAll();
}

void MainWidget::on_addpatientButton_clicked()
{
    QSqlQuery query(db);
    if(!query.exec("insert into patient values(null, '输入姓名', '输入性别', '输入年龄');"))
    {
        qDebug() << "添加失败" <<query.lastError().text();
        return;
    }

    patientmodel->setQuery("select * from patient;");
}

void MainWidget::on_deletepatientButton_clicked()
{
    QString patientdeletecontent = ui->patientdelete_lineEdit->text();
    QString sql;
    if(patientdeletecontent.isEmpty())
    {
        ui->patientdelete_lineEdit->setText("请输入病人ID后再点击删除");
    }
    else
    {
        sql = QString("delete from patient where patientId ='%1';").arg(patientdeletecontent);
        patientmodel->setQuery(sql,db);
    }
    patientmodel->setQuery("select * from patient;");
}



void MainWidget::on_searchpatientButton_clicked()
{
    QString content = ui->lineEdit->text();
    QString sql;
    if(content.isEmpty())
    {
        sql = "select * from patient";
    }
    else
    {
        sql = QString("select * from patient where name='%1';").arg(content);
    }


    patientmodel->setQuery(sql, db);
}

void MainWidget::on_adddeviceButton_clicked()
{
    QSqlQuery query(db);
    if(!query.exec("insert into device values(null, 'dev_new','2020-7-28');"))
    {
        qDebug() << "添加失败" <<query.lastError().text();
        return;
    }

    devicemodel->setQuery("select * from device;");
}

void MainWidget::on_deletedeviceButton_clicked()
{
    QString devicedeletecontent = ui->devicedelete_lineEdit->text();
    QString sql;
    if(devicedeletecontent.isEmpty())
    {
        ui->devicedelete_lineEdit->setText("请输入设备ID后再点击删除");
    }
    else
    {
        sql = QString("delete from device where dev_id ='%1';").arg(devicedeletecontent);
        devicemodel->setQuery(sql,db);
    }
    devicemodel->setQuery("select * from device;");
}




void MainWidget::on_searchdeviceButton_clicked()
{
    QString devicecontent = ui->devicelineEdit->text();
    QString sql;
    if(devicecontent.isEmpty())
    {
        sql = "select * from device";
    }
    else
    {
        sql = QString("select * from device where dev_serial='%1';").arg(devicecontent);
    }

    devicemodel->setQuery(sql, db);
}


void MainWidget::on_ecgadd_Button_clicked()
{
    static unsigned short waveData[500];
    // 4.准备数据
    QByteArray ba;
    unsigned char ch;
    for(int i=0; i<500; i++)
    {
        ch = (ecgWaveHistory[i] >> 8) & 0xff;
        ba.append(ch);  // 高字节
        ch = ecgWaveHistory[i] & 0xff;
        ba.append(ch);  // 低字节
    }

    // 5.执行插入语句
    QSqlQuery query(db);
    QString sql = "insert into sample values(null, 2, :waveData, datetime('now', 'localtime'));";
    query.prepare(sql);
    query.bindValue(":waveData", QVariant(ba));
    if (query.exec())
    {
        qDebug() << "插入成功";
    }
    else
    {
        qDebug() << "插入失败" << query.lastError().text();
    }

    // 6.读取
    sql = "select * from sample;";
    query.exec(sql);
    while(query.next())
    {
        qDebug() << query.value(0).toInt();
        qDebug() << query.value(1).toInt();
        ba = query.value(2).toByteArray();
        qDebug() << query.value(3).toDateTime().toString("yyyy-MM-dd HH:mm:ss");
    }

    unsigned short usValue = 0;
    int cnt = 0;
    for(int i=0; i<ba.size(); i+=2)
    {
        usValue = 0;
        ch = ba.at(i);
        usValue |= (ch << 8);  // 高字节
        ch = ba.at(i+1);
        usValue |= ch&0xff;    // 低字节

        waveData[cnt++] = usValue;  // 保存
    }
    qDebug() << "保存结束";

    historymodel->setQuery("select * from sample;");

}

void MainWidget::on_ecgdelete_Button_clicked()
{
    QString ecgdeletecontent = ui->ecghistorydelete_lineEdit->text();
    QString sql;
    if(ecgdeletecontent.isEmpty())
    {
        ui->ecghistorydelete_lineEdit->setText("请输入历史ID后再点击删除");
    }
    else
    {
        sql = QString("delete from sample where sampleId ='%1';").arg(ecgdeletecontent);
        historymodel->setQuery(sql,db);
    }
    historymodel->setQuery("select * from sample;");
}

void MainWidget::on_connectButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}
