# Телефонная книга — Qt приложение

## Структура проекта

```
phonebook/
├── phonebook.pro       # Файл проекта Qt
├── main.cpp            # Точка входа, инициализация БД и стилей
├── contact.h           # Структура Contact
├── databasemanager.h   # Заголовок класса DatabaseManager (Singleton)
├── databasemanager.cpp # Реализация CRUD через QSqlDatabase + SQLite
├── mainwindow.h        # Главное окно
├── mainwindow.cpp      # Логика UI: таблица, поиск, кнопки
├── contactdialog.h     # Диалог добавления / редактирования
└── contactdialog.cpp   # Валидация формы
```

## Требования

- Qt 5.12+ или Qt 6.x
- Модуль `sql` (входит в стандартную установку Qt)
- Компилятор C++17 (GCC, Clang, MSVC)

## Сборка

### Qt Creator (рекомендуется)
1. Открыть `phonebook.pro` в Qt Creator
2. Нажать **Build → Build Project** (Ctrl+B)
3. Нажать **Run** (Ctrl+R)

### Командная строка (Linux / macOS)
```bash
cd phonebook
qmake phonebook.pro
make -j$(nproc)
./phonebook
```

### Командная строка (Windows с MinGW)
```cmd
cd phonebook
qmake phonebook.pro
mingw32-make
phonebook.exe
```

## База данных

Файл `phonebook.db` (SQLite) создаётся автоматически в рабочей директории
при первом запуске. Схема:

```sql
CREATE TABLE contacts (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    firstName TEXT NOT NULL,
    lastName  TEXT NOT NULL,
    phone     TEXT NOT NULL,
    email     TEXT DEFAULT '',
    address   TEXT DEFAULT '',
    notes     TEXT DEFAULT ''
);
```

## Функциональность

| Действие        | UI                         | Горячая клавиша |
|-----------------|----------------------------|-----------------|
| Добавить        | Кнопка «Добавить»          | Ctrl+N          |
| Редактировать   | Кнопка / двойной клик      | F2              |
| Удалить         | Кнопка «Удалить»           | Delete          |
| Поиск           | Поле поиска (живой фильтр) | —               |
| Просмотр деталей| Панель справа              | —               |

## Архитектура

```
main.cpp
  └── DatabaseManager::openDatabase()   ← singleton, инициализация SQLite
  └── MainWindow::show()

MainWindow
  ├── использует DatabaseManager для CRUD
  ├── открывает ContactDialog для add/edit
  └── отображает результаты в QTableWidget

DatabaseManager (Singleton)
  ├── addContact()
  ├── getAllContacts(filter)
  ├── getContactById()
  ├── updateContact()
  └── deleteContact()
```
