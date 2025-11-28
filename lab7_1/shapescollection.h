#pragma once

#include <QWidget>
#include <QComboBox>
#include <QStatusBar>
#include <QLabel>
#include "shapecanvas.h"

class ShapesCollection : public QWidget {
    Q_OBJECT
public:
    explicit ShapesCollection(QWidget* parent = nullptr);



private:
    void setupUI();
    void updateStatusBar(Shape* shape);

    ShapeCanvas* m_canvas;
    QComboBox* m_typeSelector;
    QStatusBar* m_statusBar;

    QLabel* m_coordsLabel;
    QLabel* m_colorLabel;
    QLabel* m_sizeLabel;
};
