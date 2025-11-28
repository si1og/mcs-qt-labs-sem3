#include "shapescollection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

ShapesCollection::ShapesCollection(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void ShapesCollection::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* controlLayout = new QHBoxLayout();

    m_typeSelector = new QComboBox(this);
    m_typeSelector->addItem("Прямоугольник", Shape::Rectangle);
    m_typeSelector->addItem("Треугольник", Shape::Triangle);
    m_typeSelector->addItem("Эллипс", Shape::Ellipse);

    QPushButton* addButton = new QPushButton("Добавить фигуру", this);
    QPushButton* removeButton = new QPushButton("Удалить активную", this);

    controlLayout->addWidget(m_typeSelector);
    controlLayout->addWidget(addButton);
    controlLayout->addWidget(removeButton);
    controlLayout->addStretch();

    mainLayout->addLayout(controlLayout);

    m_canvas = new ShapeCanvas(this);
    mainLayout->addWidget(m_canvas, 1);

    m_statusBar = new QStatusBar(this);

    m_coordsLabel = new QLabel("Координаты: —");
    m_colorLabel = new QLabel("Цвет: —");
    m_sizeLabel = new QLabel("Размер: —");

    m_statusBar->addWidget(m_coordsLabel, 1);
    m_statusBar->addWidget(m_colorLabel, 1);
    m_statusBar->addWidget(m_sizeLabel, 1);

    mainLayout->addWidget(m_statusBar);

    connect(addButton, &QPushButton::clicked, this, [this]() {
        Shape::Type type = static_cast<Shape::Type>(
            m_typeSelector->currentData().toInt()
        );
        m_canvas->addShape(type);
    });

    connect(removeButton, &QPushButton::clicked, this, [this]() {
        m_canvas->removeActiveShape();
    });

    connect(m_canvas, &ShapeCanvas::activeShapeChanged,
            this, &ShapesCollection::updateStatusBar);

    setWindowTitle("Lab3 - Shapes Collection (7.2 GraphicsView)");
    resize(600, 500);
}

void ShapesCollection::updateStatusBar(Shape* shape) {
    if (!shape) {
        m_coordsLabel->setText("Координаты: —");
        m_colorLabel->setText("Цвет: —");
        m_sizeLabel->setText("Размер: —");
        return;
    }

    QPointF pos = shape->pos();
    QRect rect = shape->rect();
    QColor color = shape->color();

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
