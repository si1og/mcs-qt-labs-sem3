#pragma once

#include <QWidget>
#include <QComboBox>
#include "shapecanvas.h"

class ShapesCollection : public QWidget {
    Q_OBJECT
public:
    explicit ShapesCollection(QWidget* parent = nullptr);

private:
    void setupUI();

    ShapeCanvas* m_canvas;
    QComboBox* m_typeSelector;
};
