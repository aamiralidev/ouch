#ifndef OUCH_H
#define OUCH_H

#include <QLabel>
#include <QMainWindow>
#include <QSettings>
#include <QMovie>
#include <QSystemTrayIcon>
#include <QMenu>

class Ouch : public QWidget
{
    Q_OBJECT
private:
    QSettings* settings;
    QLabel* label;
    QString supported_formats;
    QString filename;
    QRect geo;
    QMovie* move_gif;
    bool control_key_pressed{false};
    bool alt_key_pressed{false};
    QPoint oldPosition;
    QSystemTrayIcon systemTray{this};
    QMenu menu{this};
    QAction quit_action{"&Quit", this};
    QAction select_action{"&Select", this};


public:
    Ouch(QWidget *parent = nullptr);
    ~Ouch();

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeMovie(QPoint delta);
    void reloadFile();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void setWindowTray();
};
#endif // OUCH_H
