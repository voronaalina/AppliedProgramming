#pragma once
#include <vector>
#include "Book.h"
#include "Interfaces.h"

class Library {
public:
    Library(IBookRepository& repository, INotifier& notifier)
        : repository_(repository), notifier_(notifier) {
    }

    void addBook(const Book& book) {
        books_.push_back(book);
        repository_.save(book);
        notifier_.notify(book);
    }

    const std::vector<Book>& getBooks() const { return books_; }

private:
    std::vector<Book> books_;
    IBookRepository& repository_;
    INotifier& notifier_;
};