#pragma once
#include <string>
struct Camera {
 int id=0; std::string name, location, ipAddress, status;
};
struct RegisteredEvent {
 int id=0; int cameraId=0; std::string eventType;
 int severity=0; std::string eventTime, description;
};
