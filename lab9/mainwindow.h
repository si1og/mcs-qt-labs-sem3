#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QSpinBox>
#include <QTableView>
#include <QStatusBar>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>

#include "shapecanvas.h"
#include "shapesdatabase.h"
#include "shapedelegate.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onAddShape();
    void onRemoveShape();
    void onToggleVisibility();
    void onAddConnection();
    void onRemoveConnection();
    void onActiveShapeChanged(::Shape* shape);
    void onTableSelectionChanged();
    void onFilterChanged();
    void onConnectionRequested(int fromId, int toId);
    void onTypeChanged(int index);
    void onExportDatabase();
    void onImportDatabase();
    void onGenerateGraph();

private:
    void setupUI();
    void setupConnections();
    void updateStatusBar(::Shape* shape);
    void refreshTable();

    ShapesDatabase* m_db;
    ShapeCanvas* m_canvas;

    QComboBox* m_typeSelector;
    QSpinBox* m_widthSpin;
    QSpinBox* m_heightSpin;
    QSpinBox* m_sidesSpin;
    QLabel* m_sidesLabel;

    QTableView* m_tableView;

    QCheckBox* m_filterRect;
    QCheckBox* m_filterTriangle;
    QCheckBox* m_filterEllipse;
    QCheckBox* m_filterPolygon;
    QLineEdit* m_filterIdEdit;

    QPushButton* m_connectBtn;
    QPushButton* m_disconnectBtn;
    int m_connectionFirstId = -1;

    QLineEdit* m_degreeSequenceEdit;
    QPushButton* m_generateGraphBtn;

    QLabel* m_coordsLabel;
    QLabel* m_colorLabel;
    QLabel* m_sizeLabel;
    QLabel* m_idLabel;

    ShapeCountDelegate* m_countDelegate;
    ShapeTypeDelegate* m_typeDelegate;
};
