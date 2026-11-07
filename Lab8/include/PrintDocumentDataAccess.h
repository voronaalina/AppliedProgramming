#pragma once

#include <optional>
#include <string>
#include <vector>

#include "PrintDocument.h"
#include "Database.h"

// Окремий компонент доступу до даних залежної сутності PrintDocument.
// Додатковий абстрактний інтерфейс для нього не створюється.
class PrintDocumentDataAccess
{
public:
    explicit PrintDocumentDataAccess(
        const std::string& databaseFile
    );

    int add(const PrintDocument& document);

    std::optional<PrintDocument> findById(int id);

    std::vector<PrintDocument> findByUserId(int userId);

    bool updateStatus(
        int id,
        const std::string& status
    );

    bool remove(int id);

private:
    using Storage = decltype(createStorage(""));

    Storage storage_;
};