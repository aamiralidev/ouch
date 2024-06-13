#include "ouch.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //app.setQuitOnLastWindowClosed(false);
    Ouch w;
    w.show();
    return app.exec();
}
