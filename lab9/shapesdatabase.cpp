#include "shapesdatabase.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QFile>

ShapesDatabase::ShapesDatabase(QObject* parent) 
    : QObject(parent), m_model(nullptr)
{
}

ShapesDatabase::~ShapesDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool ShapesDatabase::initialize() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("shapes.db");
    
    if (!m_db.open()) {
        qWarning() << "Ошибка открытия базы данных:" << m_db.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        return false;
    }
    
    m_model = new QSqlTableModel(this, m_db);
    m_model->setTable("shapes");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "Тип");
    m_model->setHeaderData(2, Qt::Horizontal, "X");
    m_model->setHeaderData(3, Qt::Horizontal, "Y");
    m_model->setHeaderData(4, Qt::Horizontal, "Ширина");
    m_model->setHeaderData(5, Qt::Horizontal, "Высота");
    m_model->setHeaderData(6, Qt::Horizontal, "R");
    m_model->setHeaderData(7, Qt::Horizontal, "G");
    m_model->setHeaderData(8, Qt::Horizontal, "B");
    m_model->setHeaderData(9, Qt::Horizontal, "Стороны");
    m_model->setHeaderData(10, Qt::Horizontal, "Видимость");
    m_model->setHeaderData(11, Qt::Horizontal, "Связи");
    m_model->setHeaderData(12, Qt::Horizontal, "Кол-во типа");
    
    return true;
}

bool ShapesDatabase::createTables() {
    QSqlQuery query(m_db);
    
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS shapes ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   type INTEGER NOT NULL,"
        "   x INTEGER NOT NULL,"
        "   y INTEGER NOT NULL,"
        "   width INTEGER NOT NULL,"
        "   height INTEGER NOT NULL,"
        "   r INTEGER NOT NULL,"
        "   g INTEGER NOT NULL,"
        "   b INTEGER NOT NULL,"
        "   sides INTEGER DEFAULT 6,"
        "   visible INTEGER DEFAULT 1,"
        "   connections TEXT DEFAULT '',"
        "   type_count INTEGER DEFAULT 0"
        ")"
    );
    
    if (!success) {
        qWarning() << "Ошибка создания таблицы shapes:" << query.lastError().text();
        return false;
    }
    
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS connections ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   from_id INTEGER NOT NULL,"
        "   to_id INTEGER NOT NULL,"
        "   UNIQUE(from_id, to_id)"
        ")"
    );
    
    if (!success) {
        qWarning() << "Ошибка создания таблицы connections:" << query.lastError().text();
        return false;
    }
    
    return true;
}

int ShapesDatabase::addShape(Shape::Type type, int x, int y, int width, int height,
                             int r, int g, int b, int sides) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO shapes (type, x, y, width, height, r, g, b, sides, visible, connections, type_count) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, 1, '', 0)"
    );
    query.addBindValue(static_cast<int>(type));
    query.addBindValue(x);
    query.addBindValue(y);
    query.addBindValue(width);
    query.addBindValue(height);
    query.addBindValue(r);
    query.addBindValue(g);
    query.addBindValue(b);
    query.addBindValue(sides);
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления фигуры:" << query.lastError().text();
        return -1;
    }
    
    int id = query.lastInsertId().toInt();
    updateShapeCount(type);
    refresh();
    
    emit shapeAdded(id);
    emit dataChanged();
    
    return id;
}

bool ShapesDatabase::removeShape(int id) {
    ShapeInfo info = getShapeInfo(id);
    
    removeAllConnections(id);
    
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM shapes WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления фигуры:" << query.lastError().text();
        return false;
    }
    
    updateShapeCount(info.type);
    refresh();
    
    emit shapeRemoved(id);
    emit dataChanged();
    
    return true;
}

bool ShapesDatabase::updateShapePosition(int id, int x, int y) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE shapes SET x = ?, y = ? WHERE id = ?");
    query.addBindValue(x);
    query.addBindValue(y);
    query.addBindValue(id);
    
    if (!query.exec()) {
        qWarning() << "Ошибка обновления позиции:" << query.lastError().text();
        return false;
    }
    
    refresh();
    return true;
}

bool ShapesDatabase::setShapeVisible(int id, bool visible) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE shapes SET visible = ? WHERE id = ?");
    query.addBindValue(visible ? 1 : 0);
    query.addBindValue(id);
    
    if (!query.exec()) {
        qWarning() << "Ошибка изменения видимости:" << query.lastError().text();
        return false;
    }
    
    refresh();
    emit dataChanged();
    
    return true;
}

bool ShapesDatabase::addConnection(int fromId, int toId) {
    if (fromId == toId) return false;
    
    int first = qMin(fromId, toId);
    int second = qMax(fromId, toId);
    
    QSqlQuery query(m_db);
    query.prepare("INSERT OR IGNORE INTO connections (from_id, to_id) VALUES (?, ?)");
    query.addBindValue(first);
    query.addBindValue(second);
    
    if (!query.exec()) {
        qWarning() << "Ошибка добавления связи:" << query.lastError().text();
        return false;
    }
    
    auto updateConnField = [this](int shapeId) {
        QList<int> connected = getConnectedShapes(shapeId);
        QStringList ids;
        for (int id : connected) {
            ids << QString::number(id);
        }
        QString connStr = ids.join(",");
        
        QSqlQuery q(m_db);
        q.prepare("UPDATE shapes SET connections = ? WHERE id = ?");
        q.addBindValue(connStr);
        q.addBindValue(shapeId);
        q.exec();
    };
    
    updateConnField(first);
    updateConnField(second);
    
    refresh();
    emit connectionAdded(first, second);
    emit dataChanged();
    
    return true;
}

bool ShapesDatabase::removeConnection(int fromId, int toId) {
    int first = qMin(fromId, toId);
    int second = qMax(fromId, toId);
    
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM connections WHERE from_id = ? AND to_id = ?");
    query.addBindValue(first);
    query.addBindValue(second);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления связи:" << query.lastError().text();
        return false;
    }
    
    auto updateConnField = [this](int shapeId) {
        QList<int> connected = getConnectedShapes(shapeId);
        QStringList ids;
        for (int id : connected) {
            ids << QString::number(id);
        }
        QString connStr = ids.join(",");
        
        QSqlQuery q(m_db);
        q.prepare("UPDATE shapes SET connections = ? WHERE id = ?");
        q.addBindValue(connStr);
        q.addBindValue(shapeId);
        q.exec();
    };
    
    updateConnField(first);
    updateConnField(second);
    
    refresh();
    emit connectionRemoved(first, second);
    emit dataChanged();
    
    return true;
}

bool ShapesDatabase::removeAllConnections(int shapeId) {
    QList<int> connected = getConnectedShapes(shapeId);
    
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM connections WHERE from_id = ? OR to_id = ?");
    query.addBindValue(shapeId);
    query.addBindValue(shapeId);
    
    if (!query.exec()) {
        qWarning() << "Ошибка удаления связей:" << query.lastError().text();
        return false;
    }
    
    for (int otherId : connected) {
        QList<int> otherConnected = getConnectedShapes(otherId);
        QStringList ids;
        for (int id : otherConnected) {
            ids << QString::number(id);
        }
        QString connStr = ids.join(",");
        
        QSqlQuery q(m_db);
        q.prepare("UPDATE shapes SET connections = ? WHERE id = ?");
        q.addBindValue(connStr);
        q.addBindValue(otherId);
        q.exec();
        
        emit connectionRemoved(shapeId, otherId);
    }
    
    query.prepare("UPDATE shapes SET connections = '' WHERE id = ?");
    query.addBindValue(shapeId);
    query.exec();
    
    return true;
}

QList<QPair<int, int>> ShapesDatabase::getAllConnections() {
    QList<QPair<int, int>> result;
    
    QSqlQuery query(m_db);
    query.exec("SELECT from_id, to_id FROM connections");
    
    while (query.next()) {
        result.append({query.value(0).toInt(), query.value(1).toInt()});
    }
    
    return result;
}

QList<int> ShapesDatabase::getConnectedShapes(int shapeId) {
    QList<int> result;
    
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT from_id, to_id FROM connections "
        "WHERE from_id = ? OR to_id = ?"
    );
    query.addBindValue(shapeId);
    query.addBindValue(shapeId);
    query.exec();
    
    while (query.next()) {
        int fromId = query.value(0).toInt();
        int toId = query.value(1).toInt();
        int otherId = (fromId == shapeId) ? toId : fromId;
        if (!result.contains(otherId)) {
            result.append(otherId);
        }
    }
    
    return result;
}

int ShapesDatabase::getShapeCount(Shape::Type type) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM shapes WHERE type = ?");
    query.addBindValue(static_cast<int>(type));
    query.exec();
    
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

ShapesDatabase::ShapeInfo ShapesDatabase::getShapeInfo(int id) {
    ShapeInfo info = {0, Shape::Rectangle, 0, 0, 0, 0, 0, 0, 0, 6, true};
    
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM shapes WHERE id = ?");
    query.addBindValue(id);
    query.exec();
    
    if (query.next()) {
        info.id = query.value(0).toInt();
        info.type = static_cast<Shape::Type>(query.value(1).toInt());
        info.x = query.value(2).toInt();
        info.y = query.value(3).toInt();
        info.width = query.value(4).toInt();
        info.height = query.value(5).toInt();
        info.r = query.value(6).toInt();
        info.g = query.value(7).toInt();
        info.b = query.value(8).toInt();
        info.sides = query.value(9).toInt();
        info.visible = query.value(10).toBool();
    }
    
    return info;
}

QList<ShapesDatabase::ShapeInfo> ShapesDatabase::getAllShapes() {
    QList<ShapeInfo> result;
    
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM shapes");
    
    while (query.next()) {
        ShapeInfo info;
        info.id = query.value(0).toInt();
        info.type = static_cast<Shape::Type>(query.value(1).toInt());
        info.x = query.value(2).toInt();
        info.y = query.value(3).toInt();
        info.width = query.value(4).toInt();
        info.height = query.value(5).toInt();
        info.r = query.value(6).toInt();
        info.g = query.value(7).toInt();
        info.b = query.value(8).toInt();
        info.sides = query.value(9).toInt();
        info.visible = query.value(10).toBool();
        result.append(info);
    }
    
    return result;
}

bool ShapesDatabase::shapeExists(int id) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM shapes WHERE id = ?");
    query.addBindValue(id);
    query.exec();
    
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

QString ShapesDatabase::getDatabasePath() const {
    return m_db.databaseName();
}

bool ShapesDatabase::importFromFile(const QString& filePath) {
    m_model->clear();
    m_db.close();
    
    QString dbPath = "shapes.db";
    QFile::remove(dbPath);
    
    if (!QFile::copy(filePath, dbPath)) {
        m_db.open();
        m_model->setTable("shapes");
        m_model->select();
        return false;
    }
    
    if (!m_db.open()) {
        return false;
    }
    
    m_model->setTable("shapes");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    
    emit dataChanged();
    return true;
}

void ShapesDatabase::updateShapeCount(Shape::Type type) {
    int count = getShapeCount(type);
    
    QSqlQuery query(m_db);
    query.prepare("UPDATE shapes SET type_count = ? WHERE type = ?");
    query.addBindValue(count);
    query.addBindValue(static_cast<int>(type));
    query.exec();
}

void ShapesDatabase::refresh() {
    if (m_model) {
        m_model->select();
    }
}
