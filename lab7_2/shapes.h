#pragma once

#include <QGraphicsItem>
#include <QRect>
#include <QColor>
#include <QPainter>
#include <QPoint>

class Shape : public QGraphicsItem {
public:
    enum Type { Rectangle, Triangle, Ellipse };

    Shape(Type type, const QRect& rect, const QColor& color);
    virtual ~Shape() = default;

    virtual void draw(QPainter& painter) const = 0;
    virtual bool contains(const QPoint& point) const;

    void move(const QPoint& delta);
    void setActive(bool active) { m_active = active; update(); }
    bool isActive() const { return m_active; }

    QRect rect() const { return m_rect; }
    Type shapeType() const { return m_type; }
    QColor color() const { return m_color; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    Type m_type;
    QRect m_rect;
    QColor m_color;
    bool m_active = false;
};

class RectangleShape : public Shape {
public:
    RectangleShape(const QRect& rect, const QColor& color);
    void draw(QPainter& painter) const override;
    QPainterPath shape() const override;
};

class TriangleShape : public Shape {
public:
    TriangleShape(const QRect& rect, const QColor& color);
    void draw(QPainter& painter) const override;
    bool contains(const QPoint& point) const override;
    QPainterPath shape() const override;
};

class EllipseShape : public Shape {
public:
    EllipseShape(const QRect& rect, const QColor& color);
    void draw(QPainter& painter) const override;
    bool contains(const QPoint& point) const override;
    QPainterPath shape() const override;
};
