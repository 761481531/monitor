#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
#include <QUdpSocket>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:
    void readMessage();
    void sendMessage();
    void timedisplay();



private slots:
    void on_patientButton_clicked();

    void on_deviceButton_clicked();

    void on_realtimeButton_clicked();

    void on_historyButton_clicked();

    void on_bindButton_clicked();

    void on_bindOpButton_clicked();

    void on_pushButton_4_clicked();

    void on_addpatientButton_clicked();

    void on_deletepatientButton_clicked();

    void on_searchpatientButton_clicked();

    void on_adddeviceButton_clicked();

    void on_deletedeviceButton_clicked();

    void on_searchdeviceButton_clicked();

    void on_ecgadd_Button_clicked();

    void on_ecgdelete_Button_clicked();

    void on_connectButton_clicked();

private:
    Ui::MainWidget *ui;
    QSqlDatabase db;
    QSqlQueryModel *patientComboBoxModel;
    QSqlQueryModel *deviceComboBoxModel;
    QSqlRelationalTableModel* patientDeviceModel;
    QSqlQueryModel *patientmodel;
    QSqlQueryModel *devicemodel;
    QSqlQueryModel *historymodel;
    QUdpSocket *udpsocket;

};

#endif // MAINWIDGET_H
