#include "database.hpp"
#include "camera_repository.hpp"
#include "event_repository.hpp"
#include "scenarios.hpp"
#include <sqlite_orm/sqlite_orm.h>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

namespace {
    struct Snapshot { int cameras; int events; };

    Snapshot snapshot(Storage& s) {
        return { static_cast<int>(s.count<Camera>()),
                 static_cast<int>(s.count<RegisteredEvent>()) };
    }

    void show(const char* label, const Snapshot& x) {
        std::cout << "  " << label << ": cameras=" << x.cameras
            << ", events=" << x.events << "\n";
    }

    int countCamerasNamed(Storage& s, const std::string& name) {
        return static_cast<int>(
            s.count<Camera>(sqlite_orm::where(sqlite_orm::c(&Camera::name) == name)));
    }
}

// ------------------------------------------------------------ Завдання 3
void runSchemaDemo() {
    auto& s = getStorage();
    std::cout << "\n=== SCHEMA / FOREIGN KEY ===\n";

    for (const auto& table : s.table_names()) {
        if (table.rfind("sqlite_", 0) == 0) continue;   // службові таблиці SQLite
        std::cout << "Table " << table << ": "
            << (s.table_exists(table) ? "exists" : "MISSING") << "\n";
    }

    // Перевірка посилальної цілісності: подія з неіснуючою камерою
    std::cout << "FK check: insert event with camera_id=999999\n";
    try {
        s.insert(RegisteredEvent{ 0, 999999, "motion", 1, "2026-11-17 00:00:00", "FK test" });
        std::cout << "  WARNING: no error, foreign keys are OFF\n";
    }
    catch (const std::exception& e) {
        std::cout << "  Expected database error: " << e.what() << "\n";
    }
}

// ------------------------------------------------------------ Завдання 4
void runCrudDemo() {
    std::cout << "\n=== REPOSITORY / CRUD ===\n";
    std::unique_ptr<ICameraRepository> repo = std::make_unique<CameraRepository>();

    const int id = repo->add({ 0, "Temporary camera", "Lab", "192.168.1.199", "active" });
    std::cout << "CREATE id=" << id << "\n";

    auto cam = repo->getById(id);
    std::cout << "READ " << (cam ? cam->name : "not found") << "\n";

    if (cam) {
        cam->status = "maintenance";
        std::cout << "UPDATE " << repo->update(*cam) << "\n";
        auto after = repo->getById(id);
        if (after) std::cout << "READ after update: status=" << after->status << "\n";
    }

    std::cout << "READ missing id=99999: "
        << (repo->getById(99999) ? "found" : "not found") << "\n";
    std::cout << "DELETE " << repo->remove(id) << "\n";
    std::cout << "READ after delete: "
        << (repo->getById(id) ? "still exists" : "not found") << "\n";
}

// ------------------------------------------------------------ Завдання 5
void runQueryDemo() {
    EventRepository r;
    std::cout << "\n=== QUERIES ===\n";

    std::cout << "FILTER severity >= 4\n";
    for (auto& e : r.getBySeverity(4))
        std::cout << "  " << e.eventType << " severity=" << e.severity << "\n";

    std::cout << "SORT + LIMIT: latest 5\n";
    for (auto& e : r.getRecentFirst(5))
        std::cout << "  " << e.eventTime << " " << e.eventType << "\n";

    std::cout << "COUNT events=" << r.countAll() << "\n";

    std::cout << "GROUP BY camera\n";
    for (auto& [name, n] : r.countByCamera())
        std::cout << "  " << name << ": " << n << "\n";

    std::cout << "INNER JOIN camera/event\n";
    for (auto& [name, type, sev, time] : r.joinCameraEvents())
        std::cout << "  " << name << " | " << type << " | " << sev << " | " << time << "\n";
}

// ------------------------------------------------------------ Завдання 6
void runTransactionDemo() {
    auto& s = getStorage();
    std::cout << "\n=== TRANSACTIONS ===\n";

    // A. Успішна транзакція (COMMIT)
    std::cout << "[A] COMMIT\n";
    show("before", snapshot(s));
    const bool committed = s.transaction([&] {
        const int id = static_cast<int>(
            s.insert(Camera{ 0, "Tx OK camera", "Gate", "192.168.1.180", "active" }));
        s.insert(RegisteredEvent{ 0, id, "motion", 2, "2026-11-17 14:00:00", "Initial event" });
        return true;
        });
    show("after ", snapshot(s));
    std::cout << "  committed=" << (committed ? "true" : "false") << "\n";

    // B. Помилка у другій операції: виняток -> ROLLBACK
    std::cout << "[B] ROLLBACK (exception from 2nd operation)\n";
    const Snapshot beforeB = snapshot(s);
    show("before", beforeB);
    try {
        s.transaction([&] {
            s.insert(Camera{ 0, "Tx fail camera", "Roof", "192.168.1.181", "active" });
            s.insert(RegisteredEvent{ 0, 999999, "intrusion", 5,
                                      "2026-11-17 14:05:00", "Intentional FK violation" });
            return true;
            });
        std::cout << "  WARNING: no exception (foreign keys are OFF?)\n";
    }
    catch (const std::exception& e) {
        std::cout << "  Expected database error: " << e.what() << "\n";
    }
    const Snapshot afterB = snapshot(s);
    show("after ", afterB);
    const int leftoverB = countCamerasNamed(s, "Tx fail camera");
    std::cout << "  'Tx fail camera' in DB: " << leftoverB
        << ((leftoverB == 0 && beforeB.cameras == afterB.cameras &&
            beforeB.events == afterB.events)
            ? "  -> ROLLBACK verified: YES\n"
            : "  -> ROLLBACK verified: NO\n");

    // C. Те саме БЕЗ транзакції: часткове збереження
    std::cout << "[C] Without transaction\n";
    show("before", snapshot(s));
    try {
        s.insert(Camera{ 0, "No-Tx camera", "Basement", "192.168.1.182", "active" });
        s.insert(RegisteredEvent{ 0, 999999, "intrusion", 5,
                                  "2026-11-17 14:10:00", "Intentional FK violation" });
    }
    catch (const std::exception& e) {
        std::cout << "  Expected database error: " << e.what() << "\n";
    }
    show("after ", snapshot(s));
    std::cout << "  'No-Tx camera' in DB: " << countCamerasNamed(s, "No-Tx camera")
        << "  -> partial save (camera without event)\n";

    // D. Відкат через return false
    std::cout << "[D] ROLLBACK (lambda returns false)\n";
    const Snapshot beforeD = snapshot(s);
    show("before", beforeD);
    const bool committedD = s.transaction([&] {
        s.insert(Camera{ 0, "Rollback false camera", "Test", "192.168.1.183", "active" });
        return false;
        });
    const Snapshot afterD = snapshot(s);
    show("after ", afterD);
    std::cout << "  committed=" << (committedD ? "true" : "false")
        << ", ROLLBACK verified: "
        << (beforeD.cameras == afterD.cameras ? "YES" : "NO") << "\n";
}