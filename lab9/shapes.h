#pragma once

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QRect>
#include <QColor>
#include <QPainter>
#include <QPoint>
#include <QPolygon>
#include <QVector>

class Shape;

class ConnectionLine : public QGraphicsLineItem {
public:
    ConnectionLine(Shape* from, Shape* to);
    
    Shape* fromShape() const { return m_from; }
    Shape* toShape() const { return m_to; }
    
    void updatePosition();
    
private:
    Shape* m_from;
    Shape* m_to;
};

class Shape : public QGraphicsItem {
public:
    enum Type { Rectangle = 0, Triangle = 1, Ellipse = 2, Polygon = 3 };
    
    Shape(int id, Type type, const QRect& rect, const QColor& color);
    virtual ~Shape();
    
    virtual void draw(QPainter& painter) const = 0;
    virtual bool contains(const QPointF& point) const;
    
    void setActive(bool active) { m_active = active; update(); }
    bool isActive() const { return m_active; }
    
    int id() const { return m_id; }
    QRect rect() const { return m_rect; }
    Type shapeType() const { return m_type; }
    QColor color() const { return m_color; }
    
    void setVisible(bool visible);
    bool isShapeVisible() const { return m_visible; }
    
    void addConnection(ConnectionLine* line);
    void removeConnection(ConnectionLine* line);
    QVector<ConnectionLine*> connections() const { return m_connections; }
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    static QString typeToString(Type type);
    static Type stringToType(const QString& str);
    
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    
    int m_id;
    Type m_type;
    QRect m_rect;
    QColor m_color;
    bool m_active = false;
    bool m_visible = true;
    QVector<ConnectionLine*> m_connections;
};

class RectangleShape : public Shape {
public:
    RectangleShape(int id, const QRect& rect, const QColor& color);
    void draw(QPainter& painter) const override;
    QPainterPath shape() const override;
};

class TriangleShape : public Shape {
public:
    TriangleShape(int id, const QRect& rect, const QColor& color);
    void draw(QPainter& painter) const override;
    bool contains(const QPointF& point) const override;
    QPainterPath shape() const override;
    
private:
    QPolygon getTrianglePolygon() const;
};

class EllipseShape : public Shape {
public:
    EllipseShape(int id, const QRect& rect, const QColor& color);
    void draw(QPainter& painter) const override;
    bool contains(const QPointF& point) const override;
    QPainterPath shape() const override;
};

class PolygonShape : public Shape {
public:
    PolygonShape(int id, const QRect& rect, const QColor& color, int sides = 6);
    void draw(QPainter& painter) const override;
    bool contains(const QPointF& point) const override;
    QPainterPath shape() const override;
    
    int sides() const { return m_sides; }
    
private:
    int m_sides;
    QPolygonF getPolygon() const;
};
