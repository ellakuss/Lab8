#include "camera_repository.hpp"
#include "database.hpp"
int CameraRepository::add(const Camera& c){return (int)getStorage().insert(c);}
std::optional<Camera> CameraRepository::getById(int id){return getStorage().get_optional<Camera>(id);}
std::vector<Camera> CameraRepository::getAll(){return getStorage().get_all<Camera>(sqlite_orm::order_by(&Camera::id));}
bool CameraRepository::update(const Camera& c){if(!getById(c.id)) return false; getStorage().update(c); return true;}
bool CameraRepository::remove(int id){
 if(!getById(id)) return false;
 getStorage().remove_all<RegisteredEvent>(sqlite_orm::where(sqlite_orm::c(&RegisteredEvent::cameraId)==id));
 getStorage().remove<Camera>(id); return true;
}
