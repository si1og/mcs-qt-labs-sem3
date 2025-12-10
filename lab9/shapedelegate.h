#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

// Делегат для отображения количества фигур иконками
// 1-3 фигуры → 1 иконка
// 4-10 фигур → 2 иконки
// >10 фигур → 3 иконки
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

// Делегат для отображения типа фигуры иконкой
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
