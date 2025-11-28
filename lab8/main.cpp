#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("PhoneBook");
    app.setOrganizationName("Lab4");

    MainWindow window;
    window.show();

    return app.exec();
}
