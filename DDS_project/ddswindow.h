#ifndef DDSWINDOW_H
#define DDSWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

#include <../user/Library/AD9910/AD9910V2.h >

#define MHz 1000000

#define _PILSE_POSITION_SIZE 2
#define _PILSE_POSITION 0x20

#define _PULSE_SIZE 2
#define _PULSE 0x21

QT_BEGIN_NAMESPACE
namespace Ui { class ddsWindow; }
QT_END_NAMESPACE

class ddsWindow : public QMainWindow
{
    Q_OBJECT

public:
    ddsWindow(QWidget *parent = nullptr);
    ~ddsWindow();

private slots:
    void on_connect_dds_Button_clicked(bool checked);

    void on_para_set_Button_clicked();
    void on_pushButton_clicked();

    void readData();

    void on_para_set_Button_2_clicked();

    void on_para_initial_Button_clicked();

private:
    Ui::ddsWindow *ui;
    QSerialPort *serial;
};
#endif // DDSWINDOW_H
