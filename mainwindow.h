#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class HyoFrame;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

    void appendLog(const QString &msg);
    void sendDBusData(const QByteArray &send);

    void makeRespReadModels(const HyoFrame *recv, HyoFrame *send);
    void makeRespReadData(const HyoFrame *recv, HyoFrame *send);
    void makeRespWriteBitData(const HyoFrame *recv, HyoFrame *send);
    void makeRespWriteData(const HyoFrame *recv, HyoFrame *send);

private slots:
    void onStartClicked();
    void onAddClicked();
    void onDelClicked();

    void onReceivedDBusData(const QByteArray &recv);
};

#endif // MAINWINDOW_H
