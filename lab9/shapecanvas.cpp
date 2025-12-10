#include "shapecanvas.h"
#include <QRandomGenerator>

ShapeCanvas::ShapeCanvas(ShapesDatabase* db, QWidget* parent) 
    : QGraphicsView(parent), m_db(db) 
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 600, 400);
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setMinimumSize(600, 400);
    setBackgroundBrush(Qt::transparent);
    
    viewport()->setAutoFillBackground(false);
    setStyleSheet("background: transparent;");
    
    setFrameShape(QFrame::StyledPanel);
}

void ShapeCanvas::addShape(::Shape::Type type, int width, int height, int sides) {
    QRandomGenerator* rng = QRandomGenerator::global();
    
    int x = rng->bounded(0, (int)m_scene->sceneRect().width() - width);
    int y = rng->bounded(0, (int)m_scene->sceneRect().height() - height);
    
    QColor color(rng->bounded(80, 200),
                 rng->bounded(80, 200),
                 rng->bounded(80, 200));
    
    int id = m_db->addShape(type, x, y, width, height, 
                           color.red(), color.green(), color.blue(), sides);
    
    if (id < 0) return;
    
    ShapesDatabase::ShapeInfo info = m_db->getShapeInfo(id);
    ::Shape* shape = createShapeFromInfo(info);
    
    if (m_activeShape) {
        m_activeShape->setActive(false);
    }
    
    shape->setActive(true);
    m_activeShape = shape;
    
    m_shapes[id] = shape;
    m_scene->addItem(shape);
    
    emit activeShapeChanged(m_activeShape);
}

void ShapeCanvas::removeShape(int id) {
    if (!m_shapes.contains(id)) return;
    
    ::Shape* shape = m_shapes[id];
    
    QVector<ConnectionLine*> connections = shape->connections();
    for (ConnectionLine* line : connections) {
        ::Shape* other = (line->fromShape() == shape) ? line->toShape() : line->fromShape();
        if (other) {
            int otherId = other->id();
            auto key = qMakePair(qMin(id, otherId), qMax(id, otherId));
            m_connectionLines.remove(key);
        }
    }
    
    if (m_activeShape == shape) {
        m_activeShape = nullptr;
    }
    
    m_scene->removeItem(shape);
    m_shapes.remove(id);
    delete shape;
    
    m_db->removeShape(id);
    
    emit activeShapeChanged(m_activeShape);
}

void ShapeCanvas::setShapeVisible(int id, bool visible) {
    if (!m_shapes.contains(id)) return;
    
    m_shapes[id]->setVisible(visible);
    m_db->setShapeVisible(id, visible);
}

void ShapeCanvas::addConnection(int fromId, int toId) {
    if (!m_shapes.contains(fromId) || !m_shapes.contains(toId)) return;
    if (fromId == toId) return;
    
    auto key = qMakePair(qMin(fromId, toId), qMax(fromId, toId));
    if (m_connectionLines.contains(key)) return;
    
    ::Shape* from = m_shapes[fromId];
    ::Shape* to = m_shapes[toId];
    
    ConnectionLine* line = new ConnectionLine(from, to);
    from->addConnection(line);
    to->addConnection(line);
    
    m_scene->addItem(line);
    m_connectionLines[key] = line;
    
    m_db->addConnection(fromId, toId);
}

void ShapeCanvas::removeConnection(int fromId, int toId) {
    auto key = qMakePair(qMin(fromId, toId), qMax(fromId, toId));
    
    if (!m_connectionLines.contains(key)) return;
    
    ConnectionLine* line = m_connectionLines[key];
    
    if (m_shapes.contains(fromId)) {
        m_shapes[fromId]->removeConnection(line);
    }
    if (m_shapes.contains(toId)) {
        m_shapes[toId]->removeConnection(line);
    }
    
    m_scene->removeItem(line);
    m_connectionLines.remove(key);
    delete line;
    
    m_db->removeConnection(fromId, toId);
}

::Shape* ShapeCanvas::getShapeById(int id) const {
    return m_shapes.value(id, nullptr);
}

void ShapeCanvas::loadFromDatabase() {
    for (auto* shape : m_shapes) {
        m_scene->removeItem(shape);
        delete shape;
    }
    m_shapes.clear();
    
    for (auto* line : m_connectionLines) {
        m_scene->removeItem(line);
        delete line;
    }
    m_connectionLines.clear();
    
    m_activeShape = nullptr;
    
    QList<ShapesDatabase::ShapeInfo> shapes = m_db->getAllShapes();
    for (const auto& info : shapes) {
        ::Shape* shape = createShapeFromInfo(info);
        m_shapes[info.id] = shape;
        m_scene->addItem(shape);
    }
    
    QList<QPair<int, int>> connections = m_db->getAllConnections();
    for (const auto& conn : connections) {
        if (m_shapes.contains(conn.first) && m_shapes.contains(conn.second)) {
            ::Shape* from = m_shapes[conn.first];
            ::Shape* to = m_shapes[conn.second];
            
            ConnectionLine* line = new ConnectionLine(from, to);
            from->addConnection(line);
            to->addConnection(line);
            
            auto key = qMakePair(qMin(conn.first, conn.second), 
                                 qMax(conn.first, conn.second));
            m_connectionLines[key] = line;
            m_scene->addItem(line);
        }
    }
}

void ShapeCanvas::selectShapeForConnection(int id) {
    if (!m_shapes.contains(id)) return;
    
    if (!m_connectionStart) {
        m_connectionStart = m_shapes[id];
    } else if (m_connectionStart->id() != id) {
        emit connectionRequested(m_connectionStart->id(), id);
        m_connectionStart = nullptr;
    }
}

void ShapeCanvas::clearConnectionSelection() {
    m_connectionStart = nullptr;
}

::Shape* ShapeCanvas::createShapeFromInfo(const ShapesDatabase::ShapeInfo& info) {
    QRect rect(0, 0, info.width, info.height);
    QColor color(info.r, info.g, info.b);
    
    ::Shape* shape = nullptr;
    
    switch (info.type) {
        case ::Shape::Rectangle:
            shape = new RectangleShape(info.id, rect, color);
            break;
        case ::Shape::Triangle:
            shape = new TriangleShape(info.id, rect, color);
            break;
        case ::Shape::Ellipse:
            shape = new EllipseShape(info.id, rect, color);
            break;
        case ::Shape::Polygon:
            shape = new PolygonShape(info.id, rect, color, info.sides);
            break;
    }
    
    if (shape) {
        shape->setPos(info.x, info.y);
        shape->setVisible(info.visible);
    }
    
    return shape;
}

void ShapeCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->pos();
        ::Shape* clickedShape = shapeAt(pos);
        
        if (m_activeShape) {
            m_activeShape->setActive(false);
        }
        
        if (clickedShape) {
            clickedShape->setActive(true);
            m_activeShape = clickedShape;
            bringToFront(clickedShape);
            m_dragging = true;
        } else {
            m_activeShape = nullptr;
        }
        
        emit activeShapeChanged(m_activeShape);
    }
    
    QGraphicsView::mousePressEvent(event);
}

void ShapeCanvas::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsView::mouseMoveEvent(event);
    
    if (m_dragging && m_activeShape) {
        emit activeShapeChanged(m_activeShape);
    }
}

void ShapeCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_dragging && m_activeShape) {
        QPointF pos = m_activeShape->pos();
        m_db->updateShapePosition(m_activeShape->id(), pos.x(), pos.y());
        emit shapePositionChanged(m_activeShape->id(), pos.x(), pos.y());
        m_dragging = false;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

::Shape* ShapeCanvas::shapeAt(const QPoint& pos) {
    QPointF scenePos = mapToScene(pos);
    QList<QGraphicsItem*> items = m_scene->items(scenePos);
    
    for (QGraphicsItem* item : items) {
        ::Shape* shape = dynamic_cast<::Shape*>(item);
        if (shape && shape->isShapeVisible()) {
            return shape;
        }
    }
    return nullptr;
}

void ShapeCanvas::bringToFront(::Shape* shape) {
    qreal maxZ = 0;
    for (QGraphicsItem* item : m_scene->items()) {
        if (item->zValue() > maxZ && dynamic_cast<::Shape*>(item)) {
            maxZ = item->zValue();
        }
    }
    shape->setZValue(maxZ + 1);
}
