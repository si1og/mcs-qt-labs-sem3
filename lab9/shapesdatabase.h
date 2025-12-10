#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QPair>
#include "shapes.h"

class ShapesDatabase : public QObject {
    Q_OBJECT
    
public:
    explicit ShapesDatabase(QObject* parent = nullptr);
    ~ShapesDatabase();
    
    bool initialize();
    
    // CRUD операции для фигур
    int addShape(Shape::Type type, int x, int y, int width, int height, 
                 int r, int g, int b, int sides = 6);
    bool removeShape(int id);
    bool updateShapePosition(int id, int x, int y);
    bool setShapeVisible(int id, bool visible);
    
    // Связи
    bool addConnection(int fromId, int toId);
    bool removeConnection(int fromId, int toId);
    bool removeAllConnections(int shapeId);
    QList<QPair<int, int>> getAllConnections();
    QList<int> getConnectedShapes(int shapeId);
    
    // Получение данных
    QSqlTableModel* getModel() { return m_model; }
    int getShapeCount(Shape::Type type);
    
    // Информация о фигуре
    struct ShapeInfo {
        int id;
        Shape::Type type;
        int x, y, width, height;
        int r, g, b;
        int sides;
        bool visible;
    };
    
    ShapeInfo getShapeInfo(int id);
    QList<ShapeInfo> getAllShapes();
    bool shapeExists(int id);
    
    // Экспорт/Импорт
    QString getDatabasePath() const;
    bool importFromFile(const QString& filePath);
    
    void refresh();
    
signals:
    void shapeAdded(int id);
    void shapeRemoved(int id);
    void connectionAdded(int fromId, int toId);
    void connectionRemoved(int fromId, int toId);
    void dataChanged();
    
private:
    bool createTables();
    void updateShapeCount(Shape::Type type);
    
    QSqlDatabase m_db;
    QSqlTableModel* m_model;
};
