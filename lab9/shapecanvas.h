#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QMap>
#include "shapes.h"
#include "shapesdatabase.h"

// Forward declaration
class Shape;

class ShapeCanvas : public QGraphicsView {
    Q_OBJECT
    
public:
    explicit ShapeCanvas(ShapesDatabase* db, QWidget* parent = nullptr);
    
    void addShape(::Shape::Type type, int width, int height);
    void removeShape(int id);
    void setShapeVisible(int id, bool visible);
    
    void addConnection(int fromId, int toId);
    void removeConnection(int fromId, int toId);
    
    ::Shape* getActiveShape() const { return m_activeShape; }
    ::Shape* getShapeById(int id) const;
    
    void loadFromDatabase();
    void selectShapeForConnection(int id);
    void clearConnectionSelection();
    
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
    
    QGraphicsScene* m_scene;
    ShapesDatabase* m_db;
    
    QMap<int, ::Shape*> m_shapes;
    QMap<QPair<int,int>, ConnectionLine*> m_connectionLines;
    
    ::Shape* m_activeShape = nullptr;
    ::Shape* m_connectionStart = nullptr;
    bool m_dragging = false;
};
