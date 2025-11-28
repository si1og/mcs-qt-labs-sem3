#pragma once

#include <QRect>
#include <QColor>
#include <QPainter>
#include <QPoint>

class Shape {
public:
    enum Type { Rectangle, Triangle, Ellipse };

    Shape(Type type, const QRect& rect, const QColor& color);
    virtual ~Shape() = default;

    virtual void draw(QPainter& painter) const = 0;
    virtual bool contains(const QPoint& point) const;

    void move(const QPoint& delta);
    void setActive(bool active) { m_active = active; }
    bool isActive() const { return m_active; }

    QRect boundingRect() const { return m_rect; }
    Type type() const { return m_type; }
    QColor color() const { return m_color; }

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
};

class TriangleShape : public Shape {
public:
    TriangleShape(const QRect& rect, const QColor& color);
    void draw(QPainter& painter) const override;
};

class EllipseShape : public Shape {
public:
    EllipseShape(const QRect& rect, const QColor& color);
    void draw(QPainter& painter) const override;
};
