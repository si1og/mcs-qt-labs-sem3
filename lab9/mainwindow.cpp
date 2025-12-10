#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>

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
    
    m_typeDelegate = new ShapeTypeDelegate(this);
    m_countDelegate = new ShapeCountDelegate(this);
    m_tableView->setItemDelegateForColumn(1, m_typeDelegate);   // Тип
    m_tableView->setItemDelegateForColumn(12, m_countDelegate); // Количество типа
    
    m_tableView->setColumnWidth(0, 40);  // ID
    m_tableView->setColumnWidth(1, 100); // Тип
    m_tableView->setColumnWidth(2, 40);  // X
    m_tableView->setColumnWidth(3, 40);  // Y
    m_tableView->setColumnWidth(4, 60);  // Ширина
    m_tableView->setColumnWidth(5, 60);  // Высота
    m_tableView->setColumnWidth(6, 30);  // R
    m_tableView->setColumnWidth(7, 30);  // G
    m_tableView->setColumnWidth(8, 30);  // B
    m_tableView->setColumnWidth(9, 60);  // Стороны
    m_tableView->setColumnWidth(10, 70); // Видимость
    m_tableView->setColumnWidth(11, 70); // Связи
    m_tableView->setColumnWidth(12, 80); // Кол-во типа
    
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
    
    setWindowTitle("Lab5 - Shapes Collection с БД");
    resize(1200, 700);
    
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddShape);
    connect(removeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveShape);
    connect(toggleVisBtn, &QPushButton::clicked, this, &MainWindow::onToggleVisibility);
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
    
    m_canvas->addShape(type, width, height);
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
    // Получаем выбранную строку в таблице
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
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
    
    // Удаляем первую найденную связь
    m_canvas->removeConnection(active->id(), connected.first());
}

void MainWindow::onActiveShapeChanged(::Shape* shape) {
    updateStatusBar(shape);
    
    // Сбрасываем выбор связи если выбрали другую фигуру
    if (m_connectionFirstId >= 0 && shape && shape->id() != m_connectionFirstId) {
        // Можно создать связь
    }
}

void MainWindow::onTableSelectionChanged() {
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    int row = selected.first().row();
    int id = m_db->getModel()->data(m_db->getModel()->index(row, 0)).toInt();
    
    // Активируем фигуру на canvas
    ::Shape* shape = m_canvas->getShapeById(id);
    if (shape && shape->isShapeVisible()) {
        ::Shape* current = m_canvas->getActiveShape();
        if (current) current->setActive(false);
        shape->setActive(true);
        updateStatusBar(shape);
    }
}

void MainWindow::onFilterChanged() {
    QList<ShapesDatabase::ShapeInfo> shapes = m_db->getAllShapes();
    
    // Скрытые ID
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
        
        // Проверяем фильтр по типу
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
        
        // Проверяем фильтр по ID
        if (hiddenIds.contains(info.id)) {
            visible = false;
        }
        
        m_canvas->setShapeVisible(info.id, visible);
    }
}

void MainWindow::onConnectionRequested(int fromId, int toId) {
    m_canvas->addConnection(fromId, toId);
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
    m_tableView->resizeRowsToContents();
}
