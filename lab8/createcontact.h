#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "contact.h"

class ContactDialog : public QDialog {
    Q_OBJECT
public:
    explicit ContactDialog(QWidget* parent = nullptr);

    void setContact(const Contact& contact);
    Contact getContact() const;

private slots:
    void addPhone();
    void removePhone();
    void validate();

private:
    void setupUI();
    void updatePhoneTable();
    bool validateAll();
    void showError(QLabel* label, const QString& error);
    void clearError(QLabel* label);

    QLineEdit* m_lastNameEdit;
    QLineEdit* m_firstNameEdit;
    QLineEdit* m_middleNameEdit;
    QLineEdit* m_addressEdit;
    QDateEdit* m_birthDateEdit;
    QLineEdit* m_emailEdit;

    QTableWidget* m_phonesTable;
    QLineEdit* m_phoneInput;
    QComboBox* m_phoneTypeCombo;

    QLabel* m_lastNameError;
    QLabel* m_firstNameError;
    QLabel* m_middleNameError;
    QLabel* m_addressError;
    QLabel* m_birthDateError;
    QLabel* m_emailError;
    QLabel* m_phoneError;

    QPushButton* m_okButton;
    QPushButton* m_cancelButton;

    QList<PhoneNumber> m_phones;
};
