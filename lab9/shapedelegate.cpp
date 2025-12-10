#include "shapedelegate.h"
#include <QtMath>
#include <QApplication>

// ============== ShapeCountDelegate ==============

ShapeCountDelegate::ShapeCountDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

void ShapeCountDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                               const QModelIndex& index) const {
    // Рисуем фон
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }
    
    int count = index.data().toInt();
    if (count <= 0) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    
    // Определяем количество иконок
    int iconCount = 1;
    if (count >= 4 && count <= 10) {
        iconCount = 2;
    } else if (count > 10) {
        iconCount = 3;
    }
    
    // Получаем тип фигуры из соседнего столбца (столбец 1 = type)
    int type = index.sibling(index.row(), 1).data().toInt();
    
    // Размер иконки
    int iconSize = qMin(option.rect.height() - 4, 20);
    int totalWidth = iconCount * iconSize + (iconCount - 1) * 2;
    int startX = option.rect.x() + (option.rect.width() - totalWidth) / 2;
    int startY = option.rect.y() + (option.rect.height() - iconSize) / 2;
    
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    
    for (int i = 0; i < iconCount; ++i) {
        QRect iconRect(startX + i * (iconSize + 2), startY, iconSize, iconSize);
        drawShapeIcon(painter, type, iconRect);
    }
    
    painter->restore();
}

QSize ShapeCountDelegate::sizeHint(const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const {
    Q_UNUSED(index);
    return QSize(80, option.rect.height());
}

void ShapeCountDelegate::drawShapeIcon(QPainter* painter, int type, const QRect& rect) const {
    painter->setBrush(QColor(100, 150, 200));
    painter->setPen(QPen(Qt::black, 1));
    
    switch (type) {
        case 0: // Rectangle
            painter->drawRect(rect.adjusted(2, 2, -2, -2));
            break;
        case 1: { // Triangle
            QPolygon triangle;
            triangle << QPoint(rect.center().x(), rect.top() + 2)
                     << QPoint(rect.left() + 2, rect.bottom() - 2)
                     << QPoint(rect.right() - 2, rect.bottom() - 2);
            painter->drawPolygon(triangle);
            break;
        }
        case 2: // Ellipse
            painter->drawEllipse(rect.adjusted(2, 2, -2, -2));
            break;
        case 3: { // Polygon (hexagon)
            QPolygonF polygon;
            QPointF center = rect.center();
            qreal r = (rect.width() - 4) / 2.0;
            for (int i = 0; i < 6; ++i) {
                qreal angle = 2.0 * M_PI * i / 6 - M_PI / 2;
                polygon << QPointF(center.x() + r * qCos(angle),
                                  center.y() + r * qSin(angle));
            }
            painter->drawPolygon(polygon);
            break;
        }
    }
}

// ============== ShapeTypeDelegate ==============

ShapeTypeDelegate::ShapeTypeDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

void ShapeTypeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const {
    // Рисуем фон
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }
    
    int type = index.data().toInt();
    
    // Размер иконки
    int iconSize = qMin(option.rect.height() - 4, 24);
    int startX = option.rect.x() + 5;
    int startY = option.rect.y() + (option.rect.height() - iconSize) / 2;
    
    QRect iconRect(startX, startY, iconSize, iconSize);
    
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    
    drawShapeIcon(painter, type, iconRect);
    
    // Рисуем название типа
    QString typeName;
    switch (type) {
        case 0: typeName = "Прямоуг."; break;
        case 1: typeName = "Треуг."; break;
        case 2: typeName = "Эллипс"; break;
        case 3: typeName = "Многоуг."; break;
        default: typeName = "?"; break;
    }
    
    QRect textRect = option.rect;
    textRect.setLeft(startX + iconSize + 5);
    painter->setPen(option.palette.text().color());
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, typeName);
    
    painter->restore();
}

QSize ShapeTypeDelegate::sizeHint(const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const {
    Q_UNUSED(index);
    return QSize(100, option.rect.height());
}

void ShapeTypeDelegate::drawShapeIcon(QPainter* painter, int type, const QRect& rect) const {
    painter->setBrush(QColor(100, 150, 200));
    painter->setPen(QPen(Qt::black, 1));
    
    switch (type) {
        case 0: // Rectangle
            painter->drawRect(rect.adjusted(2, 2, -2, -2));
            break;
        case 1: { // Triangle
            QPolygon triangle;
            triangle << QPoint(rect.center().x(), rect.top() + 2)
                     << QPoint(rect.left() + 2, rect.bottom() - 2)
                     << QPoint(rect.right() - 2, rect.bottom() - 2);
            painter->drawPolygon(triangle);
            break;
        }
        case 2: // Ellipse
            painter->drawEllipse(rect.adjusted(2, 2, -2, -2));
            break;
        case 3: { // Polygon (hexagon)
            QPolygonF polygon;
            QPointF center = rect.center();
            qreal r = (rect.width() - 4) / 2.0;
            for (int i = 0; i < 6; ++i) {
                qreal angle = 2.0 * M_PI * i / 6 - M_PI / 2;
                polygon << QPointF(center.x() + r * qCos(angle),
                                  center.y() + r * qSin(angle));
            }
            painter->drawPolygon(polygon);
            break;
        }
    }
}
