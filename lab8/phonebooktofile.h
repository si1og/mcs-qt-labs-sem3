#pragma once

#include <QFile>
#include <QList>
#include "contact.h"

class PhoneBookFile : public QFile {
    Q_OBJECT
public:
    explicit PhoneBookFile(const QString& fileName, QObject* parent = nullptr);

    bool loadContacts(QList<Contact>& contacts);
    bool saveContacts(const QList<Contact>& contacts);
    QString lastError() const { return m_lastError; }

    static const quint32 FILE_MAGIC = 0x50484F4E;
    static const quint32 FILE_VERSION = 1;

private:
    QString m_lastError;
};
