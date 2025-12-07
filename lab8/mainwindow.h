#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QList>
#include "contact.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void addContact();
    void editContact();
    void removeContact();
    void searchContacts();
    void sortByColumn(int column);
    void onSelectionChanged();
    void exportToYaml();
    void importFromYaml();

private:
    void setupUI();
    void setupMenuBar();
    void loadData();
    void saveData();
    void updateTable();
    void updateTable(const QList<Contact>& contacts);

    QTableWidget* m_table;
    QLineEdit* m_searchEdit;
    QComboBox* m_searchFieldCombo;
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_removeButton;

    QList<Contact> m_contacts;
    QString m_dataFile;

    int m_sortColumn = -1;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
    int m_sortClickCount = 0;
};