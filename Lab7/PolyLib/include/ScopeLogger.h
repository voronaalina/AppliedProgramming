#pragma once

#include <iostream>
#include <string>
#include <utility>

// RAII-об'єкт: повідомляє про своє створення і знищення.
class ScopeLogger
{
private:
    std::string name_;

public:
    explicit ScopeLogger(std::string name)
        : name_(std::move(name))
    {
        std::cout << "[CREATE]  " << name_ << '\n';
    }

    ~ScopeLogger() noexcept
    {
        std::cout << "[DESTROY] " << name_ << '\n';
    }

    ScopeLogger(const ScopeLogger&) = delete;
    ScopeLogger& operator=(const ScopeLogger&) = delete;
};