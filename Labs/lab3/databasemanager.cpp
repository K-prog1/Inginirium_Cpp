#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

// ────────────────────────────────────────────────────────────────────────────
// Singleton
// ────────────────────────────────────────────────────────────────────────────
DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
{}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

// ────────────────────────────────────────────────────────────────────────────
// Открытие базы данных
// ────────────────────────────────────────────────────────────────────────────
bool DatabaseManager::openDatabase(const QString& path)
{
    // Qt поддерживает несколько драйверов; используем встроенный SQLite
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qCritical() << "Cannot open database:" << m_lastError;
        return false;
    }

    // Включаем поддержку внешних ключей (SQLite выключает её по умолчанию)
    QSqlQuery pragma;
    pragma.exec("PRAGMA foreign_keys = ON;");

    return createTables();
}

void DatabaseManager::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Создание схемы БД
// ────────────────────────────────────────────────────────────────────────────
bool DatabaseManager::createTables()
{
    QSqlQuery q;
    const QString createSQL = R"(
        CREATE TABLE IF NOT EXISTS contacts (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            firstName TEXT    NOT NULL,
            lastName  TEXT    NOT NULL,
            phone     TEXT    NOT NULL,
            email     TEXT    DEFAULT '',
            address   TEXT    DEFAULT '',
            notes     TEXT    DEFAULT ''
        );
    )";

    if (!q.exec(createSQL)) {
        m_lastError = q.lastError().text();
        qCritical() << "createTables error:" << m_lastError;
        return false;
    }
    return true;
}

// ────────────────────────────────────────────────────────────────────────────
// Добавление контакта
// ────────────────────────────────────────────────────────────────────────────
int DatabaseManager::addContact(const Contact& contact)
{
    QSqlQuery q;
    q.prepare(R"(
        INSERT INTO contacts (firstName, lastName, phone, email, address, notes)
        VALUES (:firstName, :lastName, :phone, :email, :address, :notes)
    )");

    q.bindValue(":firstName", contact.firstName);
    q.bindValue(":lastName",  contact.lastName);
    q.bindValue(":phone",     contact.phone);
    q.bindValue(":email",     contact.email);
    q.bindValue(":address",   contact.address);
    q.bindValue(":notes",     contact.notes);

    if (!q.exec()) {
        m_lastError = q.lastError().text();
        qWarning() << "addContact error:" << m_lastError;
        return -1;
    }

    // lastInsertId() возвращает id только что добавленной строки
    return q.lastInsertId().toInt();
}

// ────────────────────────────────────────────────────────────────────────────
// Получение всех контактов (с необязательной фильтрацией)
// ────────────────────────────────────────────────────────────────────────────
QList<Contact> DatabaseManager::getAllContacts(const QString& filter)
{
    QList<Contact> list;
    QSqlQuery q;

    if (filter.isEmpty()) {
        q.prepare("SELECT * FROM contacts ORDER BY lastName, firstName");
    } else {
        // Ищем по имени, фамилии или номеру телефона
        q.prepare(R"(
            SELECT * FROM contacts
            WHERE firstName LIKE :f
               OR lastName  LIKE :f
               OR phone     LIKE :f
            ORDER BY lastName, firstName
        )");
        q.bindValue(":f", "%" + filter + "%");
    }

    if (!q.exec()) {
        m_lastError = q.lastError().text();
        qWarning() << "getAllContacts error:" << m_lastError;
        return list;
    }

    while (q.next()) {
        Contact c;
        c.id        = q.value("id").toInt();
        c.firstName = q.value("firstName").toString();
        c.lastName  = q.value("lastName").toString();
        c.phone     = q.value("phone").toString();
        c.email     = q.value("email").toString();
        c.address   = q.value("address").toString();
        c.notes     = q.value("notes").toString();
        list.append(c);
    }
    return list;
}

// ────────────────────────────────────────────────────────────────────────────
// Получение одного контакта по id
// ────────────────────────────────────────────────────────────────────────────
Contact DatabaseManager::getContactById(int id)
{
    Contact c;
    QSqlQuery q;
    q.prepare("SELECT * FROM contacts WHERE id = :id");
    q.bindValue(":id", id);

    if (q.exec() && q.next()) {
        c.id        = q.value("id").toInt();
        c.firstName = q.value("firstName").toString();
        c.lastName  = q.value("lastName").toString();
        c.phone     = q.value("phone").toString();
        c.email     = q.value("email").toString();
        c.address   = q.value("address").toString();
        c.notes     = q.value("notes").toString();
    }
    return c;
}

// ────────────────────────────────────────────────────────────────────────────
// Обновление контакта
// ────────────────────────────────────────────────────────────────────────────
bool DatabaseManager::updateContact(const Contact& contact)
{
    QSqlQuery q;
    q.prepare(R"(
        UPDATE contacts
        SET firstName = :firstName,
            lastName  = :lastName,
            phone     = :phone,
            email     = :email,
            address   = :address,
            notes     = :notes
        WHERE id = :id
    )");

    q.bindValue(":firstName", contact.firstName);
    q.bindValue(":lastName",  contact.lastName);
    q.bindValue(":phone",     contact.phone);
    q.bindValue(":email",     contact.email);
    q.bindValue(":address",   contact.address);
    q.bindValue(":notes",     contact.notes);
    q.bindValue(":id",        contact.id);

    if (!q.exec()) {
        m_lastError = q.lastError().text();
        qWarning() << "updateContact error:" << m_lastError;
        return false;
    }
    return true;
}

// ────────────────────────────────────────────────────────────────────────────
// Удаление контакта
// ────────────────────────────────────────────────────────────────────────────
bool DatabaseManager::deleteContact(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM contacts WHERE id = :id");
    q.bindValue(":id", id);

    if (!q.exec()) {
        m_lastError = q.lastError().text();
        qWarning() << "deleteContact error:" << m_lastError;
        return false;
    }
    return true;
}

// ────────────────────────────────────────────────────────────────────────────
// Количество контактов
// ────────────────────────────────────────────────────────────────────────────
int DatabaseManager::contactCount()
{
    QSqlQuery q("SELECT COUNT(*) FROM contacts");
    if (q.next()) {
        return q.value(0).toInt();
    }
    return 0;
}
