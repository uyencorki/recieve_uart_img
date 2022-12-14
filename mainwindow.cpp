#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>


// Qt header
#include <QFileDialog>
#include <QDebug>

#include <QMainWindow>
#include <QTimer>
#include <QList>
#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeWidgetItem>




using namespace std;


bool SETTING_CONF_OK = false;
int img_cout = 0;
QString save_path ;
QString img_data [300] ;

int size_img = 0;
int r_d = 0;
QByteArray databyte_1[300];
bool stop_save = true;


// get para and set uart
void get_setting_para ()
{

}




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Set name
    this->setWindowTitle("RECEIVE_UART_IMAGE 2022/12/13 ");

    //File setting Link get
    QString file = QCoreApplication::applicationFilePath();         //Get the excute file path
    QString filePath = QFileInfo(file).absolutePath();

    //FILE and setting
    get_setting_para();   // setting uart

    // load uart
    uart_debug_setting();

    // load path
    QDir dir (".");
    save_path = dir.absolutePath();
    ui->output_dir_text->appendPlainText(save_path);



     //Set name again
//     UART DEBUG check
//     connect(&timer_uart_setting_wait,SIGNAL(timeout()),  this, SLOT(uart_debug_setting()));
//     timer_uart_setting_wait.start(100);
}

MainWindow::~MainWindow()
{
    serialPort->close();
    delete ui;
}




void MainWindow::save_img (int img_cout)
{
    // test git

    QString file_name ;
    file_name =   save_path + "/uart_img_" + QString::number(img_cout) + ".jpg";
    cout << "file_name = " << file_name.toStdString() << endl;
    bool start_write = false;
    QByteArray data_img;
    QString img_data_tol;

    QFile file(file_name);
    if (file.open(QIODevice::WriteOnly))
    {
        QString header = QString("FFD8FF");
        for (int i = 0; i < r_d ; i ++)
        {

           if ( img_data[i].contains(header) == true )
           {
               int pos_header = img_data[i].indexOf(header,Qt::CaseSensitive);
               img_data[i] = img_data[i].mid(pos_header);
               data_img = QByteArray::fromHex(img_data[i].toUtf8());
               start_write = true ;
           }

            if (start_write == true)
            {
                if (i == r_d -1 )
                {
                   QStringList final_str =  img_data[i].split(QRegExp("\\s+"), QString::SkipEmptyParts);
                   data_img = QByteArray::fromHex(final_str[0].toUtf8());
                   img_data_tol = img_data_tol + final_str[0];
                }
                else
                {
                img_data_tol += img_data[i];
                }
             }

           if (img_data_tol.length() == 0)
           {
               ui->image_show_lable->setText("header is not found, please take another picture.. ");
               img_cout = img_cout - 1;
           }


            data_img = QByteArray::fromHex(img_data_tol.toUtf8());






            }

         file.write(data_img);
    }
    file.close();
}







// related to uart

void MainWindow::uart_send(QString cmd)
{
    serialPort->write(cmd.toUtf8());
    serialPort->write("\n");
}


void MainWindow::receive_data()
{
// "Saving image, please wait a moment");

    ui->image_show_lable->setText("Saving image, please wait a moment ....");
    ui->img_name->setText("....");

    stop_save = true ;
    QByteArray databyte = serialPort->readAll();
    QString data = databyte;
   img_data[r_d] = data;
    QString match = QString("finsh");
    if ( img_data[r_d].contains(match) == true )
    {
        cout << "r_d = " << r_d << endl;
        stop_save = false ;
    }
    r_d ++;


    if (stop_save == false )
    {
        save_img (img_cout);
        QString image_name = "uart_img_";
        image_name = image_name + QString::number(img_cout) + ".jpg";
        ui->img_name->setText(image_name);
        qDebug () << "save path " << save_path << endl ;
        image_name = save_path + "/" +image_name ;
        qDebug () << "save path " << image_name << endl ;

        QPixmap pix(image_name);
        ui->image_show_lable->setPixmap(pix);
        ui->image_show_lable->setScaledContents(true);
        ui->image_show_lable->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    }



}


void MainWindow::uart_debug_setting()
{

    QByteArray com_num_dat;
    QFile file_read("com_num.conf");
    if (!file_read.open(QIODevice::ReadOnly | QIODevice::Text))
          return;

    QTextStream in(&file_read);
    // You could use readAll() here, too.
    while (!in.atEnd())
    {
          QString line = in.readLine();
          com_num_dat.append(line);
    }
    cout <<"com_num_dat " <<  com_num_dat.toStdString() << endl;
    file_read.close();

    ui->com_number->setValue(com_num_dat.toInt());


    // open COM
    QString com_mubber =  "COM";
    com_mubber = com_mubber + com_num_dat;
    serialPort->close();
    serialPort->setPortName(com_mubber);
    serialPort->open(QIODevice::ReadWrite);

    if (!serialPort->isOpen())
    {
        QString open_com = "OPEN " + com_mubber + " FAULT..." ;
        ui->image_show_lable->setText(open_com);
//        cout << " serialPort->isOpen()) " << endl;
//        //bat dau count_display
//        if(count_display_timer.isActive()==false) {
//            //bat dau lai count
//            //start
//            count_display_timer.start(1000);
//        }
        return;
    }

    //stop count_display
    if(count_display_timer.isActive()==true) count_display_timer.stop();
    //uart debug config
    switch (ui->baud_rate->value())
        {
            case 9600:
                serialPort->setBaudRate(QSerialPort::Baud9600);
                break;
            case 19200:
                serialPort->setBaudRate(QSerialPort::Baud19200);
                break;
            case 38400 :
                serialPort->setBaudRate(QSerialPort::Baud38400 );
                break ;
            case 57600:
                serialPort->setBaudRate(QSerialPort::Baud57600);
            break;
            case 115200:
                    serialPort->setBaudRate(QSerialPort::Baud115200  );
            break;
        }

    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);//
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    connect(serialPort,SIGNAL(readyRead()),this, SLOT(receive_data()));

    QString com_set = "Setting " + com_mubber + " IS DONE.";
    ui->image_show_lable->setText(com_set);
   //stop qua trinh check uart debug
    timer_uart_setting_wait.stop();




}







// related to buttom
void MainWindow::on_set_com_clicked()
{
    cout << "run to uart_debug_setting " << endl;

    // save com number
    QString com_num_str = QString::number(ui->com_number->value());
    QFile file("com_num.conf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out <<com_num_str;
    file.close();

    uart_debug_setting();
}

void MainWindow::on_output_dir_set_clicked()
{
    QString filename_1 =  QFileDialog::getExistingDirectory( this, "output", QDir::currentPath(),
                                                            QFileDialog::DontUseCustomDirectoryIcons );
    if( !filename_1.isNull() )
              {
                qDebug() << "selected file path : " << filename_1.toUtf8();
              }
    ui->output_dir_text->setPlainText(filename_1);
    save_path = filename_1;
}

void MainWindow::on_shutter_clicked()
{
    QString cmd_img = "jpeg_shutter()";
    uart_send(cmd_img);

    img_cout ++;
    r_d = 0;


//        QString x = "0FFD8FFDB004300100B0C0E0C0A100E0D0E1211101318291B181616183224261E293B343E3D3A34393841495E50414559463839526F53596164696A693F4F737B72667A5E676965FFDB004301111212181518301B1B30654339436565656565656565656565656565656565656565656565656565656565656565656565656565656565656565656565656565FFC00011080438078003012200021101031101FFC4001F0000010501010101010100000000000000000102030405060708090A0BFFC400B5100002010303020403050504040000017D01020300041105122131410613516107227114328191A1082342B1C11552D1F02433627282090A161718191A25262728292A3435363738393A434445464748494A535455565758595A636465666768696A737475767778797A838485868788898A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE1E2E3E4E5E6E7E8E9EAF1F2F3F4F5F6F7F8F9FAFFC4001F0100030101010101010101010000000000000102030405060708090A0BFFC400B51100020102040403040705040400010277000102031104052131061241510761711322328108144291A1B1C109233352F0156272D10A162434E125F11718191A262728292A35363738393A434445464748494A535455565758595A636465666768696A737475767778797A82838485868788898A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE2E3E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FAFFDA000C03010002110311003F00CFF0DDA2CD7DE6BAE4C4415FAFF9FE75DDC678AC7D3ED422646413CD6B460E2A248891643D29218608A8C53D4567";

//         QString h =  "FFD8FF"  ;
//        int a = x.indexOf(h, Qt::CaseInsensitive);

//        cout << "a "  << a << endl;

//        QString after = x.mid(a); // Apple
//        cout << "after = " <<after.toStdString() << endl;


//    QString file_name ;
//    file_name =   save_path + "/uart_img_" + QString::number(img_cout) + ".jpg";
//    int a = 0;
//    cout << "file_name = " << file_name.toStdString() << endl;
////    QString data_test = "FFD8FFDB004300100B0C0E0C0A100E0D0E1211101318291B181";
////    QString data_test = "FFD8FFDB004300100B0C0E0C0A100E0D0E1211101318291B181616183224261E293B343E3D3A34393841495E50414559";

//    QString data_test = "FFD8FFDB";

//    QFile file(file_name);
//    if (file.open(QIODevice::WriteOnly))
//    {
//           QByteArray data_img = QByteArray::fromHex(data_test.toUtf8());

//           file.write(data_img);
//    }

//    file.close();

//    img_cout ++;

//        QString data_test = "FFD8FFD";
//        cout << "data_test.length " << data_test.length() << endl ;
}
