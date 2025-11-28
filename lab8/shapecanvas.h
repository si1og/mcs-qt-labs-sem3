#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <vector>
#include <memory>
#include "shapes.h"

class ShapeCanvas : public QWidget {
    Q_OBJECT
public:
    explicit ShapeCanvas(QWidget* parent = nullptr);

    void addShape(Shape::Type type);
    void removeActiveShape();
    bool hasActiveShape() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void activeShapeChanged(Shape* shape);

private:
    std::vector<std::unique_ptr<Shape>> m_shapes;
    Shape* m_activeShape = nullptr;
    QPoint m_lastMousePos;
    bool m_dragging = false;

    Shape* shapeAt(const QPoint& pos);
    void bringToFront(Shape* shape);
};
