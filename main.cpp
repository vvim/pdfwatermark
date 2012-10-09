#include <QtGui/QApplication>
#include "waterboard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WaterBoard w;
    w.show();
    
    return a.exec();
}
