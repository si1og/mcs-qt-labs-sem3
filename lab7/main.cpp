#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "widgetcollection.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WidgetCollection window;
    window.show();

    return app.exec();
}
