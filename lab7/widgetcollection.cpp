#include "widgetcollection.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QScrollBar>
#include <QSpinBox>
#include <QCheckBox>
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
    QCheckBox* checkbox = new QCheckBox(this);

    widgetCounter++;

    QHBoxLayout* widgetRow = new QHBoxLayout();

    widgetRow->addWidget(checkbox);

    // Создание виджета в зависимости от типа
    // %1 - это аргумент, который передается в функцию QString::arg()
    if (type == "QLabel") {
        ChainableLabel* label = new ChainableLabel(
                    QString("Label #%1: 0").arg(widgetCounter), this
                );
        label->setStyleSheet("QLabel { border: 1px solid gray; padding: 5px; }");
        newWidget = label;
        widgetRow->addWidget(label);

    } else if (type == "QSlider") {
        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, 100);
        slider->setValue(0);
        slider->setObjectName(QString("Slider #%1").arg(widgetCounter));
        newWidget = slider;
        widgetRow->addWidget(slider);

    } else if (type == "QScrollBar") {
        QScrollBar* scrollbar = new QScrollBar(Qt::Horizontal, this);
        scrollbar->setRange(0, 100);
        scrollbar->setValue(0);
        scrollbar->setObjectName(QString("ScrollBar #%1").arg(widgetCounter));
        newWidget = scrollbar;
        widgetRow->addWidget(scrollbar);

    } else if (type == "QSpinBox") {
        QSpinBox* spinbox = new QSpinBox(this);
        spinbox->setRange(0, 100);
        spinbox->setValue(0);
        spinbox->setObjectName(QString("SpinBox #%1").arg(widgetCounter));
        newWidget = spinbox;
        widgetRow->addWidget(spinbox);
    }

    if (newWidget) {
        widgets.push_back(newWidget);
        checkboxes.push_back(checkbox);

        connect(checkbox, &QCheckBox::toggled, this, [this, checkbox](bool checked) {
            if (checked) {
                // Если пытаются поставить галочку, сразу убираем её
                checkbox->setChecked(false);
            } else {
                // Если отжимают галочку, удаляем все связи этого виджета
                // disconnectWidget(newWidget);
            }
        });

        widgetRow->setStretch(1, 1);
        widgetsLayout->addLayout(widgetRow);

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

    for (QCheckBox* cb : checkboxes) {
        cb->setChecked(false);
    }

    int connectionsCount = 0;

    std::vector<QWidget*> sources;    // Первичные источники
    std::vector<QWidget*> relays;     // Ретрансляторы
    std::vector<QWidget*> receivers;  // Только приёмники

    // Классифицируем виджеты
    classifyWidgets(sources, relays, receivers);

    qDebug() << "первичных источников:" << sources.size();
    qDebug() << "ретрансляторов:" << relays.size();
    qDebug() << "конечных получателей:" << receivers.size();

    if (sources.empty()) {
        qDebug() << "нет первичных источников (QSlider/QScrollBar/QSpinBox)";
        return;
    }

    connectChain(sources, relays, receivers, connectionsCount);
    updateCheckboxes();

    qDebug() << "=== создано соединений:" << connectionsCount << "===\n";
}

bool WidgetCollection::connectWidgets(QWidget* sender, QWidget* receiver, int& connectionsCount) {
    if (sender == receiver) {
        return false;
    }

    const char* signal = getSignalForWidget(sender);
    const char* slot = getSlotForWidget(receiver);

    if (!signal || !slot) {
        return false;
    }

    bool success = QObject::connect(
        sender, signal,
        receiver, slot,
        Qt::UniqueConnection
    );

    if (success) {
        connectionsCount++;
        qDebug() << "   " << sender->metaObject()->className()
                 << " → " << receiver->metaObject()->className();
    }

    return success;
}

bool WidgetCollection::isPrimarySource(QWidget* widget) {
    const char* className = widget->metaObject()->className();

    // Только виджеты, с которыми пользователь взаимодействует
    return (qstrcmp(className, "QSlider") == 0 ||
            qstrcmp(className, "QScrollBar") == 0 ||
            qstrcmp(className, "QSpinBox") == 0);
}

bool WidgetCollection::canRelay(QWidget* widget) {
    // Может передавать сигнал дальше
    const char* signal = getSignalForWidget(widget);
    return (signal != nullptr);
}

bool WidgetCollection::canReceive(QWidget* widget) {
    // Может принимать сигнал
    const char* slot = getSlotForWidget(widget);
    return (slot != nullptr);
}

void WidgetCollection::classifyWidgets(std::vector<QWidget*>& sources,
                                       std::vector<QWidget*>& relays,
                                       std::vector<QWidget*>& receivers) {
    for (QWidget* widget : widgets) {
        if (isPrimarySource(widget)) {
            // Первичный источник (QSlider, QScrollBar, QSpinBox)
            sources.push_back(widget);
        } else if (canRelay(widget)) {
            // Ретранслятор (ChainableLabel)
            relays.push_back(widget);
        } else if (canReceive(widget)) {
            // Только приёмник (обычный QLabel, если есть)
            receivers.push_back(widget);
        }
    }
}

void WidgetCollection::connectChain(const std::vector<QWidget*>& sources,
                                    const std::vector<QWidget*>& relays,
                                    const std::vector<QWidget*>& receivers,
                                    int& connectionsCount) {

    if (sources.empty()) return;

    if (sources.size() > 1) {
        qDebug() << "\nвеерное соединение источников между собой:";
        for (size_t i = 0; i < sources.size(); i++) {
            for (size_t j = 0; j < sources.size(); j++) {
                if (i != j) {
                    connectWidgets(sources[i], sources[j], connectionsCount);
                }
            }
        }
    }

    std::vector<QWidget*> chain;

    chain.insert(chain.end(), relays.begin(), relays.end());
    chain.insert(chain.end(), receivers.begin(), receivers.end());

    connectWidgets(sources[0], chain[0], connectionsCount);

    qDebug() << "\последовательное соединение ретрансляторов:";
    for (size_t i = 0; i < chain.size() - 1; i++) {
        connectWidgets(chain[i], chain[i + 1], connectionsCount);
    }
}

const char* WidgetCollection::getSignalForWidget(QWidget* widget) {
    const char* className = widget->metaObject()->className();

    if (qstrcmp(className, "ChainableLabel") == 0) {
            return SIGNAL(valueChanged(int));
        }

    if (qstrcmp(className, "QSlider") == 0 ||
        qstrcmp(className, "QScrollBar") == 0 ||
        qstrcmp(className, "QSpinBox") == 0) {
        return SIGNAL(valueChanged(int));
    }

    return nullptr;
}

const char* WidgetCollection::getSlotForWidget(QWidget* widget) {
    const char* className = widget->metaObject()->className();

    if (qstrcmp(className, "ChainableLabel") == 0) {
            return SLOT(setNumAndEmit(int));
        }

    if (qstrcmp(className, "QSlider") == 0 ||
        qstrcmp(className, "QScrollBar") == 0 ||
        qstrcmp(className, "QSpinBox") == 0) {
        return SLOT(setValue(int));
    }

    return nullptr;
}

void WidgetCollection::updateCheckboxes() {
    for (size_t i = 0; i < widgets.size(); i++) {
        QWidget* widget = widgets[i];
        bool hasConnections = false;

        const char* signal = getSignalForWidget(widget);
        const char* slot = getSlotForWidget(widget);

        if (signal || slot) {
            hasConnections = true;
        }

        if (i < checkboxes.size()) {
            checkboxes[i]->setChecked(hasConnections);
        }
    }
}

void WidgetCollection::disconnectWidget(QWidget* widget) {
    qDebug() << "\n=== отключение виджета ===" << widget->metaObject()->className();

    const char* signal = getSignalForWidget(widget);
    const char* slot = getSlotForWidget(widget);

    // Отключаем все исходящие сигналы от этого виджета
    if (signal) {
        for (QWidget* other : widgets) {
            if (other != widget) {
                const char* otherSlot = getSlotForWidget(other);
                if (otherSlot) {
                    QObject::disconnect(widget, signal, other, otherSlot);
                    qDebug() << "  отключен:" << widget->metaObject()->className()
                             << "->" << other->metaObject()->className();
                }
            }
        }
    }

    // Отключаем все входящие сигналы к этому виджету
    if (slot) {
        for (QWidget* other : widgets) {
            if (other != widget) {
                const char* otherSignal = getSignalForWidget(other);
                if (otherSignal) {
                    QObject::disconnect(other, otherSignal, widget, slot);
                    qDebug() << "  отключен:" << other->metaObject()->className()
                             << "->" << widget->metaObject()->className();
                }
            }
        }
    }

    qDebug() << "=========================\n";
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
