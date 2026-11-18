#include "event_repository.hpp"
#include "database.hpp"
int EventRepository::add(const RegisteredEvent& e){return (int)getStorage().insert(e);}
std::vector<RegisteredEvent> EventRepository::getAll(){return getStorage().get_all<RegisteredEvent>();}
std::vector<RegisteredEvent> EventRepository::getBySeverity(int n){
 return getStorage().get_all<RegisteredEvent>(sqlite_orm::where(sqlite_orm::c(&RegisteredEvent::severity)>=n),
 sqlite_orm::order_by(&RegisteredEvent::severity).desc());
}
std::vector<RegisteredEvent> EventRepository::getRecentFirst(int n){
 return getStorage().get_all<RegisteredEvent>(sqlite_orm::order_by(&RegisteredEvent::eventTime).desc(),sqlite_orm::limit(n));
}
int EventRepository::countAll(){return (int)getStorage().count<RegisteredEvent>();}
std::vector<std::tuple<std::string,int>> EventRepository::countByCamera(){
 return getStorage().select(sqlite_orm::columns(&Camera::name,sqlite_orm::count(&RegisteredEvent::id)),
 sqlite_orm::inner_join<RegisteredEvent>(sqlite_orm::on(sqlite_orm::c(&Camera::id)==&RegisteredEvent::cameraId)),
 sqlite_orm::group_by(&Camera::id));
}
std::vector<std::tuple<std::string,std::string,int,std::string>> EventRepository::joinCameraEvents(){
 return getStorage().select(sqlite_orm::columns(&Camera::name,&RegisteredEvent::eventType,&RegisteredEvent::severity,&RegisteredEvent::eventTime),
 sqlite_orm::inner_join<RegisteredEvent>(sqlite_orm::on(sqlite_orm::c(&Camera::id)==&RegisteredEvent::cameraId)),
 sqlite_orm::order_by(&RegisteredEvent::eventTime).desc());
}
