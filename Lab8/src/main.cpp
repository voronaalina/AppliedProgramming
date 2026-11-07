#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include <sqlite3.h>

#include "Database.h"
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

int main()
{
    // Щоб повторний запуск не накопичував записи 
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

    return 0;
}