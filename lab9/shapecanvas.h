#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QMap>
#include "shapes.h"
#include "shapesdatabase.h"

class Shape;

class ShapeCanvas : public QGraphicsView {
    Q_OBJECT

public:
    explicit ShapeCanvas(ShapesDatabase* db, QWidget* parent = nullptr);

    void addShape(::Shape::Type type, int width, int height, int sides = 6);
    void removeShape(int id);
    void setShapeVisible(int id, bool visible);

    void addConnection(int fromId, int toId);
    void removeConnection(int fromId, int toId);

    ::Shape* getActiveShape() const { return m_activeShape; }
    void setActiveShape(::Shape* shape) { m_activeShape = shape; }
    ::Shape* getShapeById(int id) const;

    void loadFromDatabase();
    void selectShapeForConnection(int id);
    void clearConnectionSelection();

    bool generateGraphByDegreeSequence(const QVector<int>& degrees, QString* errorMsg = nullptr);

signals:
    void activeShapeChanged(::Shape* shape);
    void shapePositionChanged(int id, int x, int y);
    void connectionRequested(int fromId, int toId);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    ::Shape* createShapeFromInfo(const ShapesDatabase::ShapeInfo& info);
    ::Shape* shapeAt(const QPoint& pos);
    void bringToFront(::Shape* shape);

    bool isGraphicalSequence(QVector<int> degrees);
    QVector<QPair<int,int>> buildGraphHavelHakimi(QVector<int> degrees);

    QGraphicsScene* m_scene;
    ShapesDatabase* m_db;

    QMap<int, ::Shape*> m_shapes;
    QMap<QPair<int, int>, ConnectionLine*> m_connectionLines;

    ::Shape* m_activeShape = nullptr;
    ::Shape* m_connectionStart = nullptr;
    bool m_dragging = false;
};
