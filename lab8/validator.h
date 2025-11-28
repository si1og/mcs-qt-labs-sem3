#pragma once

#include <QString>
#include <QRegularExpression>
#include <QDate>

class Validator {
public:
    struct Result {
        bool valid;
        QString error;
        QString cleaned;

        Result(bool v = false, const QString& e = "", const QString& c = "")
            : valid(v), error(e), cleaned(c) {}
    };

    static Result validateName(const QString& name);
    static Result validatePhone(const QString& phone);
    static Result validateEmail(const QString& email);
    static Result validateBirthDate(const QDate& date);
    static Result validateAddress(const QString& address);

private:
    static QString trimSpaces(const QString& str);
    static bool isLeapYear(int year);
    static int daysInMonth(int month, int year);
};
