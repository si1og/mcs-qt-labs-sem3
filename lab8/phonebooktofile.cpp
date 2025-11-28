#include "phonebooktofile.h"
#include <QDataStream>

PhoneBookFile::PhoneBookFile(const QString& fileName, QObject* parent)
    : QFile(fileName, parent) {}

bool PhoneBookFile::loadContacts(QList<Contact>& contacts) {
    contacts.clear();

    if (!exists()) {
        return true;
    }

    if (!open(QIODevice::ReadOnly)) {
        m_lastError = QString("Не удалось открыть файл: %1").arg(errorString());
        return false;
    }

    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_15);

    quint32 magic, version;
    in >> magic >> version;

    if (magic != FILE_MAGIC) {
        m_lastError = "Неверный формат файла";
        close();
        return false;
    }

    if (version > FILE_VERSION) {
        m_lastError = "Файл создан более новой версией программы";
        close();
        return false;
    }

    in >> contacts;

    if (in.status() != QDataStream::Ok) {
        m_lastError = "Ошибка чтения данных";
        close();
        return false;
    }

    close();
    return true;
}

bool PhoneBookFile::saveContacts(const QList<Contact>& contacts) {
    if (!open(QIODevice::WriteOnly)) {
        m_lastError = QString("Не удалось открыть файл для записи: %1").arg(errorString());
        return false;
    }

    QDataStream out(this);
    out.setVersion(QDataStream::Qt_5_15);

    out << FILE_MAGIC << FILE_VERSION;

    out << contacts;

    if (out.status() != QDataStream::Ok) {
        m_lastError = "Ошибка записи данных";
        close();
        return false;
    }

    close();
    return true;
}
