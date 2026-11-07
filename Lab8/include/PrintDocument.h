#pragma once

#include <string>

struct PrintDocument
{
    int id = 0;
    int userId = 0;

    std::string documentName;
    std::string status;

    int pages = 0;
    int priority = 1;
};