#include "shapecanvas.h"
#include "shapes.h"
#include <QRandomGenerator>

ShapeCanvas::ShapeCanvas(QWidget* parent) : QGraphicsView(parent) {
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 400, 300);
    setScene(m_scene);

    setRenderHint(QPainter::Antialiasing);
    setMinimumSize(400, 300);
    setBackgroundBrush(Qt::transparent);

    viewport()->setAutoFillBackground(false);
    setStyleSheet("background: transparent;");

    setFrameShape(QFrame::NoFrame);
}

void ShapeCanvas::addShape(::Shape::Type type) {
    QRandomGenerator* rng = QRandomGenerator::global();

    int w = rng->bounded(60, 120);
    int h = rng->bounded(60, 120);
    int x = rng->bounded(0, (int)m_scene->sceneRect().width() - w);
    int y = rng->bounded(0, (int)m_scene->sceneRect().height() - h);

    QRect rect(0, 0, w, h);

    QColor color(rng->bounded(50, 100),
                 rng->bounded(50, 100),
                 rng->bounded(50, 100));

    ::Shape* shape = nullptr;

    switch (type) {
        case ::Shape::Rectangle:
            shape = new RectangleShape(rect, color);
            break;
        case ::Shape::Triangle:
            shape = new TriangleShape(rect, color);
            break;
        case ::Shape::Ellipse:
            shape = new EllipseShape(rect, color);
            break;
    }

    if (m_activeShape) {
        m_activeShape->setActive(false);
    }

    shape->setPos(x, y);
    shape->setActive(true);
    m_activeShape = shape;

    m_scene->addItem(shape);

    emit activeShapeChanged(m_activeShape);
}

void ShapeCanvas::removeActiveShape() {
    if (!m_activeShape) return;

    m_scene->removeItem(m_activeShape);
    delete m_activeShape;
    m_activeShape = nullptr;

    QList<QGraphicsItem*> items = m_scene->items(Qt::DescendingOrder);

    if (items.size() > 0) {
        ::Shape* shape = dynamic_cast<::Shape*>(items.last());
        if (shape) {
            shape->setActive(true);
            m_activeShape = shape;
        }
    }

    emit activeShapeChanged(m_activeShape);
}

bool ShapeCanvas::hasActiveShape() const {
    return m_activeShape != nullptr;
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
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

::Shape* ShapeCanvas::shapeAt(const QPoint& pos) {
    QPointF scenePos = mapToScene(pos);
    QGraphicsItem* item = m_scene->itemAt(scenePos, QTransform());
    return dynamic_cast<::Shape*>(item);
}

void ShapeCanvas::bringToFront(::Shape* shape) {
    qreal maxZ = 0;
    for (QGraphicsItem* item : m_scene->items()) {
        if (item->zValue() > maxZ) {
            maxZ = item->zValue();
        }
    }
    shape->setZValue(maxZ + 1);
}
