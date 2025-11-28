#include "shapecanvas.h"
#include <QRandomGenerator>
#include <algorithm>

ShapeCanvas::ShapeCanvas(QWidget* parent) : QWidget(parent) {
    setMinimumSize(400, 300);
    setMouseTracking(true);
}

void ShapeCanvas::addShape(Shape::Type type) {
    QRandomGenerator* rng = QRandomGenerator::global();

    int w = rng->bounded(60, 120);
    int h = rng->bounded(60, 120);
    int x = rng->bounded(0, width() - w);
    int y = rng->bounded(0, height() - h);

    QRect rect(x, y, w, h);

    QColor color(rng->bounded(50, 255),
                 rng->bounded(50, 255),
                 rng->bounded(50, 255));

    std::unique_ptr<Shape> shape;

    switch (type) {
        case Shape::Rectangle:
            shape = std::make_unique<RectangleShape>(rect, color);
            break;
        case Shape::Triangle:
            shape = std::make_unique<TriangleShape>(rect, color);
            break;
        case Shape::Ellipse:
            shape = std::make_unique<EllipseShape>(rect, color);
            break;
    }

    if (m_activeShape) {
        m_activeShape->setActive(false);
    }

    shape->setActive(true);
    m_activeShape = shape.get();

    m_shapes.push_back(std::move(shape));
    update();
}

void ShapeCanvas::removeActiveShape() {
    if (!m_activeShape) return;

    auto it = std::find_if(m_shapes.begin(), m_shapes.end(),
        [this](const std::unique_ptr<Shape>& s) {
            return s.get() == m_activeShape;
        });

    if (it != m_shapes.end()) {
        m_shapes.erase(it);
        m_activeShape = nullptr;
    }

    update();
}

bool ShapeCanvas::hasActiveShape() const {
    return m_activeShape != nullptr;
}

void ShapeCanvas::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (const auto& shape : m_shapes) {
        shape->draw(painter);
    }
}

void ShapeCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->pos();
        Shape* clickedShape = shapeAt(pos);

        if (m_activeShape) {
            m_activeShape->setActive(false);
        }

        if (clickedShape) {
            clickedShape->setActive(true);
            m_activeShape = clickedShape;
            bringToFront(clickedShape);

            m_lastMousePos = pos;
            m_dragging = true;
        } else {
            m_activeShape = nullptr;
        }

        update();
    }
}

void ShapeCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging && m_activeShape) {
        QPoint pos = event->pos();
        QPoint delta = pos - m_lastMousePos;

        m_activeShape->move(delta);
        m_lastMousePos = pos;

        update();
    }
}

void ShapeCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
}

Shape* ShapeCanvas::shapeAt(const QPoint& pos) {
    for (auto it = m_shapes.rbegin(); it != m_shapes.rend(); ++it) {
        if ((*it)->contains(pos)) {
            return it->get();
        }
    }
    return nullptr;
}

void ShapeCanvas::bringToFront(Shape* shape) {
    auto it = std::find_if(m_shapes.begin(), m_shapes.end(),
        [shape](const std::unique_ptr<Shape>& s) {
            return s.get() == shape;
        });

    if (it != m_shapes.end() && it != m_shapes.end() - 1) {
        std::unique_ptr<Shape> temp = std::move(*it);
        m_shapes.erase(it);
        m_shapes.push_back(std::move(temp));
        m_activeShape = m_shapes.back().get();
    }
}
