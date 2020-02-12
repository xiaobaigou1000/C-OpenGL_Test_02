#include"MyGLWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyGLWindow w;
    w.show();
    return a.exec();
}
