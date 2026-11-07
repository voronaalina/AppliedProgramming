#pragma once

#include <optional>
#include <vector>

#include "User.h"

class IUserRepository
{
public:
    virtual ~IUserRepository() = default;

    // Create
    virtual int add(const User& user) = 0;

    // Read
    virtual std::optional<User> findById(int id) = 0;

    virtual std::vector<User> findAll() = 0;

    // Update
    virtual bool update(const User& user) = 0;

    // Delete
    virtual bool remove(int id) = 0;
};