#include "mainwindow.h"
#include "databasemanager.h"
#include "contactdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextBrowser>
#include <QMessageBox>
#include <QGroupBox>
#include <QStatusBar>
#include <QShortcut>
#include <QKeySequence>
#include <QApplication>
#include <QStyle>

// ────────────────────────────────────────────────────────────────────────────
// Конструктор / деструктор
// ────────────────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Телефонная книга");
    setMinimumSize(820, 540);
    resize(1024, 640);

    setupUi();
    setupConnections();
    loadContacts();
}

MainWindow::~MainWindow() = default;

// ────────────────────────────────────────────────────────────────────────────
// Построение интерфейса
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::setupUi()
{
    // ── Центральный виджет ───────────────────────────────────────────────
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 6);
    mainLayout->setSpacing(8);

    // ── Панель инструментов (поиск + кнопки) ────────────────────────────
    auto* toolBar = new QHBoxLayout;

    auto* searchIcon = new QLabel("🔍", this);
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Поиск по имени, фамилии или телефону...");
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumWidth(240);

    toolBar->addWidget(searchIcon);
    toolBar->addWidget(m_searchEdit, 1);
    toolBar->addStretch();

    // Кнопки действий
    m_addBtn = new QPushButton("➕  Добавить", this);
    m_editBtn = new QPushButton("✏️  Редактировать", this);
    m_deleteBtn = new QPushButton("🗑  Удалить", this);

    m_addBtn->setFixedHeight(34);
    m_editBtn->setFixedHeight(34);
    m_deleteBtn->setFixedHeight(34);

    m_editBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);

    // Стилизация кнопок
    const QString btnBase = "QPushButton { border-radius: 5px; padding: 0 14px; font-size: 13px; }";
    m_addBtn->setStyleSheet(btnBase +
        "QPushButton { background: #43A047; color: white; }"
        "QPushButton:hover { background: #388E3C; }"
        "QPushButton:pressed { background: #2E7D32; }");
    m_editBtn->setStyleSheet(btnBase +
        "QPushButton { background: #1E88E5; color: white; }"
        "QPushButton:hover { background: #1565C0; }"
        "QPushButton:pressed { background: #0D47A1; }"
        "QPushButton:disabled { background: #BDBDBD; }");
    m_deleteBtn->setStyleSheet(btnBase +
        "QPushButton { background: #E53935; color: white; }"
        "QPushButton:hover { background: #C62828; }"
        "QPushButton:pressed { background: #B71C1C; }"
        "QPushButton:disabled { background: #BDBDBD; }");

    toolBar->addWidget(m_addBtn);
    toolBar->addWidget(m_editBtn);
    toolBar->addWidget(m_deleteBtn);

    mainLayout->addLayout(toolBar);

    // ── Сплиттер: таблица | панель деталей ──────────────────────────────
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(6);

    // ── Таблица контактов ────────────────────────────────────────────────
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Фамилия", "Имя", "Телефон", "E-mail"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSortIndicatorShown(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSortingEnabled(true);
    m_table->setWordWrap(false);

    splitter->addWidget(m_table);

    // ── Панель деталей ────────────────────────────────────────────────────
    auto* detailsGroup = new QGroupBox("Подробная информация", this);
    auto* detailsLayout = new QVBoxLayout(detailsGroup);

    m_detailsView = new QTextBrowser(this);
    m_detailsView->setOpenLinks(false);
    m_detailsView->setStyleSheet("background: #FAFAFA; border: none;");
    detailsLayout->addWidget(m_detailsView);

    splitter->addWidget(detailsGroup);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    mainLayout->addWidget(splitter);

    // ── Статус-бар ────────────────────────────────────────────────────────
    m_statusLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_statusLabel);
}

// ────────────────────────────────────────────────────────────────────────────
// Подключение сигналов / слотов
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::setupConnections()
{
    connect(m_addBtn,    &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(m_editBtn,   &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(m_deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteContact);

    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &MainWindow::onSearchChanged);

    connect(m_table, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onSelectionChanged);

    connect(m_table, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::onTableDoubleClicked);

    // Горячие клавиши
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), this,
                  this, &MainWindow::onAddContact);
    new QShortcut(QKeySequence(Qt::Key_Delete), this,
                  this, &MainWindow::onDeleteContact);
    new QShortcut(QKeySequence(Qt::Key_F2), this,
                  this, &MainWindow::onEditContact);
}

// ────────────────────────────────────────────────────────────────────────────
// Загрузка / обновление таблицы
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::loadContacts(const QString& filter)
{
    m_currentFilter = filter;

    // Временно отключаем сортировку, чтобы избежать ошибок при вставке строк
    m_table->setSortingEnabled(false);

    QList<Contact> contacts = DatabaseManager::instance().getAllContacts(filter);

    m_table->setRowCount(contacts.size());

    for (int row = 0; row < contacts.size(); ++row) {
        const Contact& c = contacts.at(row);

        auto makeItem = [](const QString& text, int id = -1) {
            auto* item = new QTableWidgetItem(text);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            if (id != -1) {
                item->setData(Qt::UserRole, id);
            }
            return item;
        };

        // Первый столбец хранит id через UserRole
        m_table->setItem(row, 0, makeItem(c.lastName,  c.id));
        m_table->setItem(row, 1, makeItem(c.firstName));
        m_table->setItem(row, 2, makeItem(c.phone));
        m_table->setItem(row, 3, makeItem(c.email));
    }

    m_table->setSortingEnabled(true);
    clearDetails();
    updateStatusBar();
}

// ────────────────────────────────────────────────────────────────────────────
// Добавление нового контакта
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::onAddContact()
{
    ContactDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Contact c = dlg.contact();
        int newId = DatabaseManager::instance().addContact(c);
        if (newId == -1) {
            QMessageBox::critical(this, "Ошибка",
                "Не удалось добавить контакт:\n" +
                DatabaseManager::instance().lastError());
        } else {
            loadContacts(m_currentFilter);
        }
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Редактирование выбранного контакта
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::onEditContact()
{
    int id = selectedContactId();
    if (id == -1) return;

    Contact existing = DatabaseManager::instance().getContactById(id);
    ContactDialog dlg(this, existing);
    if (dlg.exec() == QDialog::Accepted) {
        Contact updated = dlg.contact();
        if (!DatabaseManager::instance().updateContact(updated)) {
            QMessageBox::critical(this, "Ошибка",
                "Не удалось обновить контакт:\n" +
                DatabaseManager::instance().lastError());
        } else {
            loadContacts(m_currentFilter);
        }
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Удаление выбранного контакта
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::onDeleteContact()
{
    int id = selectedContactId();
    if (id == -1) return;

    Contact c = DatabaseManager::instance().getContactById(id);
    const QString fullName = c.firstName + " " + c.lastName;

    auto reply = QMessageBox::question(
        this,
        "Подтверждение удаления",
        QString("Вы действительно хотите удалить контакт\n«%1»?").arg(fullName),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (!DatabaseManager::instance().deleteContact(id)) {
            QMessageBox::critical(this, "Ошибка",
                "Не удалось удалить контакт:\n" +
                DatabaseManager::instance().lastError());
        } else {
            loadContacts(m_currentFilter);
        }
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Фильтрация при вводе в поле поиска
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::onSearchChanged(const QString& text)
{
    loadContacts(text.trimmed());
}

// ────────────────────────────────────────────────────────────────────────────
// Обработка выбора строки в таблице
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::onSelectionChanged()
{
    int id = selectedContactId();
    bool hasSelection = (id != -1);

    m_editBtn->setEnabled(hasSelection);
    m_deleteBtn->setEnabled(hasSelection);

    if (hasSelection) {
        Contact c = DatabaseManager::instance().getContactById(id);
        showContactDetails(c);
    } else {
        clearDetails();
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Двойной клик — открыть диалог редактирования
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::onTableDoubleClicked(int /*row*/, int /*col*/)
{
    onEditContact();
}

// ────────────────────────────────────────────────────────────────────────────
// Показать детали контакта
// ────────────────────────────────────────────────────────────────────────────
void MainWindow::showContactDetails(const Contact& c)
{
    auto row = [](const QString& label, const QString& value) -> QString {
        if (value.isEmpty()) return QString();
        return QString("<tr><td style='color:#757575;padding-right:10px;'>%1</td>"
                       "<td><b>%2</b></td></tr>")
                   .arg(label, value.toHtmlEscaped());
    };

    QString html = QString(
        "<div style='font-family: sans-serif; padding: 8px;'>"
        "<h2 style='margin:0 0 12px 0; color:#1565C0;'>%1 %2</h2>"
        "<table cellspacing='4'>%3%4%5%6</table>"
        "</div>"
    ).arg(
        c.firstName.toHtmlEscaped(),
        c.lastName.toHtmlEscaped(),
        row("📞 Телефон:", c.phone),
        row("✉️ E-mail:",  c.email),
        row("🏠 Адрес:",   c.address),
        row("📝 Заметки:", c.notes)
    );

    m_detailsView->setHtml(html);
}

void MainWindow::clearDetails()
{
    m_detailsView->setHtml(
        "<div style='color:#9E9E9E; font-family:sans-serif; padding:16px;'>"
        "Выберите контакт для просмотра деталей</div>");
}

// ────────────────────────────────────────────────────────────────────────────
// Вспомогательные методы
// ────────────────────────────────────────────────────────────────────────────
int MainWindow::selectedContactId() const
{
    const QList<QTableWidgetItem*> selected = m_table->selectedItems();
    if (selected.isEmpty()) return -1;

    // id хранится в UserRole первого столбца той же строки
    int row = selected.first()->row();
    QTableWidgetItem* firstCol = m_table->item(row, 0);
    if (!firstCol) return -1;
    return firstCol->data(Qt::UserRole).toInt();
}

void MainWindow::updateStatusBar()
{
    int total   = DatabaseManager::instance().contactCount();
    int visible = m_table->rowCount();

    if (m_currentFilter.isEmpty()) {
        m_statusLabel->setText(QString("Контактов: %1").arg(total));
    } else {
        m_statusLabel->setText(
            QString("Показано: %1 из %2").arg(visible).arg(total));
    }
}
