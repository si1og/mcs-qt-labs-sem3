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

    connect(addButton, &QPushButton::clicked, this, [this]() {
        Shape::Type type = static_cast<Shape::Type>(
            m_typeSelector->currentData().toInt()
        );
        m_canvas->addShape(type);
    });

    connect(removeButton, &QPushButton::clicked, this, [this]() {
        m_canvas->removeActiveShape();
    });

    setWindowTitle("Lab3 - Shapes Collection (7.1)");
    resize(600, 500);
}
