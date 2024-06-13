#include "ouch.h"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <iostream>
#include <QScreen>
#include <QKeyEvent>
#include <QSystemTrayIcon>
#include <QMenu>

Ouch::Ouch(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTray();

    QList<QByteArray> list = QImageReader::supportedImageFormats();
    list.append(QMovie::supportedFormats());
    std::string str = "";
    for(QByteArray frmt: list){
        str += " *." + frmt.toStdString();
    }
    this->supported_formats = QString::fromStdString("Images (" + str + ")");

    this->settings = new QSettings(QSettings::IniFormat, QSettings::SystemScope, "Ouch", "Ouch");

    this->label = new QLabel(this);
    this->label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->label->setScaledContents(true);


    this->filename = this->settings->value("filename").toString();
    if(this->filename.isNull() || this->filename.isEmpty() || !(QFileInfo(this->filename).exists())){
        this->filename = QFileDialog::getOpenFileName(this, "Please Select a file", nullptr, this->supported_formats);
//        if(this->filename.isNull() || this->filename.isEmpty() || !(QFileInfo(this->filename).exists())){
//            QMessageBox::critical(this, "FileName Error", "Filename is invalid");
//        }
    }
    this->geo = this->settings->value("geo").toRect();
    if(this->geo.isNull() || this->geo.isEmpty()){
        QPixmap map{this->filename};
        QRect screen = QGuiApplication::primaryScreen()->geometry();
        this->geo = QRect(screen.width() - map.width(), screen.height() - map.height() - 50, map.width(), map.height());
    }
    this->setGeometry(this->geo);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->move_gif = new QMovie(this->filename);
    this->move_gif->setScaledSize(this->geo.size());
    this->label->setMovie(this->move_gif);
    this->move_gif->start();

}

void Ouch::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Control){
        this->control_key_pressed = true;
    }
    if(event->key() == Qt::Key_Alt){
        this->alt_key_pressed = true;
    }
    return QWidget::keyPressEvent(event);
}

void Ouch::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Control){
        this->control_key_pressed = false;
    }
    if(event->key() == Qt::Key_Alt){
        this->alt_key_pressed = false;
    }
    if(event->key() == Qt::Key_Delete && this->control_key_pressed){
        QApplication::quit();
    }
    return QWidget::keyPressEvent(event);
}

void Ouch::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::MouseButton::LeftButton){
        this->oldPosition = event->globalPos();
    }
}

void Ouch::mouseMoveEvent(QMouseEvent *event){
    if(!this->oldPosition.isNull()){
        QPoint delta = event->globalPos() - this->oldPosition;
        if(this->control_key_pressed){
            this->move(this->pos() + delta);
        }else if(this->alt_key_pressed){
            this->resizeMovie(delta);
        }
        this->oldPosition = event->globalPos();
    }
}

void Ouch::mouseDoubleClickEvent(QMouseEvent *event){
    if(this->control_key_pressed){
        this->reloadFile();
    }
    return QWidget::mouseDoubleClickEvent(event);
}

void Ouch::reloadFile(){
    QString fname = QFileDialog::getOpenFileName(this, "Please Select a file", nullptr, this->supported_formats);
    if(!(fname.isNull() || fname.isEmpty() || !(QFileInfo(fname).exists()))){
        QSize movie_size = this->label->movie()->scaledSize();
        this->filename = fname;
        delete this->move_gif;
        this->move_gif = new QMovie(this->filename);
        this->move_gif->setScaledSize(movie_size);
        this->label->setMovie(this->move_gif);
        this->move_gif->start();
    }
}

void Ouch::resizeMovie(QPoint delta){
    QSize movie_size = this->label->movie()->scaledSize();
    int height = movie_size.height() + movie_size.height() * (float(delta.y())/100);
    int width = movie_size.width() + movie_size.width() * (float(delta.y())/100);
    QRect screen = QGuiApplication::primaryScreen()->geometry();
    movie_size.setHeight(std::min(std::max(height, 50), screen.height()));
    movie_size.setWidth(std::min(std::max(width, 50), screen.width()));
    this->label->movie()->setScaledSize(movie_size);
    this->label->movie()->start();
    this->label->adjustSize();
    this->adjustSize();
}

void Ouch::setWindowTray(){
    if(!QSystemTrayIcon::isSystemTrayAvailable()){
        return;
    }

    connect(&quit_action, &QAction::triggered, this, []{QGuiApplication::exit();});
    connect(&select_action, &QAction::triggered, this, [this]{this->reloadFile();});

    menu.addAction(&quit_action);
    menu.addAction(&select_action);

    systemTray.setContextMenu(&menu);
    systemTray.setIcon(QIcon(":/images/icon.png"));
    systemTray.show();
}

void Ouch::closeEvent(QCloseEvent *event){
    this->settings->setValue("filename", this->filename);
    this->settings->setValue("geo", this->geo);
    return QWidget::closeEvent(event);
}

Ouch::~Ouch()
{
}

