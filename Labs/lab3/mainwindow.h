#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "contact.h"

class QTableWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QTextBrowser;

/**
 * MainWindow — главное окно приложения «Телефонная книга».
 *
 * Содержит:
 *   • Строку поиска (фильтрация по имени / фамилии / телефону)
 *   • Таблицу контактов (QTableWidget)
 *   • Панель кнопок: Добавить / Редактировать / Удалить
 *   • Панель деталей — просмотр полного контакта справа
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    void onSearchChanged(const QString& text);
    void onSelectionChanged();
    void onTableDoubleClicked(int row, int col);

private:
    void setupUi();
    void setupConnections();
    void loadContacts(const QString& filter = QString());
    void showContactDetails(const Contact& c);
    void clearDetails();
    int  selectedContactId() const;   // -1 если ничего не выбрано
    void updateStatusBar();

    // ── Виджеты ───────────────────────────────────────────────────────────
    QLineEdit*    m_searchEdit;
    QTableWidget* m_table;
    QPushButton*  m_addBtn;
    QPushButton*  m_editBtn;
    QPushButton*  m_deleteBtn;
    QTextBrowser* m_detailsView;
    QLabel*       m_statusLabel;

    // Текущий фильтр поиска
    QString m_currentFilter;
};

#endif // MAINWINDOW_H
