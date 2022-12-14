#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <QMainWindow>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QTextEdit>

#include <QKeyEvent>

#include <string>
#include <iostream>
#include <sstream>


#include <QThread>
#include <QTimer>
#include <QObject>

#include <QCloseEvent>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QDesktopServices>
#include <QFileInfo>




//web
//#include <QtWebEngineWidgets/QWebEngineView>
//#include <QtWebEngineWidgets/QWebEnginePage>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QLineEdit>
#include <QToolBar>
#include <QSpinBox>


#include <QMainWindow>
#include <QTimer>
#include <QList>
#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_set_com_clicked();

    void on_output_dir_set_clicked();

    void on_shutter_clicked();

    void receive_data();

    void uart_debug_setting();


private:
    Ui::MainWindow *ui;
    QTimer timer_uart_setting_wait;
    QSerialPort *serialPort = new QSerialPort();
    QTimer count_display_timer;

    void uart_send(QString cmd);

    void save_img(int);

};
#endif // MAINWINDOW_H
