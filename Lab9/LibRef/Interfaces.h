#pragma once
#include "Book.h"

//сховище книг
class IBookRepository {
public:
    virtual void save(const Book& book) = 0;
    virtual ~IBookRepository() = default;
};

//сповіщення
class INotifier {
public:
    virtual void notify(const Book& book) = 0;
    virtual ~INotifier() = default;
};