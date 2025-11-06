#include "widgetcollection.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QScrollBar>
#include <QSpinBox>
#include <QDebug>
#include <QMetaObject>
#include <QMetaMethod>

WidgetCollection::WidgetCollection(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void WidgetCollection::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // панель управления
    QHBoxLayout* controlLayout = new QHBoxLayout();

    typeSelector = new QComboBox(this);
    typeSelector->addItem("QLabel");
    typeSelector->addItem("QSlider");
    typeSelector->addItem("QScrollBar");
    typeSelector->addItem("QSpinBox");

    QPushButton* addButton = new QPushButton("Добавить виджет", this);
    QPushButton* connectButton = new QPushButton("Соединить все", this);
    QPushButton* debugButton = new QPushButton("Проверить соединения", this);

    controlLayout->addWidget(typeSelector);
    controlLayout->addWidget(addButton);
    controlLayout->addWidget(connectButton);
    controlLayout->addWidget(debugButton);

    mainLayout->addLayout(controlLayout);


    // виджеты
    widgetsLayout = new QVBoxLayout();
    mainLayout->addLayout(widgetsLayout);
    mainLayout->addStretch();


    connect(addButton, &QPushButton::clicked, this, &WidgetCollection::addWidget);
    connect(connectButton, &QPushButton::clicked, this, &WidgetCollection::connectAll);
    connect(debugButton, &QPushButton::clicked, this, &WidgetCollection::debugConnections);

    setWindowTitle("Lab6 - widgets collection");
    resize(600, 400);
}

void WidgetCollection::addWidget() {
    QString type = typeSelector->currentText();
    QWidget* newWidget = nullptr;

    widgetCounter++;

    // Создание виджета в зависимости от типа
    // %1 - это аргумент, который передается в функцию QString::arg()
    if (type == "QLabel") {
        QLabel* label = new QLabel(QString("Label #%1: 0").arg(widgetCounter), this);
        label->setStyleSheet("QLabel { border: 1px solid gray; padding: 5px; }");
        newWidget = label;

    } else if (type == "QSlider") {
        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, 100);
        slider->setValue(0);
        slider->setObjectName(QString("Slider #%1").arg(widgetCounter));
        newWidget = slider;

    } else if (type == "QScrollBar") {
        QScrollBar* scrollbar = new QScrollBar(Qt::Horizontal, this);
        scrollbar->setRange(0, 100);
        scrollbar->setValue(0);
        scrollbar->setObjectName(QString("ScrollBar #%1").arg(widgetCounter));
        newWidget = scrollbar;

    } else if (type == "QSpinBox") {
        QSpinBox* spinbox = new QSpinBox(this);
        spinbox->setRange(0, 100);
        spinbox->setValue(0);
        spinbox->setObjectName(QString("SpinBox #%1").arg(widgetCounter));
        newWidget = spinbox;
    }

    if (newWidget) {
        widgets.push_back(newWidget);
        widgetsLayout->addWidget(newWidget);

        qDebug() << "Добавлен виджет:" << newWidget->metaObject()->className()
                 << "Всего виджетов:" << widgets.size();
    }
}

void WidgetCollection::connectAll() {
    qDebug() << "\n=== соединение виджетов ===";
    qDebug() << "всего виджетов в коллекции:" << widgets.size();

    if (widgets.size() < 2) {
        qDebug() << "недостаточно виджетов для соединения (min 2)";
        return;
    }

    int connectionsCount = 0;

    for (size_t i = 0; i < widgets.size(); i++) {
        for (size_t j = 0; j < widgets.size(); j++) {
            if (i == j) continue;  // не соединяем сам с собой

            QWidget* sender = widgets[i];
            QWidget* receiver = widgets[j];

            // QMetaObject::className() returns the class name as a string at run-time,
            // without requiring native run-time type information (RTTI)
            // support through the C++ compiler.

            const char* senderClass = sender->metaObject()->className();
            const char* receiverClass = receiver->metaObject()->className();

            // Определяем сигнал отправителя и слот получателя
            const char* signal = getSignalForWidget(sender); // сигнал отправителя (как считываем)
            const char* slot = getSlotForWidget(receiver); // слот получателя (как вставляем)

            if (signal && slot) {
                bool success = QObject::connect(
                    sender, signal,
                    receiver, slot,
                    Qt::UniqueConnection  // предотвращает дублирование
                );

                if (success) {
                    connectionsCount++;
                    qDebug() << "   создано соединение:" << senderClass << "-->" << receiverClass;
                }
            }
        }
    }

    qDebug() << "=== создано соединений:" << connectionsCount << "===\n";
}

const char* WidgetCollection::getSignalForWidget(QWidget* widget) {
    const char* className = widget->metaObject()->className();

    if (qstrcmp(className, "QLabel") == 0) {
        return nullptr;  // QLabel не имеет полезных сигналов

    } else if (qstrcmp(className, "QSlider") || qstrcmp(className, "QScrollBar") == 0 || qstrcmp(className, "QSpinBox") == 0) {
        return SIGNAL(valueChanged(int));
    }

    return nullptr;
}

const char* WidgetCollection::getSlotForWidget(QWidget* widget) {
    const char* className = widget->metaObject()->className();

    if (qstrcmp(className, "QLabel") == 0) {
        return SLOT(setNum(int));

    } else if (qstrcmp(className, "QSlider") || qstrcmp(className, "QScrollBar") == 0 || qstrcmp(className, "QSpinBox") == 0) {
        return SLOT(setValue(int));
    }


    return nullptr;
}

void WidgetCollection::debugConnections() {
    qDebug() << "\n=== ПРОВЕРКА СОЕДИНЕНИЙ ===";
    qDebug() << "всего виджетов:" << widgets.size();

    if (widgets.size() < 2) {
        qDebug() << "недостаточно виджетов для соединений\n";
        return;
    }

    int expectedConnections = widgets.size() * (widgets.size() - 1);
    int actualConnections = 0;

    qDebug() << "\nсоединения между виджетами:";

    for (size_t i = 0; i < widgets.size(); i++) {
        QWidget* sender = widgets[i];  // Уже QWidget*, не нужно приведение
        const char* senderClass = sender->metaObject()->className();
        const char* signal = getSignalForWidget(sender);

        if (!signal) continue;

        for (size_t j = 0; j < widgets.size(); j++) {
            if (i == j) continue;

            QWidget* receiver = widgets[j];
            const char* receiverClass = receiver->metaObject()->className();
            const char* slot = getSlotForWidget(receiver);

            if (slot) {
                qDebug() << "S: " << senderClass << "#" << i+1
                         << "-->" << receiverClass << "#" << j+1;
                actualConnections++;
            }
        }
    }

    if (actualConnections == expectedConnections) {
        qDebug() << "все виджеты соединены корректно";
    } else if (actualConnections > expectedConnections) {
        qDebug() << "обнаружены дубликаты!";
    }

    qDebug() << "\n--- вроверка висячих виджетов ---";
    int orphans = 0;
    for (QWidget* widget : widgets) {
        if (!widget->parent()) {
            qDebug() << widget->metaObject()->className() << "без родителя";
            orphans++;
        }
    }

    if (orphans == 0) {
        qDebug() << "висячих виджетов не найдено";
    }

    qDebug() << "=========================\n";
}
