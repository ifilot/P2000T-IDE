#include "mainwindow.h"
#include "config.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName(PROGRAM_ORGANIZATION);
    QCoreApplication::setOrganizationDomain(PROGRAM_DOMAIN);
    QCoreApplication::setApplicationName(PROGRAM_NAME);

    MainWindow w;
    w.show();
    return a.exec();
}
