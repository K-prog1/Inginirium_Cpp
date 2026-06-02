#include "contactdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

ContactDialog::ContactDialog(QWidget* parent, const Contact& contact)
    : QDialog(parent)
    , m_contact(contact)
{
    setupUi();
    if (contact.id != -1) {
        setWindowTitle("Редактировать контакт");
        populateFields(contact);
    } else {
        setWindowTitle("Новый контакт");
    }
    setMinimumWidth(420);
}

// ────────────────────────────────────────────────────────────────────────────
// Построение UI
// ────────────────────────────────────────────────────────────────────────────
void ContactDialog::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // ── Форма полей ──────────────────────────────────────────────────────
    auto* form = new QFormLayout;
    form->setRowWrapPolicy(QFormLayout::DontWrapRows);
    form->setLabelAlignment(Qt::AlignRight);
    form->setHorizontalSpacing(12);
    form->setVerticalSpacing(8);

    m_firstName = new QLineEdit(this);
    m_firstName->setPlaceholderText("Введите имя");
    form->addRow("Имя *:", m_firstName);

    m_lastName = new QLineEdit(this);
    m_lastName->setPlaceholderText("Введите фамилию");
    form->addRow("Фамилия *:", m_lastName);

    m_phone = new QLineEdit(this);
    m_phone->setPlaceholderText("+7 (999) 123-45-67");
    // Разрешаем цифры, пробелы, +, (), -, точку
    QRegularExpression phoneRx(R"([\d\s\+\(\)\-\.]{0,20})");
    m_phone->setValidator(new QRegularExpressionValidator(phoneRx, this));
    form->addRow("Телефон *:", m_phone);

    m_email = new QLineEdit(this);
    m_email->setPlaceholderText("example@mail.com");
    form->addRow("E-mail:", m_email);

    m_address = new QLineEdit(this);
    m_address->setPlaceholderText("Город, улица, дом");
    form->addRow("Адрес:", m_address);

    m_notes = new QTextEdit(this);
    m_notes->setPlaceholderText("Дополнительные сведения...");
    m_notes->setFixedHeight(80);
    form->addRow("Заметки:", m_notes);

    mainLayout->addLayout(form);

    // Пометка обязательных полей
    auto* hint = new QLabel("* — обязательные поля", this);
    hint->setStyleSheet("color: gray; font-size: 11px;");
    mainLayout->addWidget(hint);

    // ── Кнопки ──────────────────────────────────────────────────────────
    auto* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    auto* cancelBtn = new QPushButton("Отмена", this);
    cancelBtn->setFixedWidth(100);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    auto* saveBtn = new QPushButton("Сохранить", this);
    saveBtn->setFixedWidth(100);
    saveBtn->setDefault(true);
    // Применяем акцентный стиль к кнопке «Сохранить»
    saveBtn->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; border-radius: 4px; padding: 6px; }"
        "QPushButton:hover { background-color: #1976D2; }"
        "QPushButton:pressed { background-color: #0D47A1; }"
    );
    connect(saveBtn, &QPushButton::clicked, this, &ContactDialog::onAccept);

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);
}

// ────────────────────────────────────────────────────────────────────────────
// Заполнение полей при редактировании
// ────────────────────────────────────────────────────────────────────────────
void ContactDialog::populateFields(const Contact& c)
{
    m_firstName->setText(c.firstName);
    m_lastName->setText(c.lastName);
    m_phone->setText(c.phone);
    m_email->setText(c.email);
    m_address->setText(c.address);
    m_notes->setPlainText(c.notes);
}

// ────────────────────────────────────────────────────────────────────────────
// Валидация и принятие диалога
// ────────────────────────────────────────────────────────────────────────────
void ContactDialog::onAccept()
{
    // Обязательные поля
    if (m_firstName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле «Имя» обязательно для заполнения.");
        m_firstName->setFocus();
        return;
    }
    if (m_lastName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле «Фамилия» обязательно для заполнения.");
        m_lastName->setFocus();
        return;
    }
    if (m_phone->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле «Телефон» обязательно для заполнения.");
        m_phone->setFocus();
        return;
    }

    accept();   // Закрыть диалог с кодом Accepted
}

// ────────────────────────────────────────────────────────────────────────────
// Сбор данных из полей
// ────────────────────────────────────────────────────────────────────────────
Contact ContactDialog::contact() const
{
    Contact c;
    c.id        = m_contact.id;   // сохраняем id для режима обновления
    c.firstName = m_firstName->text().trimmed();
    c.lastName  = m_lastName->text().trimmed();
    c.phone     = m_phone->text().trimmed();
    c.email     = m_email->text().trimmed();
    c.address   = m_address->text().trimmed();
    c.notes     = m_notes->toPlainText().trimmed();
    return c;
}
