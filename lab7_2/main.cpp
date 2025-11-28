#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "shapescollection.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ShapesCollection window;
    window.show();

    return app.exec();
}
