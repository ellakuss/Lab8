
#pragma once

#include <string>
#include <sqlite_orm/sqlite_orm.h>
#include "models.hpp"

using namespace sqlite_orm;

inline auto createStorage(const std::string& file) {
    return make_storage(file,
        make_table("cameras",
            make_column("id", &Camera::id, primary_key().autoincrement()),
            make_column("name", &Camera::name),
            make_column("location", &Camera::location),
            make_column("ip_address", &Camera::ipAddress),
            make_column("status", &Camera::status)
        ),
        make_table("registered_events",
            make_column("id", &RegisteredEvent::id, primary_key().autoincrement()),
            make_column("camera_id", &RegisteredEvent::cameraId),
            make_column("event_type", &RegisteredEvent::eventType),
            make_column("severity", &RegisteredEvent::severity),
            make_column("event_time", &RegisteredEvent::eventTime),
            make_column("description", &RegisteredEvent::description),
            foreign_key(&RegisteredEvent::cameraId).references(&Camera::id)
        )
    );
}

using Storage = decltype(createStorage(""));
Storage& getStorage();

void initializeDatabase();
void seedDatabase();
