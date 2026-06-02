#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include "contact.h"

/**
 * DatabaseManager — Singleton-класс для работы с SQLite базой данных.
 * Отвечает за создание таблиц, CRUD-операции над контактами.
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // Получить единственный экземпляр (Singleton)
    static DatabaseManager& instance();

    // Запрещаем копирование
    DatabaseManager(const DatabaseManager&)            = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Открыть / инициализировать БД
    bool openDatabase(const QString& path = "phonebook.db");

    // Закрыть соединение
    void closeDatabase();

    // ── CRUD ──────────────────────────────────────────────────────────────
    // Добавить контакт; возвращает присвоенный id или -1 при ошибке
    int  addContact(const Contact& contact);

    // Получить все контакты (можно фильтровать по строке поиска)
    QList<Contact> getAllContacts(const QString& filter = QString());

    // Получить один контакт по id
    Contact getContactById(int id);

    // Обновить существующий контакт
    bool updateContact(const Contact& contact);

    // Удалить контакт по id
    bool deleteContact(int id);

    // Количество записей в таблице
    int  contactCount();

    // Последнее сообщение об ошибке
    QString lastError() const { return m_lastError; }

private:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager() override;

    bool createTables();          // Создаёт схему БД при первом запуске

    QSqlDatabase m_db;
    QString      m_lastError;
};

#endif // DATABASEMANAGER_H
