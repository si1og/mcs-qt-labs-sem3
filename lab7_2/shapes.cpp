#include "shapes.h"
#include <QPen>
#include <QPolygon>


Shape::Shape(Type type, const QRect& rect, const QColor& color)
    : m_type(type), m_rect(rect), m_color(color)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

bool Shape::contains(const QPoint& point) const {
    return m_rect.contains(point);
}

void Shape::move(const QPoint& delta) {
    setPos(pos() + QPointF(delta));
}

QRectF Shape::boundingRect() const {
    qreal pw = m_active ? 3. : 1.;
    return QRectF(m_rect).adjusted(-pw/2, -pw/2, pw/2, pw/2);
}

void Shape::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing);
    draw(*painter);
}

RectangleShape::RectangleShape(const QRect& rect, const QColor& color)
    : Shape(Rectangle, rect, color) {}

void RectangleShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);

    if (m_active) {
        painter.setPen(QPen(QColor("#006e9c"), 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }

    painter.drawRect(m_rect);
}

QPainterPath RectangleShape::shape() const {
    QPainterPath path;
    path.addRect(m_rect);
    return path;
}

TriangleShape::TriangleShape(const QRect& rect, const QColor& color)
    : Shape(Triangle, rect, color) {}

void TriangleShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);

    if (m_active) {
        painter.setPen(QPen(QColor("#006e9c"), 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }

    QPolygon triangle;
    triangle << QPoint(m_rect.left() + m_rect.width() / 2, m_rect.top())
             << QPoint(m_rect.left(), m_rect.bottom())
             << QPoint(m_rect.right(), m_rect.bottom());

    painter.drawPolygon(triangle);
}

bool TriangleShape::contains(const QPoint& point) const {
    return shape().contains(point);
}

QPainterPath TriangleShape::shape() const {
    QPainterPath path;
    QPolygon triangle;
    triangle << QPoint(m_rect.left() + m_rect.width() / 2, m_rect.top())
             << QPoint(m_rect.left(), m_rect.bottom())
             << QPoint(m_rect.right(), m_rect.bottom());
    path.addPolygon(triangle);
    path.closeSubpath();
    return path;
}

EllipseShape::EllipseShape(const QRect& rect, const QColor& color)
    : Shape(Ellipse, rect, color) {}

void EllipseShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);

    if (m_active) {
        painter.setPen(QPen(QColor("#006e9c"), 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }

    painter.drawEllipse(m_rect);
}

bool EllipseShape::contains(const QPoint& point) const {
    return shape().contains(point);
}

QPainterPath EllipseShape::shape() const {
    QPainterPath path;
    path.addEllipse(m_rect);
    return path;
}
