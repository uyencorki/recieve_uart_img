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



int img_cout = 0;
int COM_NUM =  0;

QElapsedTimer timer_1;



bool SETTING_CONF_OK = false;
QString save_path ;
QString img_data [300] ;

int size_img = 0;
int r_d = 0;
QByteArray databyte_1[300];
bool stop_save = true;


// get para and set uart
void get_setting_para ()
{

    QStringList sFileAsList    ;
    QFile fTextFile("com_num.conf");
    QString data_str ;
    if(fTextFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream tStream(&fTextFile);
        tStream.setCodec("UTF-8");
        //write Text into QStringList
        while(!tStream.atEnd())
        {
            data_str = tStream.readAll();
        }
        cout << "data_str =  "<< endl << data_str.toStdString() << endl;
        sFileAsList = data_str.split("\n");
        for (int i = 0; i < sFileAsList.size() ; i ++)
        {
            if (sFileAsList[i].contains("image_num"))
            {
                QString pr = "=";
                int pos = sFileAsList[i].indexOf(pr,Qt::CaseSensitive);
                QString num = sFileAsList[i].mid(pos+1);
                img_cout =  num.toInt();
                if (img_cout > 10000) img_cout = 0 ;
            }

            if (sFileAsList[i].contains("COM"))
            {
                QString pr = "=";
                int pos = sFileAsList[i].indexOf(pr,Qt::CaseSensitive);
                QString num = sFileAsList[i].mid(pos+1);
                COM_NUM =  num.toInt();
            }


        }

         fTextFile.close();
    }

    return ;



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
    save_path = save_path + "/receive_data";
    ui->output_dir_text->appendPlainText(save_path);
    cout << " save_path in ma im " << save_path.toStdString() << endl;


    timer = new QTimer(this);
    // setup signal and slot
    connect(timer, SIGNAL(timeout()),this, SLOT(MyTimerSlot()));
    timer->start(100);


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

bool saving_img = false;
bool geting_data = false;
bool print = false;

float geting_data_time;
float saving_data_time;

void MainWindow::MyTimerSlot()
{

    if( (geting_data == true ) && ( print == true ))
    {
//        cout << "Data receiving time " << endl;
        QString set_time = "Data receiving time:  " + QString::number( timer_1.elapsed()/1000);
        timer_1.elapsed();
        ui->image_show_lable->setText(set_time);
        geting_data_time = timer_1.elapsed();
    }
    if ((saving_img == true ) && (print == true))
    {
        cout <<"Image saving time " << endl;
        QString set_time = "Image saving time:  " + QString::number( timer_1.elapsed()/1000);
        timer_1.elapsed();
        ui->image_show_lable->setText(set_time);
        saving_data_time = timer_1.elapsed();
        saving_data_time = saving_data_time - geting_data_time;
    }

}




void MainWindow::receive_data()
{
// "Saving image, please wait a moment");


//    ui->image_show_lable->setText("Saving image, please wait a moment ....");
    geting_data = true;

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
        QElapsedTimer timer_2;
        timer_2.start();
        geting_data = false;
        saving_img  = true;

        save_img (img_cout);
        QString image_name = "uart_img_";
        image_name = image_name + QString::number(img_cout) + ".jpg";
        ui->img_name->setText(image_name);
        qDebug () << "save path " << save_path << endl ;
        image_name = save_path + "/" +image_name ;
        qDebug () << "save path " << image_name << endl ;

        print = false;

        QPixmap pix(image_name);
        ui->image_show_lable->setPixmap(pix);
        ui->image_show_lable->setScaledContents(true);
        ui->image_show_lable->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

        saving_img  = false;


        float r_tm = timer_2.elapsed();
        r_tm = r_tm /1000 ;

        QString pri_r_tm = "Data receiving time:  " + QString::number( int(geting_data_time/1000)) + " Second";
        QString pri_s_tm = "Image saving time:    " + QString::number(r_tm) + " Second";
        ui->image_save_time->setText(pri_s_tm);
        ui->data_receive_time->setText(pri_r_tm);


    }



}

void MainWindow::uart_debug_setting()
{

    ui->com_number->setValue(COM_NUM);

    // open COM
    QString com_mubber =  "COM";
    com_mubber = com_mubber + QString::number(COM_NUM);
    serialPort->close();
    serialPort->setPortName(com_mubber);
    serialPort->open(QIODevice::ReadWrite);

    if (!serialPort->isOpen())
    {
        QString open_com = "OPEN " + com_mubber + " FAULT..." ;
        ui->image_show_lable->setText(open_com);
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
    COM_NUM = ui->com_number->value();

    QStringList sFileAsList    ;
    QFile fTextFile("com_num.conf");
    QString data_str ;
    if(fTextFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream tStream(&fTextFile);
        tStream.setCodec("UTF-8");
        //write Text into QStringList
        while(!tStream.atEnd())
        {
            data_str = tStream.readAll();
        }

        cout << "data_str =  "<< endl << data_str.toStdString() << endl;

        sFileAsList = data_str.split("\n");


        for (int i = 0; i < sFileAsList.size() ; i ++)
        {
            if (sFileAsList[i].contains("COM"))
            {
                QString pr = "=";
                int pos = sFileAsList[i].indexOf(pr,Qt::CaseSensitive);
                QString num = sFileAsList[i].mid(pos+1);
                sFileAsList[i] = sFileAsList[i].left(pos+1) +QString::number(COM_NUM);
            }
        }

        //Check if FileList is empty
        if (sFileAsList.isEmpty())
        {
            fTextFile.close();
            return ;
        }

        //Rewrite QStringList into Textfile
        tStream.seek(0);
        for (int i = 0; i < sFileAsList.size(); ++i)
        {
            if (i == ( sFileAsList.size() - 1) )
                fTextFile.write(sFileAsList[i].toUtf8()) ;// + "\n");
            else
                fTextFile.write(sFileAsList[i].toUtf8() + "\n");
        }

            fTextFile.close();
        }

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
    print = true;
    timer_1.start();

    QString cmd_img = "jpeg_shutter()";
    uart_send(cmd_img);

    img_cout ++;
    r_d = 0;

    QStringList sFileAsList    ;
    QFile fTextFile("com_num.conf");
    QString data_str ;
    if(fTextFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream tStream(&fTextFile);
        tStream.setCodec("UTF-8");
        //write Text into QStringList
        while(!tStream.atEnd())
        {
            data_str = tStream.readAll();
        }

        cout << "data_str =  "<< endl << data_str.toStdString() << endl;

        sFileAsList = data_str.split("\n");


        for (int i = 0; i < sFileAsList.size() ; i ++)
        {
            if (sFileAsList[i].contains("image_num"))
            {
                QString pr = "=";
                int pos = sFileAsList[i].indexOf(pr,Qt::CaseSensitive);
                QString num = sFileAsList[i].mid(pos+1);
                sFileAsList[i] = sFileAsList[i].left(pos+1) +QString::number(img_cout);
            }
        }

        //Check if FileList is empty
        if (sFileAsList.isEmpty())
        {
            fTextFile.close();
            return ;
        }

        //Rewrite QStringList into Textfile
        tStream.seek(0);
        for (int i = 0; i < sFileAsList.size(); ++i)
        {
            if (i == ( sFileAsList.size() - 1) )
                fTextFile.write(sFileAsList[i].toUtf8()) ;// + "\n");
            else
                fTextFile.write(sFileAsList[i].toUtf8() + "\n");
        }

        fTextFile.close();
        }

       return ;

}
