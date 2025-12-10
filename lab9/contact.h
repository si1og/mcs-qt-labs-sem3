#pragma once

#include <QString>
#include <QDate>
#include <QList>
#include <QDataStream>

struct PhoneNumber {
    enum Type { Work, Home, Mobile };

    Type type;
    QString number;

    PhoneNumber(Type t = Mobile, const QString& n = "")
        : type(t), number(n) {}

    QString typeString() const {
        switch (type) {
            case Work: return "Рабочий";
            case Home: return "Домашний";
            case Mobile: return "Мобильный";
            default: return "Другой";
        }
    }

    QString formatted() const {
        if (number.length() == 11 && number.startsWith("7")) {
            return QString("+%1 (%2) %3-%4-%5")
                .arg(number.mid(0, 1))
                .arg(number.mid(1, 3))
                .arg(number.mid(4, 3))
                .arg(number.mid(7, 2))
                .arg(number.mid(9, 2));
        }
        return "+" + number;
    }
};

struct Contact {
    QString firstName;
    QString lastName;
    QString middleName;
    QString address;
    QDate birthDate;
    QString email;
    QList<PhoneNumber> phones;

    QString fullName() const {
        return QString("%1 %2 %3").arg(lastName, firstName, middleName).trimmed();
    }

    QString primaryPhone() const {
        return phones.isEmpty() ? "" : phones.first().formatted();
    }

    QString allPhones() const {
        QStringList list;
        for (const auto& phone : phones) {
            list << QString("%1: %2").arg(phone.typeString(), phone.formatted());
        }
        return list.join("; ");
    }
};

inline QDataStream& operator<<(QDataStream& out, const PhoneNumber& phone) {
    out << static_cast<int>(phone.type) << phone.number;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, PhoneNumber& phone) {
    int type;
    in >> type >> phone.number;
    phone.type = static_cast<PhoneNumber::Type>(type);
    return in;
}

inline QDataStream& operator<<(QDataStream& out, const Contact& contact) {
    out << contact.firstName
        << contact.lastName
        << contact.middleName
        << contact.address
        << contact.birthDate
        << contact.email
        << contact.phones;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, Contact& contact) {
    in >> contact.firstName
       >> contact.lastName
       >> contact.middleName
       >> contact.address
       >> contact.birthDate
       >> contact.email
       >> contact.phones;
    return in;
}
