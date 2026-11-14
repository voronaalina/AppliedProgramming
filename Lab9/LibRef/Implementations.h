#pragma once
#include <iostream>
#include <vector>
#include "Book.h"
#include "Interfaces.h"

class DatabaseBookRepository : public IBookRepository {
public:
    void save(const Book&) override {
        std::cout << "Saving book to database..." << std::endl;
    }
};

class EmailNotifier : public INotifier {
public:
    void notify(const Book&) override {
        std::cout << "Sending email notification about the new book..."
            << std::endl;
    }
};

class BookPrinter {
public:
    void print(const Book& book) const {
        std::cout << "Title: " << book.getTitle()
            << ", Author: " << book.getAuthor() << std::endl;
    }

    void printAll(const std::vector<Book>& books) const {
        for (const auto& book : books) {
            print(book);
        }
    }
};