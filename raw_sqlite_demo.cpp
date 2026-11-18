#include "database.hpp"
#include "scenarios.hpp"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>

namespace {
    // RAII-обгортки: ресурси звільняються навіть при винятку
    struct Db { sqlite3* p = nullptr;      ~Db() { sqlite3_close(p); } };
    struct Stmt { sqlite3_stmt* p = nullptr; ~Stmt() { sqlite3_finalize(p); } };

    void demoCAPI() {
        std::cout << "[SQLite C API]\n";
        Db db;
        if (sqlite3_open("surveillance.db", &db.p) != SQLITE_OK)
            throw std::runtime_error("Cannot open SQLite DB");

        int id = 0;
        {
            Stmt st;
            const char* sql =
                "INSERT INTO cameras(name,location,ip_address,status) VALUES(?,?,?,?);";
            if (sqlite3_prepare_v2(db.p, sql, -1, &st.p, nullptr) != SQLITE_OK)
                throw std::runtime_error(sqlite3_errmsg(db.p));
            sqlite3_bind_text(st.p, 1, "C API test camera", -1, SQLITE_STATIC);
            sqlite3_bind_text(st.p, 2, "Test location", -1, SQLITE_STATIC);
            sqlite3_bind_text(st.p, 3, "192.168.1.190", -1, SQLITE_STATIC);
            sqlite3_bind_text(st.p, 4, "active", -1, SQLITE_STATIC);
            if (sqlite3_step(st.p) != SQLITE_DONE)
                throw std::runtime_error(sqlite3_errmsg(db.p));
            id = static_cast<int>(sqlite3_last_insert_rowid(db.p));
        }
        {
            Stmt st;
            if (sqlite3_prepare_v2(db.p, "SELECT id,name FROM cameras WHERE id=?;",
                -1, &st.p, nullptr) != SQLITE_OK)
                throw std::runtime_error(sqlite3_errmsg(db.p));
            sqlite3_bind_int(st.p, 1, id);
            if (sqlite3_step(st.p) == SQLITE_ROW) {
                const unsigned char* name = sqlite3_column_text(st.p, 1);
                std::cout << "  C API read: " << sqlite3_column_int(st.p, 0) << " "
                    << (name ? reinterpret_cast<const char*>(name) : "") << "\n";
            }
        }
        {   // прибираємо за собою, щоб дані не накопичувались
            Stmt st;
            sqlite3_prepare_v2(db.p, "DELETE FROM cameras WHERE id=?;", -1, &st.p, nullptr);
            sqlite3_bind_int(st.p, 1, id);
            sqlite3_step(st.p);
        }
    }

    void demoORM() {
        std::cout << "[sqlite_orm]\n";
        Camera cam{ 0, "ORM test camera", "Test location", "192.168.1.191", "active" };
        const int id = static_cast<int>(getStorage().insert(cam));
        auto got = getStorage().get_optional<Camera>(id);
        if (got) std::cout << "  ORM read: " << got->id << " " << got->name << "\n";
        getStorage().remove<Camera>(id);
    }
}

void runCompareDemo() {
    std::cout << "\n=== SQLITE C API vs ORM ===\n";
    demoCAPI();
    demoORM();
}