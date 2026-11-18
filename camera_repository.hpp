#pragma once
#include "models.hpp"
#include <optional>
#include <vector>
class ICameraRepository {
public:
 virtual ~ICameraRepository()=default;
 virtual int add(const Camera&)=0;
 virtual std::optional<Camera> getById(int)=0;
 virtual std::vector<Camera> getAll()=0;
 virtual bool update(const Camera&)=0;
 virtual bool remove(int)=0;
};
class CameraRepository final: public ICameraRepository {
public:
 int add(const Camera&) override;
 std::optional<Camera> getById(int) override;
 std::vector<Camera> getAll() override;
 bool update(const Camera&) override;
 bool remove(int) override;
};
