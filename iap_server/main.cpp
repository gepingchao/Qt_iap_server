#include <QApplication>
#include "iap_server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    iap_server w;
    w.show();
    
    return a.exec();
}
