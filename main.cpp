#include "mainwindow.hpp"
#include "messagehandler.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    qInstallMessageHandler(&MessageHandler::handle);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
