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
#include <algorithm>

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
    QPushButton* printButton = new QPushButton("Напечатать связи", this);
    QPushButton* debugButton = new QPushButton("Отладка", this);

    controlLayout->addWidget(typeSelector);
    controlLayout->addWidget(addButton);
    controlLayout->addWidget(connectButton);
    controlLayout->addWidget(printButton);
    controlLayout->addWidget(debugButton);

    mainLayout->addLayout(controlLayout);

    // виджеты
    widgetsLayout = new QVBoxLayout();
    mainLayout->addLayout(widgetsLayout);
    mainLayout->addStretch();

    connect(addButton, &QPushButton::clicked, this, &WidgetCollection::addWidget);
    connect(connectButton, &QPushButton::clicked, this, &WidgetCollection::connectAll);
    connect(printButton, &QPushButton::clicked, this, &WidgetCollection::printConnections);
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

    if (type == "QLabel") {
        ChainableLabel* label = new ChainableLabel(
            QString("Label #%1: 0").arg(widgetCounter), this
        );
        label->setStyleSheet("QLabel { border: 1px solid gray; padding: 5px; }");
        label->setProperty("widgetName", QString("QLabel%1").arg(widgetCounter));
        newWidget = label;
        widgetRow->addWidget(label);

    } else if (type == "QSlider") {
        QSlider* slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, 100);
        slider->setValue(0);
        slider->setProperty("widgetName", QString("QSlider%1").arg(widgetCounter));
        newWidget = slider;
        widgetRow->addWidget(slider);

    } else if (type == "QScrollBar") {
        QScrollBar* scrollbar = new QScrollBar(Qt::Horizontal, this);
        scrollbar->setRange(0, 100);
        scrollbar->setValue(0);
        scrollbar->setProperty("widgetName", QString("QScrollBar%1").arg(widgetCounter));
        newWidget = scrollbar;
        widgetRow->addWidget(scrollbar);

    } else if (type == "QSpinBox") {
        QSpinBox* spinbox = new QSpinBox(this);
        spinbox->setRange(0, 100);
        spinbox->setValue(0);
        spinbox->setProperty("widgetName", QString("QSpinBox%1").arg(widgetCounter));
        newWidget = spinbox;
        widgetRow->addWidget(spinbox);
    }

    if (newWidget) {
        widgets.push_back(newWidget);
        checkboxes.push_back(checkbox);

        connect(checkbox, &QCheckBox::toggled, this, &WidgetCollection::onCheckboxChanged);

        widgetRow->setStretch(1, 1);
        widgetsLayout->addLayout(widgetRow);

        qDebug() << "Добавлен виджет:" << newWidget->property("widgetName").toString()
                 << "Всего виджетов:" << widgets.size();
    }
}

std::vector<QWidget*> WidgetCollection::getCheckedWidgets() {
    std::vector<QWidget*> checkedWidgets;

    for (size_t i = 0; i < widgets.size() && i < checkboxes.size(); i++) {
        if (checkboxes[i]->isChecked()) {
            checkedWidgets.push_back(widgets[i]);
        }
    }

    return checkedWidgets;
}

void WidgetCollection::disconnectAllWidgets() {
    for (QWidget* widget : widgets) {
        const char* signal = getSignalForWidget(widget);

        if (signal) {
            for (QWidget* other : widgets) {
                if (other != widget) {
                    const char* otherSlot = getSlotForWidget(other);
                    if (otherSlot) {
                        QObject::disconnect(widget, signal, other, otherSlot);
                    }
                }
            }
        }
    }

    // Очищаем список активных связей
    activeConnections.clear();
}

void WidgetCollection::onCheckboxChanged() {
    disconnectAllWidgets();
    std::vector<QWidget*> checkedWidgets = getCheckedWidgets();

    if (checkedWidgets.size() >= 2) {
        connectWidgetsList(checkedWidgets);
    }
}

void WidgetCollection::connectWidgetsList(const std::vector<QWidget*>& widgetList) {
    if (widgetList.size() < 2) {
        return;
    }

    int connectionsCount = 0;

    std::vector<QWidget*> sources;
    std::vector<QWidget*> relays;
    std::vector<QWidget*> receivers;

    classifyWidgets(widgetList, sources, relays, receivers);

    if (sources.empty()) {
        qDebug() << "нет первичных источников (QSlider/QScrollBar/QSpinBox)";
        return;
    }

    connectChain(sources, relays, receivers, connectionsCount);

    qDebug() << "создано соединений:" << connectionsCount;
}

void WidgetCollection::connectAll() {
    for (QCheckBox* cb : checkboxes) {
        cb->blockSignals(true);
        cb->setChecked(true);
        cb->blockSignals(false);
    }

    connectWidgetsList(widgets);
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
        activeConnections.push_back({sender, receiver});

        QString senderName = sender->property("widgetName").toString();
        QString receiverName = receiver->property("widgetName").toString();
        qDebug() << "  " << senderName << "->" << receiverName;
    }

    return success;
}

bool WidgetCollection::isPrimarySource(QWidget* widget) {
    const char* className = widget->metaObject()->className();

    return (qstrcmp(className, "QSlider") == 0 ||
            qstrcmp(className, "QScrollBar") == 0 ||
            qstrcmp(className, "QSpinBox") == 0);
}

bool WidgetCollection::canRelay(QWidget* widget) {
    const char* signal = getSignalForWidget(widget);
    return (signal != nullptr);
}

bool WidgetCollection::canReceive(QWidget* widget) {
    const char* slot = getSlotForWidget(widget);
    return (slot != nullptr);
}

void WidgetCollection::classifyWidgets(const std::vector<QWidget*>& widgetList,
                                       std::vector<QWidget*>& sources,
                                       std::vector<QWidget*>& relays,
                                       std::vector<QWidget*>& receivers) {
    sources.clear();
    relays.clear();
    receivers.clear();

    for (QWidget* widget : widgetList) {
        if (isPrimarySource(widget)) {
            sources.push_back(widget);
        } else if (canRelay(widget)) {
            relays.push_back(widget);
        } else if (canReceive(widget)) {
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

    if (chain.empty()) {
        return;
    }

    connectWidgets(sources[0], chain[0], connectionsCount);

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
    const char* signal = getSignalForWidget(widget);
    const char* slot = getSlotForWidget(widget);

    if (signal) {
        for (QWidget* other : widgets) {
            if (other != widget) {
                const char* otherSlot = getSlotForWidget(other);
                if (otherSlot) {
                    QObject::disconnect(widget, signal, other, otherSlot);
                }
            }
        }
    }

    if (slot) {
        for (QWidget* other : widgets) {
            if (other != widget) {
                const char* otherSignal = getSignalForWidget(other);
                if (otherSignal) {
                    QObject::disconnect(other, otherSignal, widget, slot);
                }
            }
        }
    }

    // Удаляем связи с этим виджетом из списка
    activeConnections.erase(
        std::remove_if(activeConnections.begin(), activeConnections.end(),
            [widget](const Connection& conn) {
                return conn.sender == widget || conn.receiver == widget;
            }),
        activeConnections.end()
    );
}

void WidgetCollection::printConnections() {
    qDebug() << "\n=== АКТИВНЫЕ СВЯЗИ ===";

    if (activeConnections.empty()) {
        qDebug() << "Нет активных связей";
        qDebug() << "======================\n";
        return;
    }

    for (const Connection& conn : activeConnections) {
        QString senderName = conn.sender->property("widgetName").toString();
        QString receiverName = conn.receiver->property("widgetName").toString();
        qDebug().noquote() << senderName << "->" << receiverName;
    }

    qDebug() << "======================\n";
}

void WidgetCollection::debugConnections() {
    qDebug() << "\n========== ОТЛАДКА СОЕДИНЕНИЙ ==========";
    qDebug() << "Всего виджетов:" << widgets.size();
    qDebug() << "Всего записанных соединений:" << activeConnections.size();

    qDebug() << "\n--- Проверка дубликатов ---";
    int duplicatesCount = 0;

    for (size_t i = 0; i < activeConnections.size(); i++) {
        for (size_t j = i + 1; j < activeConnections.size(); j++) {
            if (activeConnections[i].sender == activeConnections[j].sender &&
                activeConnections[i].receiver == activeConnections[j].receiver) {

                QString senderName = activeConnections[i].sender->property("widgetName").toString();
                QString receiverName = activeConnections[i].receiver->property("widgetName").toString();
                qDebug() << "  ДУБЛИКАТ:" << senderName << "->" << receiverName;
                duplicatesCount++;
            }
        }
    }

    if (duplicatesCount == 0) {
        qDebug() << "  дубликатов не найдено";
    } else {
        qDebug() << "  найдено дубликатов:" << duplicatesCount;
    }

    qDebug() << "\n--- проверка висячих соединений ---";
    int danglingCount = 0;

    for (const Connection& conn : activeConnections) {
        bool senderExists = std::find(widgets.begin(), widgets.end(), conn.sender) != widgets.end();
        bool receiverExists = std::find(widgets.begin(), widgets.end(), conn.receiver) != widgets.end();

        if (!senderExists || !receiverExists) {
            QString senderName = conn.sender->property("widgetName").toString();
            QString receiverName = conn.receiver->property("widgetName").toString();
            qDebug() << "  «висячее» соединение:" << senderName << "->" << receiverName;

            if (!senderExists) qDebug() << "    sender не найден в коллекции";
            if (!receiverExists) qDebug() << "    receiver не найден в коллекции";

            danglingCount++;
        }
    }

    if (danglingCount == 0) {
        qDebug() << "  висячих соединений не найдено";
    } else {
        qDebug() << "  найдено висячих соединений:" << danglingCount;
    }

    qDebug() << "\n--- проверка виджетов без родителя ---";
    int orphanCount = 0;

    for (QWidget* widget : widgets) {
        if (!widget->parent()) {
            QString name = widget->property("widgetName").toString();
            qDebug() << name << "не имеет родителя";
            orphanCount++;
        }
    }

    if (orphanCount == 0) {
        qDebug() << "  все виджеты имеют родителя";
    } else {
        qDebug() << "  виджетов без родителя:" << orphanCount;
    }

    qDebug() << "\n--- статистика по чекбоксам ---";
    std::vector<QWidget*> checked = getCheckedWidgets();
    qDebug() << "  выделено виджетов:" << checked.size();

    std::vector<QWidget*> connectedWidgets;
    for (const Connection& conn : activeConnections) {
        if (std::find(connectedWidgets.begin(), connectedWidgets.end(), conn.sender) == connectedWidgets.end()) {
            connectedWidgets.push_back(conn.sender);
        }
        if (std::find(connectedWidgets.begin(), connectedWidgets.end(), conn.receiver) == connectedWidgets.end()) {
            connectedWidgets.push_back(conn.receiver);
        }
    }
    qDebug() << "  виджетов в соединениях:" << connectedWidgets.size();

    qDebug() << "\n--- список соединений ---";
    if (activeConnections.empty()) {
        qDebug() << "  (пусто)";
    } else {
        for (const Connection& conn : activeConnections) {
            QString senderName = conn.sender->property("widgetName").toString();
            QString receiverName = conn.receiver->property("widgetName").toString();
            qDebug().noquote() << "  " << senderName << "->" << receiverName;
        }
    }

    qDebug() << "\n=========================================\n";
}
