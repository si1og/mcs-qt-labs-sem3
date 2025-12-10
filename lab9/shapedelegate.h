#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

class ShapeCountDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    explicit ShapeCountDelegate(QObject* parent = nullptr);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
    
private:
    void drawShapeIcon(QPainter* painter, int type, const QRect& rect) const;
};
class ShapeTypeDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    explicit ShapeTypeDelegate(QObject* parent = nullptr);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
                   
private:
    void drawShapeIcon(QPainter* painter, int type, const QRect& rect) const;
};
