#pragma once
#include <string>
#include <utility>

class Book {
public:
    Book(std::string title, std::string author)
        : title_(std::move(title)), author_(std::move(author)) {
    }

    const std::string& getTitle() const { return title_; }
    const std::string& getAuthor() const { return author_; }

private:
    std::string title_;
    std::string author_;
};