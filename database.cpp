#include "database.hpp"
#include <vector>

Storage& getStorage() {
    static Storage s = createStorage("surveillance.db");
    return s;
}

void initializeDatabase() {
    auto& s = getStorage();
    s.open_forever();        // одне з'єднання на весь час роботи програми
    s.sync_schema();         // створення таблиць; перевірка FK вмикається автоматично,
    // бо в схемі є foreign_key(...)
}

void seedDatabase() {
    auto& s = getStorage();

    s.transaction([&] {
        s.remove_all<RegisteredEvent>();   // спочатку дочірня таблиця
        s.remove_all<Camera>();

        const std::vector<Camera> cams = {
            { 0, "Entrance camera",  "Main entrance",   "192.168.1.101", "active" },
            { 0, "Parking camera",   "Parking area",    "192.168.1.102", "active" },
            { 0, "Warehouse camera", "Warehouse",       "192.168.1.103", "active" },
            { 0, "Office camera",    "Office floor",    "192.168.1.104", "active" },
            { 0, "Perimeter camera", "North perimeter", "192.168.1.105", "maintenance" }
        };

        std::vector<int> id;                       // реальні id камер
        for (const auto& c : cams)
            id.push_back(static_cast<int>(s.insert(c)));

        // у кожній події: індекс камери (0..4) замість жорсткого id
        struct Ev { int cam; const char* type; int sev; const char* time; const char* desc; };
        const std::vector<Ev> ev = {
            { 0, "motion",    2, "2026-11-17 08:10:00", "Movement near entrance" },
            { 0, "person",    3, "2026-11-17 08:15:00", "Person detected" },
            { 0, "intrusion", 5, "2026-11-17 08:20:00", "Restricted zone entry" },
            { 1, "vehicle",   2, "2026-11-17 09:00:00", "Vehicle entered parking" },
            { 1, "motion",    1, "2026-11-17 09:05:00", "Movement in parking" },
            { 1, "person",    3, "2026-11-17 09:10:00", "Person near cars" },
            { 2, "motion",    2, "2026-11-17 10:00:00", "Movement in warehouse" },
            { 2, "intrusion", 4, "2026-11-17 10:15:00", "Unexpected warehouse access" },
            { 2, "person",    2, "2026-11-17 10:20:00", "Worker detected" },
            { 3, "person",    1, "2026-11-17 11:00:00", "Employee entered office" },
            { 3, "motion",    1, "2026-11-17 11:30:00", "Movement in office" },
            { 3, "object",    2, "2026-11-17 12:00:00", "Object in corridor" },
            { 4, "motion",    2, "2026-11-17 12:30:00", "Perimeter movement" },
            { 4, "intrusion", 5, "2026-11-17 12:45:00", "Perimeter breach" },
            { 4, "vehicle",   3, "2026-11-17 13:00:00", "Vehicle near perimeter" }
        };

        for (const auto& e : ev)
            s.insert(RegisteredEvent{ 0, id[e.cam], e.type, e.sev, e.time, e.desc });

        return true;
        });
}