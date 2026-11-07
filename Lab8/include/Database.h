#pragma once

#include <string>
#include <sqlite_orm/sqlite_orm.h>

#include "User.h"
#include "PrintDocument.h"

inline auto createStorage(const std::string& filename)
{
    using namespace sqlite_orm;

    return make_storage(
        filename,

        make_table(
            "User",

            make_column(
                "id",
                &User::id,
                primary_key().autoincrement()
            ),

            make_column(
                "name",
                &User::name,
                not_null()
            ),

            make_column(
                "email",
                &User::email,
                not_null()
            ),

            make_column(
                "department",
                &User::department,
                not_null()
            )
        ),

        // Таблиця документів друку
        make_table(
            "PrintDocument",

            make_column(
                "id",
                &PrintDocument::id,
                primary_key().autoincrement()
            ),

            make_column(
                "userId",
                &PrintDocument::userId
            ),

            make_column(
                "documentName",
                &PrintDocument::documentName,
                not_null()
            ),

            make_column(
                "status",
                &PrintDocument::status,
                not_null()
            ),

            make_column(
                "pages",
                &PrintDocument::pages
            ),

            make_column(
                "priority",
                &PrintDocument::priority
            ),

            foreign_key(
                &PrintDocument::userId
            ).references(
                &User::id
            )
        )
    );
}

