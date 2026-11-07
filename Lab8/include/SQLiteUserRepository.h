#pragma once

#include <string>

#include "IUserRepository.h"
#include "Database.h"

class SQLiteUserRepository
    : public IUserRepository
{
public:
    explicit SQLiteUserRepository(
        const std::string& databaseFile
    );

    int add(const User& user) override;

    std::optional<User> findById(int id) override;

    std::vector<User> findAll() override;

    bool update(const User& user) override;

    bool remove(int id) override;

private:
    using Storage = decltype(createStorage(""));

    Storage storage_;
};