#ifndef CONTACT_H
#define CONTACT_H

#include <QString>

// Структура для хранения данных об одном контакте
struct Contact {
    int     id;           // Уникальный идентификатор (из БД)
    QString firstName;    // Имя
    QString lastName;     // Фамилия
    QString phone;        // Номер телефона
    QString email;        // E-mail (необязательно)
    QString address;      // Адрес (необязательно)
    QString notes;        // Заметки (необязательно)

    Contact()
        : id(-1)
    {}
};

#endif // CONTACT_H
