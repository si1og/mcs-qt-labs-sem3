#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include "shapes.h"

class ShapeCanvas : public QGraphicsView {
    Q_OBJECT
public:
    explicit ShapeCanvas(QWidget* parent = nullptr);

    void addShape(::Shape::Type type);
    void removeActiveShape();
    bool hasActiveShape() const;
    ::Shape* getActiveShape() const { return m_activeShape; }

signals:
    void activeShapeChanged(::Shape* shape);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QGraphicsScene* m_scene;
    ::Shape* m_activeShape = nullptr;
    bool m_dragging = false;

    ::Shape* shapeAt(const QPoint& pos);
    void bringToFront(::Shape* shape);
};
