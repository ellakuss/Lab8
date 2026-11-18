#pragma once
#include "models.hpp"
#include <tuple>
#include <utility>
#include <vector>
class EventRepository {
public:
 int add(const RegisteredEvent&);
 std::vector<RegisteredEvent> getAll();
 std::vector<RegisteredEvent> getBySeverity(int);
 std::vector<RegisteredEvent> getRecentFirst(int);
 int countAll();
 std::vector<std::tuple<std::string,int>> countByCamera();
 std::vector<std::tuple<std::string,std::string,int,std::string>> joinCameraEvents();
};
