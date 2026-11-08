#include "TestData.h"
#include "Database.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

void prepareTestData()
{
    const std::string databaseFile = "print_queue_queries.db";

    // Видаляємо лише окрему демонстраційну БД.
    // Файл print_queue.db залишається без змін.
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

    storage.open_forever();
    storage.sync_schema();

    std::vector<User> userList = {
        {0, "Olena Kovalenko", "olena.kovalenko@example.com", "Accounting"},
        {0, "Andrii Melnyk",   "andrii.melnyk@example.com",   "IT"},
        {0, "Iryna Bondar",    "iryna.bondar@example.com",    "Marketing"},
        {0, "Taras Shevchuk",  "taras.shevchuk@example.com",  "HR"},
        {0, "Maria Tkachenko", "maria.tkachenko@example.com", "Legal"}
    };

    std::vector<int> userIds;

    for (const auto& user : userList)
    {
        userIds.push_back(storage.insert(user));
    }

    std::vector<PrintDocument> documentList = {
        {0, userIds[0], "Annual report.pdf",      "done",     120, 3},
        {0, userIds[0], "Invoice 2026-10.pdf",    "queued",     2, 5},
        {0, userIds[0], "Balance sheet.xlsx",     "printing",  14, 4},

        {0, userIds[1], "Network diagram.pdf",    "done",       6, 2},
        {0, userIds[1], "Server manual.pdf",      "queued",    85, 1},
        {0, userIds[1], "Access policy.docx",     "failed",    12, 3},
        {0, userIds[1], "Backup schedule.pdf",    "queued",     3, 4},

        {0, userIds[2], "Campaign brochure.pdf",  "done",      24, 2},
        {0, userIds[2], "Poster A3.pdf",          "queued",     1, 3},
        {0, userIds[2], "Market analysis.pdf",    "printing",  40, 2},

        {0, userIds[3], "Employment contract.pdf","queued",     8, 2},
        {0, userIds[3], "Vacation plan.xlsx",     "done",       4, 1},

        {0, userIds[4], "License agreement.pdf",  "printing",  30, 4},
        {0, userIds[4], "Court claim.pdf",        "failed",    18, 5},
        {0, userIds[4], "Meeting notes.docx",     "done",       5, 1}
    };

    for (const auto& document : documentList)
    {
        storage.insert(document);
    }

    std::cout << "Test database prepared\n";
    std::cout << "User records: "
        << storage.count<User>() << '\n';
    std::cout << "PrintDocument records: "
        << storage.count<PrintDocument>() << '\n';
}