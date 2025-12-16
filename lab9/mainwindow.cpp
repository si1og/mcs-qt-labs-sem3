#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QFileDialog>
#include <QFile>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    m_db = new ShapesDatabase(this);

    if (!m_db->initialize()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось инициализировать базу данных");
    }

    setupUI();
    setupConnections();

    m_canvas->loadFromDatabase();
    refreshTable();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QGroupBox* controlGroup = new QGroupBox("Управление фигурами");
    QHBoxLayout* controlLayout = new QHBoxLayout(controlGroup);

    controlLayout->addWidget(new QLabel("Тип:"));
    m_typeSelector = new QComboBox();
    m_typeSelector->addItem("Прямоугольник", ::Shape::Rectangle);
    m_typeSelector->addItem("Треугольник", ::Shape::Triangle);
    m_typeSelector->addItem("Эллипс", ::Shape::Ellipse);
    m_typeSelector->addItem("Многоугольник", ::Shape::Polygon);
    controlLayout->addWidget(m_typeSelector);

    controlLayout->addWidget(new QLabel("Ширина:"));
    m_widthSpin = new QSpinBox();
    m_widthSpin->setRange(30, 200);
    m_widthSpin->setValue(80);
    controlLayout->addWidget(m_widthSpin);

    controlLayout->addWidget(new QLabel("Высота:"));
    m_heightSpin = new QSpinBox();
    m_heightSpin->setRange(30, 200);
    m_heightSpin->setValue(80);
    controlLayout->addWidget(m_heightSpin);

    m_sidesLabel = new QLabel("Стороны:");
    m_sidesSpin = new QSpinBox();
    m_sidesSpin->setRange(3, 100);
    m_sidesSpin->setValue(6);
    controlLayout->addWidget(m_sidesLabel);
    controlLayout->addWidget(m_sidesSpin);

    m_sidesLabel->setVisible(false);
    m_sidesSpin->setVisible(false);

    QPushButton* addBtn = new QPushButton("Добавить");
    QPushButton* removeBtn = new QPushButton("Удалить");
    QPushButton* toggleVisBtn = new QPushButton("Скрыть/Показать");

    controlLayout->addWidget(addBtn);
    controlLayout->addWidget(removeBtn);
    controlLayout->addWidget(toggleVisBtn);
    controlLayout->addStretch();

    mainLayout->addWidget(controlGroup);

    QGroupBox* connectionGroup = new QGroupBox("Связи");
    QHBoxLayout* connectionLayout = new QHBoxLayout(connectionGroup);

    m_connectBtn = new QPushButton("Создать связь (выберите 2 фигуры)");
    m_disconnectBtn = new QPushButton("Удалить связь");

    connectionLayout->addWidget(m_connectBtn);
    connectionLayout->addWidget(m_disconnectBtn);
    connectionLayout->addStretch();

    mainLayout->addWidget(connectionGroup);

    QGroupBox* graphGenGroup = new QGroupBox("Генерация графа по степеням вершин");
    QHBoxLayout* graphGenLayout = new QHBoxLayout(graphGenGroup);

    graphGenLayout->addWidget(new QLabel("Степени:"));
    m_degreeSequenceEdit = new QLineEdit();
    m_degreeSequenceEdit->setPlaceholderText("например: 2,2,2,2 или 3,3,2,2,2");
    m_degreeSequenceEdit->setMinimumWidth(200);
    graphGenLayout->addWidget(m_degreeSequenceEdit);

    m_generateGraphBtn = new QPushButton("Сгенерировать граф");
    graphGenLayout->addWidget(m_generateGraphBtn);
    graphGenLayout->addStretch();

    mainLayout->addWidget(graphGenGroup);

    QGroupBox* ioGroup = new QGroupBox("База данных");
    QHBoxLayout* ioLayout = new QHBoxLayout(ioGroup);

    QPushButton* exportBtn = new QPushButton("Импортировать БД");
    QPushButton* importBtn = new QPushButton("Экпортировать БД");

    ioLayout->addWidget(exportBtn);
    ioLayout->addWidget(importBtn);
    ioLayout->addStretch();

    mainLayout->addWidget(ioGroup);

    QSplitter* splitter = new QSplitter(Qt::Horizontal);

    m_canvas = new ShapeCanvas(m_db);
    splitter->addWidget(m_canvas);

    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    QGroupBox* filterGroup = new QGroupBox("Фильтры отображения");
    QGridLayout* filterLayout = new QGridLayout(filterGroup);

    m_filterRect = new QCheckBox("Прямоугольники");
    m_filterRect->setChecked(true);
    m_filterTriangle = new QCheckBox("Треугольники");
    m_filterTriangle->setChecked(true);
    m_filterEllipse = new QCheckBox("Эллипсы");
    m_filterEllipse->setChecked(true);
    m_filterPolygon = new QCheckBox("Многоугольники");
    m_filterPolygon->setChecked(true);

    filterLayout->addWidget(m_filterRect, 0, 0);
    filterLayout->addWidget(m_filterTriangle, 0, 1);
    filterLayout->addWidget(m_filterEllipse, 1, 0);
    filterLayout->addWidget(m_filterPolygon, 1, 1);

    filterLayout->addWidget(new QLabel("Скрыть ID:"), 2, 0);
    m_filterIdEdit = new QLineEdit();
    m_filterIdEdit->setPlaceholderText("например: 1,3,5");
    filterLayout->addWidget(m_filterIdEdit, 2, 1);

    rightLayout->addWidget(filterGroup);

    QGroupBox* tableGroup = new QGroupBox("База данных фигур");
    QVBoxLayout* tableLayout = new QVBoxLayout(tableGroup);

    m_tableView = new QTableView();
    m_tableView->setModel(m_db->getModel());
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setMinimumWidth(400);
    m_tableView->verticalHeader()->setDefaultSectionSize(30);
    m_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    m_typeDelegate = new ShapeTypeDelegate(this);
    m_countDelegate = new ShapeCountDelegate(this);
    m_tableView->setItemDelegateForColumn(1, m_typeDelegate);
    m_tableView->setItemDelegateForColumn(12, m_countDelegate);

    m_tableView->setColumnWidth(0, 40);
    m_tableView->setColumnWidth(1, 100);
    m_tableView->setColumnWidth(2, 40);
    m_tableView->setColumnWidth(3, 40);
    m_tableView->setColumnWidth(4, 60);
    m_tableView->setColumnWidth(5, 60);
    m_tableView->setColumnWidth(6, 30);
    m_tableView->setColumnWidth(7, 30);
    m_tableView->setColumnWidth(8, 30);
    m_tableView->setColumnWidth(9, 60);
    m_tableView->setColumnWidth(10, 70);
    m_tableView->setColumnWidth(11, 70);
    m_tableView->setColumnWidth(12, 80);

    tableLayout->addWidget(m_tableView);
    rightLayout->addWidget(tableGroup);

    splitter->addWidget(rightPanel);
    splitter->setSizes({600, 500});

    mainLayout->addWidget(splitter, 1);

    m_coordsLabel = new QLabel("Координаты: —");
    m_colorLabel = new QLabel("Цвет: —");
    m_sizeLabel = new QLabel("Размер: —");
    m_idLabel = new QLabel("ID: —");

    statusBar()->addWidget(m_idLabel, 1);
    statusBar()->addWidget(m_coordsLabel, 1);
    statusBar()->addWidget(m_colorLabel, 1);
    statusBar()->addWidget(m_sizeLabel, 1);

    setWindowTitle("Lab5");
    resize(1200, 700);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddShape);
    connect(removeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveShape);
    connect(toggleVisBtn, &QPushButton::clicked, this, &MainWindow::onToggleVisibility);
    connect(m_typeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onTypeChanged);
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::onExportDatabase);
    connect(importBtn, &QPushButton::clicked, this, &MainWindow::onImportDatabase);
    connect(m_generateGraphBtn, &QPushButton::clicked, this, &MainWindow::onGenerateGraph);
}

void MainWindow::setupConnections() {
    connect(m_canvas, &ShapeCanvas::activeShapeChanged,
            this, &MainWindow::onActiveShapeChanged);
    connect(m_canvas, &ShapeCanvas::connectionRequested,
            this, &MainWindow::onConnectionRequested);

    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onTableSelectionChanged);

    connect(m_filterRect, &QCheckBox::stateChanged, this, &MainWindow::onFilterChanged);
    connect(m_filterTriangle, &QCheckBox::stateChanged, this, &MainWindow::onFilterChanged);
    connect(m_filterEllipse, &QCheckBox::stateChanged, this, &MainWindow::onFilterChanged);
    connect(m_filterPolygon, &QCheckBox::stateChanged, this, &MainWindow::onFilterChanged);
    connect(m_filterIdEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterChanged);

    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onAddConnection);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MainWindow::onRemoveConnection);

    connect(m_db, &ShapesDatabase::dataChanged, this, &MainWindow::refreshTable);
}

void MainWindow::onAddShape() {
    ::Shape::Type type = static_cast<::Shape::Type>(m_typeSelector->currentData().toInt());
    int width = m_widthSpin->value();
    int height = m_heightSpin->value();
    int sides = (type == ::Shape::Polygon) ? m_sidesSpin->value() : 6;

    m_canvas->addShape(type, width, height, sides);
}

void MainWindow::onRemoveShape() {
    ::Shape* active = m_canvas->getActiveShape();
    if (active) {
        m_canvas->removeShape(active->id());
    } else {
        QMessageBox::information(this, "Удаление", "Выберите фигуру для удаления");
    }
}

void MainWindow::onToggleVisibility() {
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {

        Shape* activeShape = m_canvas->getActiveShape();
        if (activeShape) {
            int id = activeShape->id();
            bool visible = activeShape->isShapeVisible();
            m_canvas->setShapeVisible(id, !visible);
            return;
        }

        QMessageBox::information(this, "Видимость",
            "Выберите фигуру в таблице для изменения видимости");
        return;
    }

    int row = selected.first().row();
    int id = m_db->getModel()->data(m_db->getModel()->index(row, 0)).toInt();
    bool visible = m_db->getModel()->data(m_db->getModel()->index(row, 10)).toBool();

    m_canvas->setShapeVisible(id, !visible);
}

void MainWindow::onAddConnection() {
    ::Shape* active = m_canvas->getActiveShape();
    if (!active) {
        QMessageBox::information(this, "Связь",
            "Выберите первую фигуру на canvas");
        return;
    }

    if (m_connectionFirstId < 0) {
        m_connectionFirstId = active->id();
        m_connectBtn->setText(QString("Выбрана фигура %1. Выберите вторую").arg(m_connectionFirstId));
    } else {
        if (m_connectionFirstId != active->id()) {
            m_canvas->addConnection(m_connectionFirstId, active->id());
        }
        m_connectionFirstId = -1;
        m_connectBtn->setText("Создать связь (выберите 2 фигуры)");
    }
}

void MainWindow::onRemoveConnection() {
    ::Shape* active = m_canvas->getActiveShape();
    if (!active) {
        QMessageBox::information(this, "Связь",
            "Выберите фигуру для удаления её связей");
        return;
    }

    QList<int> connected = m_db->getConnectedShapes(active->id());
    if (connected.isEmpty()) {
        QMessageBox::information(this, "Связь",
            "У этой фигуры нет связей");
        return;
    }

    m_canvas->removeConnection(active->id(), connected.first());
}

void MainWindow::onActiveShapeChanged(::Shape* shape) {
    updateStatusBar(shape);
}

void MainWindow::onTableSelectionChanged() {
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    int row = selected.first().row();
    int id = m_db->getModel()->data(m_db->getModel()->index(row, 0)).toInt();

    ::Shape* current = m_canvas->getActiveShape();
    if (current) {
        current->setActive(false);
    }

    ::Shape* shape = m_canvas->getShapeById(id);
    if (shape && shape->isShapeVisible()) {
        shape->setActive(true);
        m_canvas->setActiveShape(shape);
        updateStatusBar(shape);
    } else {
        m_canvas->setActiveShape(nullptr);
        updateStatusBar(nullptr);
    }
}

void MainWindow::onFilterChanged() {
    QList<ShapesDatabase::ShapeInfo> shapes = m_db->getAllShapes();

    QSet<int> hiddenIds;
    QString idsText = m_filterIdEdit->text();
    if (!idsText.isEmpty()) {
        QStringList idList = idsText.split(',', Qt::SkipEmptyParts);
        for (const QString& idStr : idList) {
            bool ok;
            int id = idStr.trimmed().toInt(&ok);
            if (ok) hiddenIds.insert(id);
        }
    }

    for (const auto& info : shapes) {
        bool visible = true;

        switch (info.type) {
            case ::Shape::Rectangle:
                visible = m_filterRect->isChecked();
                break;
            case ::Shape::Triangle:
                visible = m_filterTriangle->isChecked();
                break;
            case ::Shape::Ellipse:
                visible = m_filterEllipse->isChecked();
                break;
            case ::Shape::Polygon:
                visible = m_filterPolygon->isChecked();
                break;
        }

        if (hiddenIds.contains(info.id)) {
            visible = false;
        }

        m_canvas->setShapeVisible(info.id, visible);
    }
}

void MainWindow::onConnectionRequested(int fromId, int toId) {
    m_canvas->addConnection(fromId, toId);
}

void MainWindow::onTypeChanged(int index) {
    Q_UNUSED(index);
    ::Shape::Type type = static_cast<::Shape::Type>(m_typeSelector->currentData().toInt());
    bool isPolygon = (type == ::Shape::Polygon);
    m_sidesLabel->setVisible(isPolygon);
    m_sidesSpin->setVisible(isPolygon);
}

void MainWindow::onExportDatabase() {
    QString fileName = QFileDialog::getSaveFileName(this,
        "Сохранить базу данных", "shapes_export.db", "SQLite базы данных (*.db)");

    if (fileName.isEmpty()) return;

    QString srcPath = m_db->getDatabasePath();

    if (QFile::exists(fileName)) {
        QFile::remove(fileName);
    }

    if (QFile::copy(srcPath, fileName)) {
        QMessageBox::information(this, "Экспорт", "База данных успешно сохранена");
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
    }
}

void MainWindow::onImportDatabase() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "Загрузить базу данных", "", "SQLite базы данных (*.db)");

    if (fileName.isEmpty()) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Подтверждение импорта",
        "Текущая база данных будет полностью заменена.\nПродолжить?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    if (m_db->importFromFile(fileName)) {
        m_canvas->loadFromDatabase();
        QMessageBox::information(this, "Импорт", "База данных успешно загружена");
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить базу данных");
    }
}

void MainWindow::updateStatusBar(::Shape* shape) {
    if (!shape) {
        m_idLabel->setText("ID: —");
        m_coordsLabel->setText("Координаты: —");
        m_colorLabel->setText("Цвет: —");
        m_sizeLabel->setText("Размер: —");
        return;
    }

    QPointF pos = shape->pos();
    QRect rect = shape->rect();
    QColor color = shape->color();

    m_idLabel->setText(QString("ID: %1").arg(shape->id()));
    m_coordsLabel->setText(QString("Координаты: (%1, %2)")
        .arg((int)pos.x())
        .arg((int)pos.y()));
    m_colorLabel->setText(QString("Цвет: RGB(%1, %2, %3)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue()));
    m_sizeLabel->setText(QString("Размер: %1 × %2")
        .arg(rect.width())
        .arg(rect.height()));
}

void MainWindow::refreshTable() {
    m_db->getModel()->select();
}

void MainWindow::onGenerateGraph() {
    QString input = m_degreeSequenceEdit->text().trimmed();

    if (input.isEmpty()) {
        QMessageBox::warning(this, "Генерация графа",
            "Введите последовательность степеней вершин.\n"
            "Например: 2,2,2,2 (цикл из 4 вершин) или 3,3,2,2,2 (дом)");
        return;
    }

    QStringList parts = input.split(',', Qt::SkipEmptyParts);
    QVector<int> degrees;

    for (const QString& part : parts) {
        bool ok;
        int degree = part.trimmed().toInt(&ok);
        if (!ok || degree < 0) {
            QMessageBox::warning(this, "Ошибка ввода",
                QString("Некорректное значение: '%1'\n"
                        "Введите неотрицательные целые числа через запятую.").arg(part.trimmed()));
            return;
        }
        degrees.append(degree);
    }

    if (degrees.isEmpty()) {
        QMessageBox::warning(this, "Генерация графа", "Не удалось распознать степени вершин.");
        return;
    }

    QString errorMsg;
    if (m_canvas->generateGraphByDegreeSequence(degrees, &errorMsg)) {
        QMessageBox::information(this, "Успешная генерация графа",
            QString("Граф с %1 вершинами успешно сгенерирован.").arg(degrees.size()));
        refreshTable();
    } else {
        QMessageBox::critical(this, "Граф не существует", errorMsg);
    }
}
