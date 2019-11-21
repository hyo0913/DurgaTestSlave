#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDBusConnection>
#include <QDBusMessage>

#include <QComboBox>
#include <QSpinBox>

#include "DurgaTestCommon.h"
#include "HyoFrame.h"
#include "HyoDurgaModuleForm.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->treeWidget->header()->setResizeMode(QHeaderView::Stretch);
    /*
    int width = ui->treeWidget->header()->width();
    ui->treeWidget->setColumnWidth(0, width/2);
    */

    connect(ui->pushButtonAdd, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    connect(ui->pushButtonDel, SIGNAL(clicked()), this, SLOT(onDelClicked()));
    connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(onStartClicked()));

    bool isRegisterd = QDBusConnection::sessionBus().registerObject("/com/m2i/Fwdaemon", this);
    if( isRegisterd ) {
        appendLog("QDBusConnection::sessionBus().registerObject O");
    } else {
        appendLog("QDBusConnection::sessionBus().registerObject X");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendLog(const QString &msg)
{
    ui->plainTextEditLog->appendPlainText(msg);
}

void MainWindow::sendDBusData(const QByteArray &send)
{
    QDBusMessage sendMsg = QDBusMessage::createSignal("/com/m2i/Fwdaemon", "com.m2i.Fwdaemon", "DURGA_D");
    sendMsg << send;

    bool bRet = QDBusConnection::sessionBus().send(sendMsg);
    if( bRet ) {
        appendLog(QString("Send :%1").arg(QString(send.toHex().toUpper())));
    } else {
        appendLog("Send Error.");
    }
}

void MainWindow::makeRespReadModels(const HyoFrame *recv, HyoFrame *send)
{
    uchar reqNo = 0;
    uchar command = 0;

    uchar status = 0;

    recv->setIndex(0);
    reqNo = recv->readU1();
    command = recv->readU1();

    QTreeWidgetItem* item = NULL;
    HyoDurgaModuleForm* module = NULL;
    QString model;
    QList<uint> modelCodes;

    for( int i = 0; i < ui->treeWidget->topLevelItemCount(); i++ )
    {
        item = ui->treeWidget->topLevelItem(i);
        module = static_cast<HyoDurgaModuleForm*>(ui->treeWidget->itemWidget(item, 0));
        if( module == NULL ) {
            status = 2;
            break;
        }

        model = module->model();

        if( model == ModelName_DI ) {
            modelCodes << 1;
        } else if( model == ModelName_DO ) {
            modelCodes << 2;
        } else if( model == ModelName_AI ) {
            modelCodes << 3;
        } else if( model == ModelName_AO ) {
            modelCodes << 4;
        } else {
            status = 4;
            break;
        }
    }

    send->writeU1(reqNo);
    send->writeU1(command);
    send->writeU1(status);
    if( status == 0 ) {
        send->writeU1(modelCodes.count());
        for( int i = 0; i < modelCodes.count(); i++ ) {
            send->writeU1(modelCodes.at(i));
        }
    }
}

void MainWindow::makeRespReadData(const HyoFrame *recv, HyoFrame *send)
{
    uchar reqNo = 0;
    uchar command = 0;
    uchar slotNo = 0;
    uchar pointNo = 0;
    uchar bytes = 0;

    uchar status = 0;

    recv->setIndex(0);
    reqNo = recv->readU1();
    command = recv->readU1();
    slotNo = recv->readU1()-1;
    pointNo = recv->readU1();
    bytes = recv->readU1();

    QTreeWidgetItem* item = NULL;
    HyoDurgaModuleForm* module = NULL;
    QString model;

    HyoFrame sendVal;
    float valF4 = 0;
    uchar bytesTemp = 0;

    while( bytes > 0 )
    {
        if( slotNo >= ui->treeWidget->topLevelItemCount() ) {
            status = 6;
            break;
        }

        item = ui->treeWidget->topLevelItem(slotNo);
        if( item == NULL ) {
            status = 2;
            break;
        }

        module = static_cast<HyoDurgaModuleForm*>(ui->treeWidget->itemWidget(item, 0));
        if( module == NULL ) {
            status = 2;
            break;
        }

        model = module->model();

        if( model == ModelName_DI || model == ModelName_DO ) {
            sendVal.writeU1(module->getValue().toUInt());
            bytes -= 1;
        } else if( model == ModelName_AI || model == ModelName_AO ) {
            valF4 = module->getValue().toFloat();
            bytesTemp = bytes%4;
            if( bytesTemp == 0 ) {
                bytesTemp = 4;
            }
            sendVal.writeBuf(reinterpret_cast<uchar*>(&valF4), bytesTemp);
            bytes -= bytesTemp;
        } else {
            status = 2;
            break;
        }
        slotNo++;
    }

    send->writeU1(reqNo);
    send->writeU1(command);
    send->writeU1(status);
    if( status == 0 ) {
        send->writeU1(sendVal.size());
        *send += sendVal;
    }
}

void MainWindow::makeRespWriteBitData(const HyoFrame *recv, HyoFrame *send)
{
    uchar reqNo = 0;
    uchar command = 0;
    uchar slotNo = 0;
    uchar pointNo = 0;
    uchar value = 0;

    uchar status = 0;

    recv->setIndex(0);
    reqNo = recv->readU1();
    command = recv->readU1();
    slotNo = recv->readU1()-1;
    pointNo = recv->readU1();
    value = recv->readU1();

    QTreeWidgetItem* item = NULL;
    HyoDurgaModuleForm* module = NULL;
    QString model;

    if( status == 0 )
    {
        if( slotNo >= ui->treeWidget->topLevelItemCount() ) { status = 6; }
    }

    if( status == 0 )
    {
        item = ui->treeWidget->topLevelItem(slotNo);
        if( item == NULL ) { status = 2; }
    }

    if( status == 0 )
    {
        module = static_cast<HyoDurgaModuleForm*>(ui->treeWidget->itemWidget(item, 0));
        if( module == NULL ) { status = 2; }
    }

    if( status == 0 )
    {
        model = module->model();

        if( model == ModelName_DO )
        {
            if( pointNo > 7 ) {
                status = 6;
            } else {
                if( value == 0 ) {
                    module->setValue(pointNo, false);
                } else {
                    module->setValue(pointNo, true);
                }
            }
        }
        else
        {
            status = 2;
        }
    }

    send->writeU1(reqNo);
    send->writeU1(command);
    send->writeU1(status);
}

void MainWindow::makeRespWriteData(const HyoFrame *recv, HyoFrame *send)
{
    uchar reqNo = 0;
    uchar command = 0;
    uchar slotNo = 0;
    uchar slotNoBackup = 0;
    uchar pointNo = 0;
    uchar bytes = 0;
    uchar bytesBackup = 0;

    uchar status = 0;

    recv->setIndex(0);
    reqNo = recv->readU1();
    command = recv->readU1();
    slotNo = recv->readU1()-1;
    slotNoBackup = slotNo;
    pointNo = recv->readU1();
    bytes = recv->readU1();
    bytesBackup = bytes;

    QTreeWidgetItem* item = NULL;
    HyoDurgaModuleForm* module = NULL;
    QString model;

    QVariantList values;

    union _value {
        quint8 UInt8;
        quint16 UInt16;
        quint32 UInt32;
        float Float;
    } value;

    uchar bytesTemp = 0;

    // check
    while( bytes > 0 )
    {
        if( slotNo >= ui->treeWidget->topLevelItemCount() ) {
            status = 6;
            break;
        }

        item = ui->treeWidget->topLevelItem(slotNo);
        if( item == NULL ) {
            status = 2;
            break;
        }

        module = static_cast<HyoDurgaModuleForm*>(ui->treeWidget->itemWidget(item, 0));
        if( module == NULL ) {
            status = 2;
            break;
        }

        model = module->model();

        value.UInt32 = 0;
        if( model == ModelName_DO ) {
            value.UInt8 = recv->readU1();
            values.append(value.UInt8);
            bytes -= 1;
        } else if( model == ModelName_AO ) {
            bytesTemp = bytes%4;
            if( bytesTemp == 0 ) {
                bytesTemp = 4;
            }
            recv->readBuf(reinterpret_cast<char*>(&value.Float), static_cast<int>(bytesTemp));
            values.append(value.Float);
            bytes -= bytesTemp;
        } else {
            status = 2;
            break;
        }
        slotNo++;
    }

    // apply value
    if( status == 0 )
    {
        slotNo = slotNoBackup;
        bytes = bytesBackup;
        while( bytes > 0 )
        {
            if( slotNo >= ui->treeWidget->topLevelItemCount() ) {
                status = 6;
                break;
            }

            item = ui->treeWidget->topLevelItem(slotNo);
            if( item == NULL ) {
                status = 2;
                break;
            }

            module = static_cast<HyoDurgaModuleForm*>(ui->treeWidget->itemWidget(item, 0));
            if( module == NULL ) {
                status = 2;
                break;
            }

            model = module->model();

            if( values.count() == 0 ) {
                status = 2;
                break;
            }

            value.UInt32 = 0;
            if( model == ModelName_DO ) {
                value.UInt8 = values.takeFirst().toUInt();
                module->setValue(value.UInt8);;
                bytes -= 1;
            } else if( model == ModelName_AO ) {
                bytesTemp = bytes%4;
                if( bytesTemp == 0 ) {
                    bytesTemp = 4;
                }
                value.Float = values.takeFirst().toFloat();
                module->setValue(value.Float);;
                bytes -= bytesTemp;
            } else {
                status = 2;
                break;
            }
            slotNo++;
        }
    }

    send->writeU1(reqNo);
    send->writeU1(command);
    send->writeU1(status);
}

void MainWindow::onStartClicked()
{
    const QString text = ui->pushButtonStart->text();

    if( text == "Start" )
    {
        bool isOpen = QDBusConnection::sessionBus().connect(QString(""), "/com/m2i/Fwdaemon", "com.m2i.Fwdaemon", "DURGA_H", this, SLOT(onReceivedDBusData(QByteArray)));
        if( isOpen ) {
            ui->pushButtonStart->setText("Stop");
            appendLog("DBus session connect O. DURGA_H");
        } else {
            appendLog("DBus session connect X. DURGA_H");
        }
    }
    else
    {
        bool isClose = QDBusConnection::sessionBus().disconnect(QString(""), "/com/m2i/Fwdaemon", "com.m2i.Fwdaemon", "DURGA_H", this, SLOT(onReceivedDBusData(QByteArray)));
        if( isClose ) {
            ui->pushButtonStart->setText("Start");
            appendLog("DBus session disconnect O. DURGA_H");
        } else {
            appendLog("DBus session disconnect X. DURGA_H");
        }

        QDBusConnection::sessionBus().unregisterObject("/com/m2i/Fwdaemon");
    }
}

void MainWindow::onAddClicked()
{
    /*
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
    QComboBox* comboBox = new QComboBox();

    comboBox->addItem(ModelName_DI);
    comboBox->addItem(ModelName_DO);
    comboBox->addItem(ModelName_AI);

    ui->treeWidget->setItemWidget(item, 0, comboBox);

    QSpinBox* spinBox = new QSpinBox();
    spinBox->setMinimum(0);
    spinBox->setMaximum(65535);

    ui->treeWidget->setItemWidget(item, 1, spinBox);
    */

    QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
    HyoDurgaModuleForm* module = new HyoDurgaModuleForm();

    ui->treeWidget->setItemWidget(item, 0, module);
}

void MainWindow::onDelClicked()
{
    if( ui->treeWidget->topLevelItemCount() == 0 ) { return; }
    QTreeWidgetItem* item = ui->treeWidget->takeTopLevelItem(ui->treeWidget->topLevelItemCount()-1);
    if( item != NULL )
    {
        //ui->treeWidget->removeItemWidget(item, 0);
        //ui->treeWidget->removeItemWidget(item, 1);
        delete item;
        item = NULL;
    }
}

void MainWindow::onReceivedDBusData(const QByteArray &recv)
{
    appendLog(QString("Recv :%1").arg(QString(recv.toHex().toUpper())));

    HyoFrame recvFrame(recv);
    uchar productCode = 0;
    uint size = 0;

    recvFrame.setIndex(0);
    productCode = recvFrame.readU1();
    size = recvFrame.readU4();

    if( productCode != 2 || size != static_cast<uint>(recv.size()-5) ) {
        appendLog("Received illegal frame." + recv.toHex().toUpper());
        return;
    }

    HyoFrame recvData(recv.data()+5, recv.size()-5);
    HyoFrame sendData;
    uchar command = 0;

    recvData.setIndex(1);
    command = recvData.readU1();

    switch( command )
    {
    case 0x01: makeRespReadModels(&recvData, &sendData); break;
    case 0x02: break;
    case 0x11: makeRespReadData(&recvData, &sendData); break;
    case 0x12: makeRespWriteBitData(&recvData, &sendData); break;
    case 0x13: makeRespWriteData(&recvData, &sendData); break;
    }

    if( sendData.size() <= 0 ) {
        appendLog("Error. making send data.");
        return;
    }

    HyoFrame sendFrame;
    sendFrame.writeU1(2);
    sendFrame.writeU4(sendData.size());
    sendFrame += sendData;

    sendDBusData(sendFrame.toByteArray());
}
