#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>
#include <chrono>
#include <vector>

struct Document
{
    int id=0;
    std::string title;
    int pages=0;
};

std::deque<Document> printQueue;
std::mutex queueMutex;
std::mutex outputMutex;
std::condition_variable condition;
bool finished = false;

const int DOCUMENT_COUNT = 12;

// Потік-виробник
void producer()
{
    const std::string titles[] =
    {
        "Report",
        "Invoice",
        "Contract",
        "Presentation"
    };

    for (int i = 1; i <= DOCUMENT_COUNT; ++i)
    {
        Document doc
        {
            i,
            titles[(i - 1) % 4],
            i % 5 + 1
        };

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            printQueue.push_back(doc);
        }

        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "[Producer] Added document "
                << doc.id << ", title: " << doc.title
                << ", pages: " << doc.pages << '\n';
        }

        /*condition.notify_one();*/
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        finished = true;
    }
    condition.notify_all();

    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Producer] Generation finished\n";
    }
}

// принтер
void consumer(int consumerId)
{
    while (true)
    {
        Document doc;
        std::string queueState = "";
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, []()
                {
                    return !printQueue.empty() || finished;
                });

            if (printQueue.empty() && finished)
            {
                break;
            }

            doc = printQueue.front();
            printQueue.pop_front();

            for (const Document& d : printQueue)
            {
                queueState += std::to_string(d.id) + " ";
            }
        }

        // "Друк", затримка залежить від сторінок
        std::this_thread::sleep_for(
            std::chrono::milliseconds(doc.pages * 150));

        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "[Printer " << consumerId
                << "] Printed document " << doc.id
                << ", title: " << doc.title
                << ", pages: " << doc.pages
                << ". Queue: [ " << queueState << "]\n";
        }
    }

    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Printer " << consumerId << "] Finished\n";
    }
}

int main()
{  
    auto start = std::chrono::steady_clock::now();

    std::thread producerThread(producer);
    std::thread consumer1(consumer, 1);
    std::thread consumer2(consumer, 2);

    producerThread.join();
    consumer1.join();
    consumer2.join();

    auto end = std::chrono::steady_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "\nAll documents have been printed.\n";
    std::cout << "Total time: " << timeMs << " ms\n";
    return 0;
}