#include "mainwindow.h"
#include "createcontact.h"
#include "phonebooktofile.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // Путь к файлу данных
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    m_dataFile = dataPath + "/phonebook.dat";

    setupUI();
    setupMenuBar();
    loadData();

    setWindowTitle("Телефонный справочник");
    resize(900, 600);
}

MainWindow::~MainWindow() {
    saveData();
}

void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    QHBoxLayout* searchLayout = new QHBoxLayout();

    m_searchFieldCombo = new QComboBox(this);
    m_searchFieldCombo->addItem("Все поля", -1);
    m_searchFieldCombo->addItem("Фамилия", 0);
    m_searchFieldCombo->addItem("Имя", 1);
    m_searchFieldCombo->addItem("Телефон", 2);
    m_searchFieldCombo->addItem("Email", 3);
    m_searchFieldCombo->addItem("Адрес", 4);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Поиск...");

    searchLayout->addWidget(m_searchFieldCombo);
    searchLayout->addWidget(m_searchEdit, 1);

    mainLayout->addLayout(searchLayout);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({
        "Фамилия", "Имя", "Телефон", "Email", "Адрес", "Дата рождения"
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSortingEnabled(true);
    m_table->setAlternatingRowColors(true);

    mainLayout->addWidget(m_table, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_addButton = new QPushButton("Добавить", this);
    m_editButton = new QPushButton("Редактировать", this);
    m_removeButton = new QPushButton("Удалить", this);

    m_editButton->setEnabled(false);
    m_removeButton->setEnabled(false);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addContact);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::editContact);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::removeContact);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::searchContacts);
    connect(m_searchFieldCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::searchContacts);
    connect(m_table->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::sortByColumn);
    connect(m_table, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onSelectionChanged);
    connect(m_table, &QTableWidget::doubleClicked, this, &MainWindow::editContact);
}

void MainWindow::setupMenuBar() {
    QMenu* fileMenu = menuBar()->addMenu("Файл");

    QAction* addAction = fileMenu->addAction("Добавить контакт");
    addAction->setShortcut(QKeySequence::New);
    connect(addAction, &QAction::triggered, this, &MainWindow::addContact);

    fileMenu->addSeparator();

    QAction* exitAction = fileMenu->addAction("Выход");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    QMenu* editMenu = menuBar()->addMenu("Правка");

    QAction* editAction = editMenu->addAction("Редактировать");
    editAction->setShortcut(QKeySequence(Qt::Key_Return));
    connect(editAction, &QAction::triggered, this, &MainWindow::editContact);

    QAction* removeAction = editMenu->addAction("Удалить");
    removeAction->setShortcut(QKeySequence::Delete);
    connect(removeAction, &QAction::triggered, this, &MainWindow::removeContact);
}

void MainWindow::loadData() {
    PhoneBookFile file(m_dataFile);

    if (!file.loadContacts(m_contacts)) {
        QMessageBox::warning(this, "Ошибка",
            QString("Не удалось загрузить данные:\n%1").arg(file.lastError()));
    }

    updateTable();
}

void MainWindow::saveData() {
    PhoneBookFile file(m_dataFile);

    if (!file.saveContacts(m_contacts)) {
        QMessageBox::warning(this, "Ошибка",
            QString("Не удалось сохранить данные:\n%1").arg(file.lastError()));
    }
}

void MainWindow::updateTable() {
    updateTable(m_contacts);
}

void MainWindow::updateTable(const QList<Contact>& contacts) {
    m_table->setSortingEnabled(false);
    m_table->setRowCount(contacts.size());

    for (int i = 0; i < contacts.size(); ++i) {
        const Contact& c = contacts[i];

        m_table->setItem(i, 0, new QTableWidgetItem(c.lastName));
        m_table->setItem(i, 1, new QTableWidgetItem(c.firstName));
        m_table->setItem(i, 2, new QTableWidgetItem(c.primaryPhone()));
        m_table->setItem(i, 3, new QTableWidgetItem(c.email));
        m_table->setItem(i, 4, new QTableWidgetItem(c.address));
        m_table->setItem(i, 5, new QTableWidgetItem(c.birthDate.toString("dd.MM.yyyy")));

        m_table->item(i, 0)->setData(Qt::UserRole, i);
    }

    m_table->setSortingEnabled(true);
    m_table->resizeColumnsToContents();
}

void MainWindow::addContact() {
    ContactDialog dialog(this);
    dialog.setWindowTitle("Новый контакт");

    if (dialog.exec() == QDialog::Accepted) {
        m_contacts.append(dialog.getContact());
        saveData();
        updateTable();
    }
}

void MainWindow::editContact() {
    int row = m_table->currentRow();
    if (row < 0) return;

    int index = m_table->item(row, 0)->data(Qt::UserRole).toInt();
    if (index < 0 || index >= m_contacts.size()) return;

    ContactDialog dialog(this);
    dialog.setWindowTitle("Редактирование контакта");
    dialog.setContact(m_contacts[index]);

    if (dialog.exec() == QDialog::Accepted) {
        m_contacts[index] = dialog.getContact();
        saveData();
        searchContacts();
    }
}

void MainWindow::removeContact() {
    int row = m_table->currentRow();
    if (row < 0) return;

    int index = m_table->item(row, 0)->data(Qt::UserRole).toInt();
    if (index < 0 || index >= m_contacts.size()) return;

    QString name = m_contacts[index].fullName();

    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Удаление контакта",
        QString("Удалить контакт \"%1\"?").arg(name),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_contacts.removeAt(index);
        saveData();
        searchContacts();
    }
}

void MainWindow::searchContacts() {
    QString query = m_searchEdit->text().trimmed().toLower();
    int field = m_searchFieldCombo->currentData().toInt();

    if (query.isEmpty()) {
        updateTable();
        return;
    }

    QList<Contact> filtered;

    for (int i = 0; i < m_contacts.size(); ++i) {
        const Contact& c = m_contacts[i];
        bool match = false;

        switch (field) {
            case -1:
                match = c.lastName.toLower().contains(query) ||
                        c.firstName.toLower().contains(query) ||
                        c.primaryPhone().contains(query) ||
                        c.email.toLower().contains(query) ||
                        c.address.toLower().contains(query);
                break;
            case 0:
                match = c.lastName.toLower().contains(query);
                break;
            case 1:
                match = c.firstName.toLower().contains(query);
                break;
            case 2:
                match = c.primaryPhone().contains(query) ||
                        c.allPhones().contains(query);
                break;
            case 3:
                match = c.email.toLower().contains(query);
                break;
            case 4:
                match = c.address.toLower().contains(query);
                break;
        }

        if (match) {
            filtered.append(c);
        }
    }

    updateTable(filtered);

    for (int i = 0; i < m_table->rowCount(); ++i) {
        QString lastName = m_table->item(i, 0)->text();
        QString firstName = m_table->item(i, 1)->text();
        QString email = m_table->item(i, 3)->text();

        for (int j = 0; j < m_contacts.size(); ++j) {
            if (m_contacts[j].lastName == lastName &&
                m_contacts[j].firstName == firstName &&
                m_contacts[j].email == email) {
                m_table->item(i, 0)->setData(Qt::UserRole, j);
                break;
            }
        }
    }
}

void MainWindow::sortByColumn(int column) {
    if (column == m_sortColumn) {
        m_sortOrder = (m_sortOrder == Qt::AscendingOrder)
                      ? Qt::DescendingOrder
                      : Qt::AscendingOrder;
    } else {
        m_sortColumn = column;
        m_sortOrder = Qt::AscendingOrder;
    }

    m_table->sortItems(column, m_sortOrder);
}

void MainWindow::onSelectionChanged() {
    bool hasSelection = m_table->currentRow() >= 0;
    m_editButton->setEnabled(hasSelection);
    m_removeButton->setEnabled(hasSelection);
}
