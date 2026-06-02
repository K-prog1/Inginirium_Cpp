#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>

#include "mainwindow.h"
#include "databasemanager.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // ── Мета-данные приложения ────────────────────────────────────────────
    QApplication::setApplicationName("Телефонная книга");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("MyCompany");

    // ── Стиль Fusion обеспечивает одинаковый вид на всех платформах ───────
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Глобальная таблица стилей (светлая тема в стиле Material)
    qApp->setStyleSheet(R"(
        QMainWindow {
            background-color: #F5F5F5;
        }
        QTableWidget {
            gridline-color: #E0E0E0;
            background-color: #FFFFFF;
            selection-background-color: #BBDEFB;
            selection-color: #000000;
            font-size: 13px;
        }
        QTableWidget::item:alternate {
            background-color: #F3F8FF;
        }
        QHeaderView::section {
            background-color: #1565C0;
            color: white;
            padding: 6px 8px;
            border: none;
            font-weight: bold;
        }
        QLineEdit {
            border: 1px solid #BDBDBD;
            border-radius: 4px;
            padding: 5px 8px;
            font-size: 13px;
            background: white;
        }
        QLineEdit:focus {
            border-color: #1E88E5;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #E0E0E0;
            border-radius: 6px;
            margin-top: 8px;
            background: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px;
            color: #1565C0;
        }
        QStatusBar {
            background: #E3F2FD;
            color: #1565C0;
        }
        QScrollBar:vertical {
            width: 8px;
            background: #F5F5F5;
        }
        QScrollBar::handle:vertical {
            background: #BDBDBD;
            border-radius: 4px;
        }
    )");

    // ── Инициализация базы данных ─────────────────────────────────────────
    if (!DatabaseManager::instance().openDatabase("phonebook.db")) {
        QMessageBox::critical(nullptr,
            "Критическая ошибка",
            "Не удалось открыть базу данных.\n" +
            DatabaseManager::instance().lastError());
        return 1;
    }

    // ── Запуск главного окна ─────────────────────────────────────────────
    MainWindow window;
    window.show();

    int result = app.exec();

    DatabaseManager::instance().closeDatabase();
    return result;
}
