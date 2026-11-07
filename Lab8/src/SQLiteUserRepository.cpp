#include "SQLiteUserRepository.h"

#include <stdexcept>

SQLiteUserRepository::SQLiteUserRepository(
    const std::string& databaseFile
)
    : storage_(createStorage(databaseFile))
{
    storage_.on_open = [](sqlite3* db)
        {
            int rc = sqlite3_exec(
                db,
                "PRAGMA foreign_keys = ON;",
                nullptr,
                nullptr,
                nullptr
            );

            if (rc != SQLITE_OK)
            {
                throw std::runtime_error(
                    "Cannot enable foreign key constraints"
                );
            }
        };

    storage_.sync_schema();
}

int SQLiteUserRepository::add(
    const User& user
)
{
    return storage_.insert(user);
}

std::optional<User>
SQLiteUserRepository::findById(int id)
{
    return storage_.get_optional<User>(id);
}

std::vector<User>
SQLiteUserRepository::findAll()
{
    return storage_.get_all<User>();
}

bool SQLiteUserRepository::update(
    const User& user
)
{
    auto existing =
        storage_.get_optional<User>(user.id);

    if (!existing)
    {
        return false;
    }

    storage_.update(user);

    return true;
}

bool SQLiteUserRepository::remove(int id)
{
    auto existing =
        storage_.get_optional<User>(id);

    if (!existing)
    {
        return false;
    }

    storage_.remove<User>(id);
    return true;
}