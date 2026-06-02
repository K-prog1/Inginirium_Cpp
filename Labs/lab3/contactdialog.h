#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include "contact.h"

class QLineEdit;
class QTextEdit;
class QLabel;

/**
 * ContactDialog — модальный диалог для добавления или редактирования контакта.
 * При передаче существующего Contact поля заполняются заранее (режим редактирования).
 */
class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    // mode: true = новый контакт, false = редактирование
    explicit ContactDialog(QWidget* parent = nullptr,
                           const Contact& contact = Contact());

    // Вернуть контакт с данными, введёнными пользователем
    Contact contact() const;

private slots:
    void onAccept();   // валидация перед закрытием

private:
    void setupUi();
    void populateFields(const Contact& c);

    QLineEdit* m_firstName;
    QLineEdit* m_lastName;
    QLineEdit* m_phone;
    QLineEdit* m_email;
    QLineEdit* m_address;
    QTextEdit* m_notes;

    Contact m_contact;   // Хранит id для режима редактирования
};

#endif // CONTACTDIALOG_H
