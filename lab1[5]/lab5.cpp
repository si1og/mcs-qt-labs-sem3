#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class HelloWidget : public QWidget {
public:
    HelloWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setGeometry(100, 100, 400, 200);
        setWindowTitle("Hello World App");

        QLabel *label = new QLabel("Hello World", this);
        label->setGeometry(150, 50, 100, 30);

        QPushButton *button = new QPushButton("Выход", this);
        button->setGeometry(150, 100, 100, 30);

        connect(button, &QPushButton::clicked, qApp, &QApplication::quit);
    }
};


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    HelloWidget window;
    window.show();

    return app.exec();
}
