#include "PrintDocumentDataAccess.h"

#include <stdexcept>

PrintDocumentDataAccess::PrintDocumentDataAccess(
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

int PrintDocumentDataAccess::add(
    const PrintDocument& document
)
{
    return storage_.insert(document);
}

std::optional<PrintDocument>
PrintDocumentDataAccess::findById(int id)
{
    return storage_.get_optional<PrintDocument>(id);
}

std::vector<PrintDocument>
PrintDocumentDataAccess::findByUserId(int userId)
{
    using namespace sqlite_orm;

    return storage_.get_all<PrintDocument>(
        where(
            c(&PrintDocument::userId) == userId
        )
    );
}

bool PrintDocumentDataAccess::updateStatus(
    int id,
    const std::string& status
)
{
    auto existing =
        storage_.get_optional<PrintDocument>(id);

    if (!existing)
    {
        return false;
    }

    existing->status = status;
    storage_.update(*existing);

    return true;
}

bool PrintDocumentDataAccess::remove(int id)
{
    auto existing =
        storage_.get_optional<PrintDocument>(id);

    if (!existing)
    {
        return false;
    }

    storage_.remove<PrintDocument>(id);
    return true;
}