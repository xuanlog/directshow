#include "direct.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    direct w;
    w.show();
    return a.exec();
}
