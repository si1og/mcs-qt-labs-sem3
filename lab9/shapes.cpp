#include "shapes.h"
#include <QPen>
#include <QtMath>
#include <QGraphicsScene>

// ============== ConnectionLine ==============

ConnectionLine::ConnectionLine(Shape* from, Shape* to)
    : m_from(from), m_to(to)
{
    setPen(QPen(Qt::darkGray, 2, Qt::DashLine));
    setZValue(-1); // Линии под фигурами
    updatePosition();
}

void ConnectionLine::updatePosition() {
    if (m_from && m_to) {
        QPointF fromCenter = m_from->pos() + m_from->boundingRect().center();
        QPointF toCenter = m_to->pos() + m_to->boundingRect().center();
        setLine(QLineF(fromCenter, toCenter));
    }
}

// ============== Shape ==============

Shape::Shape(int id, Type type, const QRect& rect, const QColor& color)
    : m_id(id), m_type(type), m_rect(rect), m_color(color)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

Shape::~Shape() {
    // Удаляем все связи
    for (ConnectionLine* line : m_connections) {
        Shape* other = (line->fromShape() == this) ? line->toShape() : line->fromShape();
        if (other) {
            other->m_connections.removeAll(line);
        }
        if (scene()) {
            scene()->removeItem(line);
        }
        delete line;
    }
    m_connections.clear();
}

bool Shape::contains(const QPointF& point) const {
    return m_rect.contains(point.toPoint());
}

QRectF Shape::boundingRect() const {
    qreal pw = m_active ? 3.0 : 1.0;
    return QRectF(m_rect).adjusted(-pw/2, -pw/2, pw/2, pw/2);
}

void Shape::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    if (!m_visible) return;
    
    painter->setRenderHint(QPainter::Antialiasing);
    draw(*painter);
}

void Shape::setVisible(bool visible) {
    m_visible = visible;
    QGraphicsItem::setVisible(visible);
    
    // Обновляем видимость связей
    for (ConnectionLine* line : m_connections) {
        Shape* other = (line->fromShape() == this) ? line->toShape() : line->fromShape();
        line->setVisible(visible && other && other->isShapeVisible());
    }
}

void Shape::addConnection(ConnectionLine* line) {
    if (!m_connections.contains(line)) {
        m_connections.append(line);
    }
}

void Shape::removeConnection(ConnectionLine* line) {
    m_connections.removeAll(line);
}

QVariant Shape::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        // Обновляем позиции всех связей
        for (ConnectionLine* line : m_connections) {
            line->updatePosition();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

QString Shape::typeToString(Type type) {
    switch (type) {
        case Rectangle: return "Прямоугольник";
        case Triangle: return "Треугольник";
        case Ellipse: return "Эллипс";
        case Polygon: return "Многоугольник";
    }
    return "Неизвестно";
}

Shape::Type Shape::stringToType(const QString& str) {
    if (str == "Прямоугольник") return Rectangle;
    if (str == "Треугольник") return Triangle;
    if (str == "Эллипс") return Ellipse;
    if (str == "Многоугольник") return Polygon;
    return Rectangle;
}

// ============== RectangleShape ==============

RectangleShape::RectangleShape(int id, const QRect& rect, const QColor& color)
    : Shape(id, Rectangle, rect, color) {}

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

// ============== TriangleShape ==============

TriangleShape::TriangleShape(int id, const QRect& rect, const QColor& color)
    : Shape(id, Triangle, rect, color) {}

QPolygon TriangleShape::getTrianglePolygon() const {
    QPolygon triangle;
    triangle << QPoint(m_rect.left() + m_rect.width() / 2, m_rect.top())
             << QPoint(m_rect.left(), m_rect.bottom())
             << QPoint(m_rect.right(), m_rect.bottom());
    return triangle;
}

void TriangleShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);
    
    if (m_active) {
        painter.setPen(QPen(QColor("#006e9c"), 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }
    
    painter.drawPolygon(getTrianglePolygon());
}

bool TriangleShape::contains(const QPointF& point) const {
    return shape().contains(point);
}

QPainterPath TriangleShape::shape() const {
    QPainterPath path;
    path.addPolygon(getTrianglePolygon());
    path.closeSubpath();
    return path;
}

// ============== EllipseShape ==============

EllipseShape::EllipseShape(int id, const QRect& rect, const QColor& color)
    : Shape(id, Ellipse, rect, color) {}

void EllipseShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);
    
    if (m_active) {
        painter.setPen(QPen(QColor("#006e9c"), 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }
    
    painter.drawEllipse(m_rect);
}

bool EllipseShape::contains(const QPointF& point) const {
    return shape().contains(point);
}

QPainterPath EllipseShape::shape() const {
    QPainterPath path;
    path.addEllipse(m_rect);
    return path;
}

// ============== PolygonShape ==============

PolygonShape::PolygonShape(int id, const QRect& rect, const QColor& color, int sides)
    : Shape(id, Polygon, rect, color), m_sides(sides) {}

QPolygonF PolygonShape::getPolygon() const {
    QPolygonF polygon;
    QPointF center(m_rect.center());
    qreal rx = m_rect.width() / 2.0;
    qreal ry = m_rect.height() / 2.0;
    
    for (int i = 0; i < m_sides; ++i) {
        qreal angle = 2.0 * M_PI * i / m_sides - M_PI / 2.0;
        qreal x = center.x() + rx * qCos(angle);
        qreal y = center.y() + ry * qSin(angle);
        polygon << QPointF(x, y);
    }
    return polygon;
}

void PolygonShape::draw(QPainter& painter) const {
    painter.setBrush(m_color);
    
    if (m_active) {
        painter.setPen(QPen(QColor("#006e9c"), 3));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }
    
    painter.drawPolygon(getPolygon());
}

bool PolygonShape::contains(const QPointF& point) const {
    return shape().contains(point);
}

QPainterPath PolygonShape::shape() const {
    QPainterPath path;
    path.addPolygon(getPolygon());
    path.closeSubpath();
    return path;
}
