#include "ddswindow.h"
#include "ui_ddswindow.h"

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");
ddsWindow::ddsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ddsWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort;
    QString description;
    QString manufacturer;
    QString serialNumber;
    //获取可以用的串口
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    //输出当前系统可以使用的串口个数
    qDebug() << "Total numbers of ports: " << serialPortInfos.count();
    //将所有可以使用的串口设备添加到ComboBox中
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
    {
        QStringList list;
        description = serialPortInfo.description();
        manufacturer = serialPortInfo.manufacturer();
        serialNumber = serialPortInfo.serialNumber();
        list << serialPortInfo.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << serialPortInfo.systemLocation()
             << (serialPortInfo.vendorIdentifier() ? QString::number(serialPortInfo.vendorIdentifier(), 16) : blankString)
             << (serialPortInfo.productIdentifier() ? QString::number(serialPortInfo.productIdentifier(), 16) : blankString);
        ui->comboBox_serialPort->addItem(list.first(), list);
    }
    //设置波特率
    ui->comboBox_baudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->comboBox_baudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->comboBox_baudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->comboBox_baudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->comboBox_baudRate->setCurrentIndex(3);
    //设置数据位
    ui->comboBox_dataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->comboBox_dataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->comboBox_dataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->comboBox_dataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->comboBox_dataBits->setCurrentIndex(3);
    //设置奇偶校验位
    ui->comboBox_parity->addItem(tr("None"), QSerialPort::NoParity);
    ui->comboBox_parity->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->comboBox_parity->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->comboBox_parity->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->comboBox_parity->addItem(tr("Space"), QSerialPort::SpaceParity);
    //设置停止位
    ui->comboBox_stopBit->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    ui->comboBox_stopBit->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    //添加流控
    ui->comboBox_flowBit->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->comboBox_flowBit->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->comboBox_flowBit->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    //禁用发送按钮
    ui->para_set_Button->setEnabled(false);


    ui->connect_dds_Button->setText(QString("连接DDS"));
    ui->dds_connect_state->setStyleSheet(" background-color: rgb(255, 0, 0);");
    ui->dds_connect_state->setText(QString("串口已断开"));

    AD9910_Init();

    ui->start_freq->setText(QString::number(180));
    ui->stop_freq->setText(QString::number(220));
    ui->sweep_time->setText(QString::number(1000));
    ui->sys_time->setText(QString::number(4));
    ui->triger_freq->setText(QString::number(10000));
}

ddsWindow::~ddsWindow()
{
    delete ui;
}


void ddsWindow::on_connect_dds_Button_clicked(bool checked)
{
    if(checked == true)
    {
        //设置串口名字
        serial->setPortName(ui->comboBox_serialPort->currentText());
        //设置波特率
        serial->setBaudRate(ui->comboBox_baudRate->currentText().toInt());
        //设置数据位
        serial->setDataBits(QSerialPort::Data8);
        //设置奇偶校验位
        serial->setParity(QSerialPort::NoParity);
        //设置停止位
        serial->setStopBits(QSerialPort::OneStop);
        //设置流控
        serial->setFlowControl(QSerialPort::NoFlowControl);
        //打开串口
        if (serial->open(QIODevice::ReadWrite))
        {
            ui->comboBox_baudRate->setEnabled(false);
            ui->comboBox_dataBits->setEnabled(false);
            ui->comboBox_flowBit->setEnabled(false);
            ui->comboBox_parity->setEnabled(false);
            ui->comboBox_serialPort->setEnabled(false);
            ui->comboBox_stopBit->setEnabled(false);
            ui->para_set_Button->setEnabled(true);

            //信号与槽函数关联
            connect(serial, &QSerialPort::readyRead, this, &ddsWindow::readData);
        }
        ui->connect_dds_Button->setText(QString("断开DDS"));
        ui->dds_connect_state->setStyleSheet(" background-color: rgb(0, 255, 0);");
        ui->dds_connect_state->setText(ui->comboBox_serialPort->currentText());
    }
    else if(checked == false)
    {
        //关闭串口
        //serial->clear();
        serial->close();
        //serial->deleteLater();
        //恢复设置功能
        ui->comboBox_baudRate->setEnabled(true);
        ui->comboBox_dataBits->setEnabled(true);
        ui->comboBox_flowBit->setEnabled(true);
        ui->comboBox_parity->setEnabled(true);
        ui->comboBox_serialPort->setEnabled(true);
        ui->comboBox_stopBit->setEnabled(true);
        ui->para_set_Button->setEnabled(false);

        ui->connect_dds_Button->setText(QString("连接DDS"));
        ui->dds_connect_state->setStyleSheet(" background-color: rgb(255, 0, 0);");
        ui->dds_connect_state->setText(QString("串口已断开"));
    }
    qDebug()<<"DDS 状态"<<checked;
}



void ddsWindow::readData()
{
    const QByteArray info = serial->readAll();
    QByteArray hexData = info.toHex();
    QString string;
    string.prepend(hexData);// QByteArray转QString方法2
    qDebug()<<"receive info:"<<info;
    ui->textEdit_recv->append(string);
}

void ddsWindow::on_pushButton_clicked()
{
    ui->textEdit_recv->clear();
}


void ddsWindow::on_para_set_Button_clicked()
{
    QString Q_stat_freq = ui->start_freq->text();
    QString Q_stop_freq = ui->stop_freq->text();
    QString Q_sweep_time = ui->sweep_time->text();
    QString Q_sys_time = ui->sys_time->text();
    QString Q_triger_freq = ui->triger_freq->text();

    AD9910_RAMP_Chrip_generate(Q_stat_freq.toULongLong()*MHz, Q_stop_freq.toULongLong()*MHz, Q_sweep_time.toULongLong(),Q_sys_time.toULongLong());

    QByteArray Data_CFR1;
    QString data_cfr1
        = QString("%1%2%3%4%5").arg(HEAD_FREAME, 2, 16,QLatin1Char('0'))
              .arg(_CFR1_SIZE+1, 2, 16,QLatin1Char('0'))
              .arg(_CFR1, 2, 16,QLatin1Char('0'))
              .arg(AD9910.CFR1, _CFR1_SIZE*2, 16,QLatin1Char('0'))
              .arg(END_FREAME, 2, 16,QLatin1Char('0'));
    Data_CFR1 = QByteArray::fromHex(data_cfr1.toLatin1().replace("\"",""));

    QByteArray Data_CFR2;
    QString data_cfr2
        = QString("%1%2%3%4%5").arg(HEAD_FREAME, 2, 16,QLatin1Char('0'))
              .arg(_CFR2_SIZE+1, 2, 16,QLatin1Char('0'))
              .arg(_CFR2, 2, 16,QLatin1Char('0'))
              .arg(AD9910.CFR2, _CFR2_SIZE*2, 16,QLatin1Char('0'))
              .arg(END_FREAME, 2, 16,QLatin1Char('0'));
    Data_CFR2 = QByteArray::fromHex(data_cfr2.toLatin1().replace("\"",""));

    QByteArray Data_CFR3;
    QString data_cfr3
        = QString("%1%2%3%4%5").arg(HEAD_FREAME, 2, 16,QLatin1Char('0'))
              .arg(_CFR3_SIZE+1, 2, 16,QLatin1Char('0'))
              .arg(_CFR3, 2, 16,QLatin1Char('0'))
              .arg(AD9910.CFR3, _CFR3_SIZE*2, 16,QLatin1Char('0'))
              .arg(END_FREAME, 2, 16,QLatin1Char('0'));
    Data_CFR3 = QByteArray::fromHex(data_cfr3.toLatin1().replace("\"",""));

    QByteArray Data_Limit;
    QString data_Limit
        = QString("%1%2%3%4%5").arg(HEAD_FREAME, 2, 16,QLatin1Char('0'))
              .arg(_DIGITAL_RAMP_LIMIT_SIZE+1, 2, 16,QLatin1Char('0'))
              .arg(_DIGITAL_RAMP_LIMIT, 2, 16,QLatin1Char('0'))
              .arg(AD9910.Digital_Ramp_Limit, _DIGITAL_RAMP_LIMIT_SIZE*2, 16,QLatin1Char('0'))
              .arg(END_FREAME, 2, 16,QLatin1Char('0'));
    Data_Limit = QByteArray::fromHex(data_Limit.toLatin1().replace("\"",""));

    QByteArray Data_Step;
    QString data_Step
        = QString("%1%2%3%4%5").arg(HEAD_FREAME, 2, 16,QLatin1Char('0'))
              .arg(_DIGITAL_RAMP_STEP_SIZE_SIZE+1, 2, 16,QLatin1Char('0'))
              .arg(_DIGITAL_RAMP_STEP_SIZE, 2, 16,QLatin1Char('0'))
              .arg(AD9910.Digital_Ramp_Step_Size, _DIGITAL_RAMP_STEP_SIZE_SIZE*2, 16,QLatin1Char('0'))
              .arg(END_FREAME, 2, 16,QLatin1Char('0'));
    Data_Step = QByteArray::fromHex(data_Step.toLatin1().replace("\"",""));

    QByteArray Data_Rate;
    QString data_Rate
        = QString("%1%2%3%4%5").arg(HEAD_FREAME, 2, 16,QLatin1Char('0'))
              .arg(_DIGITAL_RAMP_RATE_SIZE+1, 2, 16,QLatin1Char('0'))
              .arg(_DIGITAL_RAMP_RATE, 2, 16,QLatin1Char('0'))
              .arg(AD9910.Digital_Ramp_Rate, _DIGITAL_RAMP_RATE_SIZE*2, 16,QLatin1Char('0'))
              .arg(END_FREAME, 2, 16,QLatin1Char('0'));
    Data_Rate = QByteArray::fromHex(data_Rate.toLatin1().replace("\"",""));

    QByteArray Data_Freq;
    QString data_freq
        = QString("%1%2%3%4%5").arg(HEAD_FREAME, 2, 16,QLatin1Char('0'))
              .arg(_TRIGER_FREQ_SIZE+1, 2, 16,QLatin1Char('0'))
              .arg(_TRIGER_FREQ, 2, 16,QLatin1Char('0'))
              .arg(Q_triger_freq.toUInt(), _TRIGER_FREQ_SIZE*2, 16,QLatin1Char('0'))
              .arg(END_FREAME, 2, 16,QLatin1Char('0'));
    Data_Freq = QByteArray::fromHex(data_freq.toLatin1().replace("\"",""));

    // 写入发送缓存区
    serial->write(Data_CFR1.replace("\"",""));
    serial->write(Data_CFR2.replace("\"",""));
    serial->write(Data_CFR3.replace("\"",""));
    serial->write(Data_Limit.replace("\"",""));
    serial->write(Data_Step.replace("\"",""));
    serial->write(Data_Rate.replace("\"",""));
    serial->write(Data_Freq.replace("\"",""));

    //qDebug()<<"参数设置"<<checked;

    qDebug()<<"开始频率"<<Q_stat_freq.toULongLong()*MHz;
    qDebug()<<"停止频率"<<Q_stop_freq.toULongLong()*MHz;
    qDebug()<<"扫频时间"<<Q_sweep_time.toULongLong();
    qDebug()<<"系统时间"<<Q_sys_time.toULongLong();
    qDebug()<<"CFR1_ALL_Hex:"<<data_cfr1.toLatin1().replace("\"","");
    qDebug()<<"CFR2_ALL_Hex:"<<data_cfr2.toLatin1().replace("\"","");
    qDebug()<<"CFR3_ALL_Hex:"<<data_cfr3.toLatin1().replace("\"","");
    qDebug()<<"Data_Limit_ALL_Hex:"<<data_Limit.toLatin1().replace("\"","");
    qDebug()<<"Data_Step_ALL_Hex:"<<data_Step.toLatin1().replace("\"","");
    qDebug()<<"Data_Rate_ALL_Hex:"<<data_Rate.toLatin1().replace("\"","");
    qDebug()<<"Data_Freq_ALL_Hex:"<<data_freq.toLatin1().replace("\"","");

}

