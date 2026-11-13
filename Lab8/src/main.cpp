#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include <sqlite3.h>

#include "Database.h"
#include "IUserRepository.h"
#include "PrintDocumentDataAccess.h"
#include "SQLiteUserRepository.h"
#include "TestData.h"
#include "User.h"

int insertUser(
    sqlite3* db,
    const User& user)
{
    const char* sql = R"(
        INSERT INTO User (name, email, department)
        VALUES (?, ?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &stmt,
        nullptr
    );

    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    rc = sqlite3_bind_text(
        stmt, 1,
        user.name.c_str(),
        -1, SQLITE_TRANSIENT
    );

    if (rc == SQLITE_OK)
    {
        rc = sqlite3_bind_text(
            stmt, 2,
            user.email.c_str(),
            -1, SQLITE_TRANSIENT
        );
    }

    if (rc == SQLITE_OK)
    {
        rc = sqlite3_bind_text(
            stmt, 3,
            user.department.c_str(),
            -1, SQLITE_TRANSIENT
        );
    }

    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(error);
    }

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(error);
    }

    int id = static_cast<int>(
        sqlite3_last_insert_rowid(db)
        );

    sqlite3_finalize(stmt);

    return id;
}

std::optional<User> findUserById(
    sqlite3* db,
    int id)
{
    const char* sql = R"(
        SELECT id, name, email, department
        FROM User
        WHERE id = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    rc = sqlite3_bind_int(stmt, 1, id);

    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(error);
    }

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        User user;

        user.id = sqlite3_column_int(stmt, 0);

        user.name = reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, 1)
            );

        user.email = reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, 2)
            );

        user.department = reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, 3)
            );

        sqlite3_finalize(stmt);

        return user;
    }

    if (rc != SQLITE_DONE)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(error);
    }

    sqlite3_finalize(stmt);

    return std::nullopt;
}

void runOrmDemo()
{
    auto storage = createStorage("print_queue.db");

    storage.on_open = [](sqlite3* db)
        {
            sqlite3_exec(
                db,
                "PRAGMA foreign_keys = ON;",
                nullptr,
                nullptr,
                nullptr
            );
        };

    storage.open_forever();
    storage.sync_schema();

    User user{
        0,
        "Oleh Vyshnia",
        "cherry228@gmail.com",
        "IT"
    };

    int id = storage.insert(user);

    std::cout << "\nORM demonstration\n";
    std::cout << "Inserted user ID: "
        << id << '\n';

    auto result = storage.get_optional<User>(id);

    if (result)
    {
        std::cout << "User found:\n";
        std::cout << "ID: " << result->id << '\n';
        std::cout << "Name: " << result->name << '\n';
        std::cout << "Email: "
            << result->email << '\n';
        std::cout << "Department: "
            << result->department << '\n';
    }
    else
    {
        std::cout << "User not found\n";
    }
}

void runRelationshipDemo()
{
    auto storage = createStorage("print_queue.db");

    storage.on_open = [](sqlite3* db)
        {
            sqlite3_exec(
                db,
                "PRAGMA foreign_keys = ON;",
                nullptr,
                nullptr,
                nullptr
            );
        };

    storage.open_forever();
    storage.sync_schema();

    // Додавання користувача
    User user{
        0,
        "Iryna Bozhko",
        "irishka@gmail.com",
        "Marketing"
    };

    int userId = storage.insert(user);

    std::cout << "\nRelationship demonstration\n";
    std::cout << "User ID: "
        << userId << '\n';

    // Перший документ
    PrintDocument d1{
        0,
        userId,
        "Quarterly report.pdf",
        "queued",
        40,
        3
    };

    // Другий документ
    PrintDocument d2{
        0,
        userId,
        "Invoice 2026-11.pdf",
        "queued",
        2,
        5
    };

    int documentId1 = storage.insert(d1);
    int documentId2 = storage.insert(d2);

    std::cout << "Print documents inserted: "
        << documentId1 << ", "
        << documentId2 << '\n';

    using namespace sqlite_orm;

    auto documents = storage.get_all<PrintDocument>(
        where(
            c(&PrintDocument::userId) == userId
        )
    );

    std::cout << "\nPrint documents of the user:\n";

    for (const auto& document : documents)
    {
        std::cout
            << "ID: " << document.id
            << ", Name: " << document.documentName
            << ", Pages: " << document.pages
            << ", Status: " << document.status
            << ", Priority: " << document.priority
            << '\n';
    }

    try
    {
        PrintDocument invalidRecord{
            0,
            -1,
            "Invalid document.pdf",
            "queued",
            1,
            1
        };

        storage.insert(invalidRecord);

        std::cout << "Unexpected insertion success\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nForeign key constraint test:\n";
        std::cout << "Insertion rejected: "
            << ex.what() << '\n';
    }
}

void runRepositoryDemo()
{
    SQLiteUserRepository sqliteRepository("print_queue.db");

    IUserRepository& repository = sqliteRepository;

    // Окремий компонент для залежної сутності PrintDocument
    PrintDocumentDataAccess documents("print_queue.db");

    std::cout << "\nRepository demonstration\n";

    // 1. CREATE
    User user{
        0,
        "Sherlock Holmes",
        "bs221b@example.com",
        "Finance"
    };

    int id = repository.add(user);

    std::cout << "\nCREATE\n";
    std::cout << "Inserted user ID: " << id << '\n';

    // 2. READ
    std::cout << "\nREAD\n";

    auto found = repository.findById(id);

    if (found)
    {
        std::cout << "ID: " << found->id << '\n';
        std::cout << "Name: " << found->name << '\n';
        std::cout << "Email: " << found->email << '\n';
        std::cout << "Department: " << found->department << '\n';
    }

    // 3. UPDATE
    std::cout << "\nUPDATE\n";

    if (found)
    {
        found->department = "Accounting";

        bool updated = repository.update(*found);

        std::cout << "Updated: "
            << std::boolalpha << updated << '\n';

        auto afterUpdate = repository.findById(id);

        if (afterUpdate)
        {
            std::cout << "New department: "
                << afterUpdate->department << '\n';
        }
    }

    // 4. READ ALL
    std::cout << "\nREAD ALL\n";

    auto allUsers = repository.findAll();

    for (const auto& item : allUsers)
    {
        std::cout
            << item.id << " | "
            << item.name << " | "
            << item.email << " | "
            << item.department << '\n';
    }

    // 5. Документи користувача (окремий компонент)
    std::cout << "\nPRINT DOCUMENTS\n";

    PrintDocument document{
        0,
        id,
        "Meeting agenda.pdf",
        "queued",
        3,
        2
    };

    int documentId = documents.add(document);

    std::cout << "Inserted document ID: "
        << documentId << '\n';

    auto userDocuments = documents.findByUserId(id);

    for (const auto& item : userDocuments)
    {
        std::cout
            << item.id << " | "
            << item.documentName << " | "
            << item.status << " | "
            << item.pages << " pages\n";
    }

    bool statusUpdated = documents.updateStatus(documentId, "printing");

    std::cout << "Status updated: "
        << std::boolalpha << statusUpdated << '\n';

    auto afterStatus = documents.findById(documentId);

    if (afterStatus)
    {
        std::cout << "New status: "
            << afterStatus->status << '\n';
    }

    bool documentRemoved = documents.remove(documentId);

    std::cout << "Document removed: "
        << std::boolalpha << documentRemoved << '\n';

    // 6. DELETE
    std::cout << "\nDELETE\n";

    bool removed = repository.remove(id);

    std::cout << "Removed: "
        << std::boolalpha << removed << '\n';

    // 7. Перевірка після видалення
    auto afterDelete = repository.findById(id);

    std::cout << "Record exists after deletion: "
        << std::boolalpha
        << afterDelete.has_value()
        << '\n';
}

void runQueryDemo()
{
    using namespace sqlite_orm;

    auto storage = createStorage("print_queue_queries.db");

    storage.sync_schema();

    std::cout << "\n=== ORM QUERY DEMONSTRATION ===\n";

    // 1. Фільтрація за кількістю сторінок
    std::cout << "\n1. Documents with more than 10 pages:\n";

    auto largeDocuments = storage.get_all<PrintDocument>(
        where(c(&PrintDocument::pages) > 10)
    );

    for (const auto& item : largeDocuments)
    {
        std::cout
            << item.id << " | "
            << item.documentName << " | "
            << item.pages << '\n';
    }

    // 2. Сортування користувачів за іменем
    std::cout << "\n2. Users sorted by name:\n";

    auto sortedUsers = storage.get_all<User>(
        order_by(&User::name).asc()
    );

    for (const auto& item : sortedUsers)
    {
        std::cout
            << item.id << " | "
            << item.name << " | "
            << item.department << '\n';
    }

    // 3. Три найбільші документи
    std::cout << "\n3. Top 3 documents by pages:\n";

    auto biggestDocuments = storage.get_all<PrintDocument>(
        order_by(&PrintDocument::pages).desc(),
        limit(3)
    );

    for (const auto& record : biggestDocuments)
    {
        std::cout
            << record.id << " | "
            << record.documentName << " | "
            << record.pages << '\n';
    }

    // 4. Поєднання фільтрації та сортування: черга друку
    std::cout << "\n4. Queued documents sorted by priority:\n";

    auto queue = storage.get_all<PrintDocument>(
        where(c(&PrintDocument::status) == "queued"),
        order_by(&PrintDocument::priority).desc()
    );

    for (const auto& item : queue)
    {
        std::cout
            << "Priority " << item.priority << " | "
            << item.documentName << '\n';
    }
}

void runAggregateDemo()
{
    using namespace sqlite_orm;

    auto storage = createStorage("print_queue_queries.db");

    storage.sync_schema();

    std::cout << "\n=== AGGREGATE QUERY DEMONSTRATION ===\n";

    // 1. Кількість документів
    auto documentCount = storage.count<PrintDocument>();

    std::cout << "\n1. Print document count: "
        << documentCount << '\n';

    // 2. Загальна кількість сторінок
    auto totalPages = storage.sum(&PrintDocument::pages);

    std::cout << "\n2. Total pages: "
        << (totalPages ? *totalPages : 0) << '\n';

    // 3. Середня кількість сторінок
    auto averagePages = storage.avg(&PrintDocument::pages);

    std::cout << "\n3. Average pages per document: "
        << averagePages << '\n';

    // 4. Групування за користувачем
    std::cout << "\n4. Pages by user:\n";

    auto pagesByUser = storage.select(
        columns(
            &PrintDocument::userId,
            sum(&PrintDocument::pages)
        ),
        group_by(&PrintDocument::userId)
    );

    for (const auto& [userId, pages] : pagesByUser)
    {
        std::cout
            << "User ID: " << userId
            << " | Total pages: " << (pages ? *pages : 0)
            << '\n';
    }
}

void runJoinDemo()
{
    using namespace sqlite_orm;

    auto storage = createStorage("print_queue_queries.db");

    storage.sync_schema();

    std::cout << "\n=== JOIN QUERY DEMONSTRATION ===\n";

    // 1. Об'єднання User і PrintDocument
    std::cout << "\n1. Print documents with their owners:\n";

    auto records = storage.select(
        columns(
            &User::name,
            &PrintDocument::documentName,
            &PrintDocument::pages,
            &PrintDocument::status
        ),
        inner_join<PrintDocument>(
            on(c(&User::id) == &PrintDocument::userId)
        ),
        order_by(&PrintDocument::id)
    );

    for (const auto& [name, document, pages, status] : records)
    {
        std::cout
            << name << " | "
            << document << " | "
            << pages << " | "
            << status << '\n';
    }

    // 2. Об'єднання таблиць із фільтрацією
    std::cout << "\n2. Documents with more than 20 pages:\n";

    auto bigRecords = storage.select(
        columns(
            &User::name,
            &PrintDocument::documentName,
            &PrintDocument::pages
        ),
        inner_join<PrintDocument>(
            on(c(&User::id) == &PrintDocument::userId)
        ),
        where(c(&PrintDocument::pages) > 20),
        order_by(&PrintDocument::pages).desc()
    );

    for (const auto& [name, document, pages] : bigRecords)
    {
        std::cout
            << name << " | "
            << document << " | "
            << pages << '\n';
    }
}

void runTransactionDemo()
{
    auto storage = createStorage("print_queue_queries.db");

    storage.on_open = [](sqlite3* db)
        {
            if (sqlite3_exec(
                db,
                "PRAGMA foreign_keys = ON;",
                nullptr,
                nullptr,
                nullptr
            ) != SQLITE_OK)
            {
                throw std::runtime_error(
                    "Cannot enable foreign keys"
                );
            }
        };

    storage.sync_schema();

    std::cout << "\n=== TRANSACTION DEMONSTRATION ===\n";

    auto usersBefore = storage.count<User>();
    auto documentsBefore = storage.count<PrintDocument>();

    std::cout << "\nBefore transaction:\n";
    std::cout << "Users: " << usersBefore << '\n';
    std::cout << "PrintDocuments: " << documentsBefore << '\n';

    // Успішна транзакція: реєстрація користувача разом з першим документом
    bool committed = storage.transaction([&]()
        {
            User user{
                0,
                "Sofia Lysenko",
                "sofia.lysenko@example.com",
                "Design"
            };

            int userId = storage.insert(user);

            PrintDocument document{
                0,
                userId,
                "Welcome guide.pdf",
                "queued",
                12,
                3
            };

            storage.insert(document);

            std::cout << "\nRecords inserted inside transaction\n";
            std::cout << "User ID: " << userId << '\n';

            return true; // COMMIT
        });

    std::cout << "\nTransaction committed: "
        << std::boolalpha << committed << '\n';

    std::cout << "\nAfter transaction:\n";
    std::cout << "Users: "
        << storage.count<User>() << '\n';
    std::cout << "PrintDocuments: "
        << storage.count<PrintDocument>() << '\n';
}

void runRollbackDemo()
{
    auto storage = createStorage("print_queue_queries.db");

    storage.on_open = [](sqlite3* db)
        {
            if (sqlite3_exec(
                db,
                "PRAGMA foreign_keys = ON;",
                nullptr,
                nullptr,
                nullptr
            ) != SQLITE_OK)
            {
                throw std::runtime_error(
                    "Cannot enable foreign keys"
                );
            }
        };

    storage.sync_schema();

    std::cout << "\n=== ROLLBACK DEMONSTRATION ===\n";

    auto usersBefore = storage.count<User>();
    auto documentsBefore = storage.count<PrintDocument>();

    std::cout << "\nBefore transaction:\n";
    std::cout << "Users: " << usersBefore << '\n';
    std::cout << "PrintDocuments: " << documentsBefore << '\n';

    try
    {
        storage.transaction([&]()
            {
                // Перша операція - коректна
                User user{
                    0,
                    "Dmytro Kovalchuk",
                    "dmytro.kovalchuk@example.com",
                    "Finance"
                };

                int userId = storage.insert(user);

                std::cout << "\nUser inserted, ID: "
                    << userId << '\n';

                // Друга операція - навмисно некоректна
                PrintDocument document{
                    0,
                    -1,
                    "Budget plan.pdf",
                    "queued",
                    20,
                    4
                };

                storage.insert(document);

                return true;
            });
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nTransaction failed: "
            << ex.what() << '\n';

        std::cout << "Changes rolled back\n";
    }

    std::cout << "\nAfter transaction:\n";
    std::cout << "Users: "
        << storage.count<User>() << '\n';
    std::cout << "PrintDocuments: "
        << storage.count<PrintDocument>() << '\n';

    // Перевірка відсутності нового користувача
    using namespace sqlite_orm;

    auto foundUsers = storage.get_all<User>(
        where(c(&User::name) == "Dmytro Kovalchuk")
    );

    std::cout << "\nUser \"Dmytro Kovalchuk\" exists: "
        << std::boolalpha
        << !foundUsers.empty() << '\n';
}

void runWithoutTransactionDemo()
{
    const std::string databaseFile =
        "print_queue_no_transaction.db";

    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);

    storage.on_open = [](sqlite3* db)
        {
            if (sqlite3_exec(
                db,
                "PRAGMA foreign_keys = ON;",
                nullptr,
                nullptr,
                nullptr
            ) != SQLITE_OK)
            {
                throw std::runtime_error(
                    "Cannot enable foreign keys"
                );
            }
        };

    storage.sync_schema();

    std::cout << "\n=== WITHOUT TRANSACTION ===\n";

    try
    {
        User user{
            0,
            "Dmytro Kovalchuk",
            "dmytro.kovalchuk@example.com",
            "Finance"
        };

        int userId = storage.insert(user);

        std::cout << "\nUser inserted, ID: "
            << userId << '\n';

        // Навмисно некоректний зовнішній ключ
        PrintDocument document{
            0,
            -1,
            "Budget plan.pdf",
            "queued",
            20,
            4
        };

        storage.insert(document);
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nOperation failed: "
            << ex.what() << '\n';
    }

    std::cout << "\nAfter operations:\n";
    std::cout << "Users: "
        << storage.count<User>() << '\n';
    std::cout << "PrintDocuments: "
        << storage.count<PrintDocument>() << '\n';
}

int main()
{
    // Щоб повторний запуск не накопичував записи (вимога 7 завдання)
    std::filesystem::remove("print_queue.db");

    sqlite3* db = nullptr;

    int rc = sqlite3_open("print_queue.db", &db);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Cannot open database: "
            << sqlite3_errmsg(db) << '\n';

        sqlite3_close(db);
        return 1;
    }

    std::cout << "Database opened successfully\n";

    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS User (
            id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            name TEXT NOT NULL,
            email TEXT NOT NULL,
            department TEXT NOT NULL
        );
    )";

    char* errorMessage = nullptr;

    rc = sqlite3_exec(
        db,
        sql,
        nullptr,
        nullptr,
        &errorMessage
    );

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: "
            << (errorMessage ? errorMessage : "Unknown error")
            << '\n';

        sqlite3_free(errorMessage);
        sqlite3_close(db);
        return 1;
    }

    std::cout << "User table is ready\n";

    try
    {
        User user{
            0,
            "Olena Kvitka",
            "kvitka@gmail.com",
            "Accounting"
        };

        int id = insertUser(db, user);
        std::cout << "Inserted user ID: "
            << id << '\n';

        auto result = findUserById(db, id);

        if (result)
        {
            std::cout << "User found:\n";
            std::cout << "ID: " << result->id << '\n';
            std::cout << "Name: " << result->name << '\n';
            std::cout << "Email: "
                << result->email << '\n';
            std::cout << "Department: "
                << result->department << '\n';
        }
        else
        {
            std::cout << "User not found\n";
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Database error: "
            << ex.what() << '\n';
    }

    sqlite3_close(db);

    try
    {
        runOrmDemo();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "ORM error: "
            << ex.what() << '\n';
    }

    try
    {
        runRelationshipDemo();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Relationship demo error: "
            << ex.what() << '\n';
    }

    try
    {
        runRepositoryDemo();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Repository error: "
            << ex.what() << '\n';
    }

    try
    {
        prepareTestData();

        runQueryDemo();
        runAggregateDemo();
        runJoinDemo();

        runTransactionDemo();
        runRollbackDemo();
        runWithoutTransactionDemo();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Application error: "
            << ex.what() << '\n';
    }

    return 0;
}