#pragma once

#include "shapes.h"
#include <QPen>
#include <QPolygon>


Shape::Shape(Type type, const QRect& rect, const QColor& color)
    : m_type(type), m_rect(rect), m_color(color) {}

bool Shape::contains(const QPoint& point) const {
    return m_rect.contains(point);
}

void Shape::move(const QPoint& delta) {
    m_rect.translate(delta);
}


RectangleShape::RectangleShape(const QRect& rect, const QColor& color)
    : Shape(Rectangle, rect, color) {}

void RectangleShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);

    if (m_active) {
        painter.setPen(QPen(Qt::black, 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }

    painter.drawRect(m_rect);
}


TriangleShape::TriangleShape(const QRect& rect, const QColor& color)
    : Shape(Triangle, rect, color) {}

void TriangleShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);

    if (m_active) {
        painter.setPen(QPen(Qt::black, 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }

    QPolygon triangle;
    triangle << QPoint(m_rect.left() + m_rect.width() / 2, m_rect.top())
             << QPoint(m_rect.left(), m_rect.bottom())
             << QPoint(m_rect.right(), m_rect.bottom());

    painter.drawPolygon(triangle);
}


EllipseShape::EllipseShape(const QRect& rect, const QColor& color)
    : Shape(Ellipse, rect, color) {}

void EllipseShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);

    if (m_active) {
        painter.setPen(QPen(Qt::black, 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }

    painter.drawEllipse(m_rect);
}
