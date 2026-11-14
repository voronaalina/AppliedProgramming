#include <iostream> 
#include <string> 
#include <vector> 
class Book {
public:
    std::string title;
    std::string author;
    void display() {
        std::cout << "Title: " << title
            << ", Author: " << author << std::endl;
    }

    void saveToDatabase() {
        std::cout << "Saving book to database..." << std::endl;
    }

    void sendEmailNotification() {
        std::cout << "Sending email notification about the new book..."
            << std::endl;
    }
};

class Library {
public:
    void addBook(const Book& book) {
        books.push_back(book);
        book.saveToDatabase();
        book.sendEmailNotification();
    }

    void displayBooks() {
        for (const auto& book : books) {
            book.display();
        }
    }

private:
    std::vector<Book> books;
};

int main() {
    Library library;

    Book book1 = { "The Catcher in the Rye", "J.D. Salinger" };
    Book book2 = { "To Kill a Mockingbird", "Harper Lee" };

    library.addBook(book1);
    library.addBook(book2);

    library.displayBooks();

    return 0;
}