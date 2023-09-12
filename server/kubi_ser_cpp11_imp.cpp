#include"kubi_ser_cpp11_imp.h"

void Kubi_Ser_Cpp11_Imp::_handle_server_stage(void *target) {
	Kubi_Ser_Cpp11_Imp* ser_imp = (Kubi_Ser_Cpp11_Imp*)target;
	if (ser_imp->_if_inl) {
		ser_imp->_fun_server_close();
	}
	else {
		ser_imp->thread_add_ser_pool();
	}
}

void Kubi_Ser_Cpp11_Imp::_handle_client_enter(void *target, const std::string &uid, const std::string &ip, int port) {
	Kubi_Ser_Cpp11_Imp* ser_imp = (Kubi_Ser_Cpp11_Imp*)target;
	if (ser_imp->_if_inl) {
		ser_imp->_fun_client_enter(uid, ip, port);
	}
	else {
		ser_imp->thread_add_client_enter(uid, ip, port);
	}
}
void Kubi_Ser_Cpp11_Imp::_handle_client_exite(void *target, const std::string &uid) {
	Kubi_Ser_Cpp11_Imp* ser_imp = (Kubi_Ser_Cpp11_Imp*)target;
	if (ser_imp->_if_inl) {
		ser_imp->_fun_client_leave(uid);
	}
	else {
		ser_imp->thread_add_client_leave(uid);
	}
	
}
void Kubi_Ser_Cpp11_Imp::_handle_request(void *target, const std::string &uid, const char *route, const kubi::KNode  *in_t) {
	Kubi_Ser_Cpp11_Imp* ser_imp = (Kubi_Ser_Cpp11_Imp*)target;
	if (ser_imp->_if_inl) {
		std::map<std::string, RC_REQUEST_FUN>::iterator it = ser_imp->_route_request_pools.find(route);
		if (it != ser_imp->_route_request_pools.end()) {
			it->second(uid,  in_t->pkg_id() ,in_t->data_str(), in_t->data_bytes());
			delete in_t;
		}
	}
	else {
		ser_imp->thread_add_reqs(uid, route, in_t);
	}
	
}
void Kubi_Ser_Cpp11_Imp::_handle_notify(void *target, const std::string &uid, const char *route, const kubi::KNode* in_t) {
	Kubi_Ser_Cpp11_Imp* ser_imp = (Kubi_Ser_Cpp11_Imp*)target;
	if (ser_imp->_if_inl) {
		std::map<std::string, RC_NOTIFY_FUN>::iterator it = ser_imp->_route_notify_pools.find(route);
		if (it != ser_imp->_route_notify_pools.end()) {
			it->second(uid, in_t->data_str(), in_t->data_bytes());
			delete in_t;
		}
	}
	else {
		ser_imp->thread_add_notify(uid, route, in_t);
	}

}
 void Kubi_Ser_Cpp11_Imp::_handle_inline_ctrl(void* target, const std::string& signal, const std::string& msg) {
	 Kubi_Ser_Cpp11_Imp* ser_imp = (Kubi_Ser_Cpp11_Imp*)target;
	if (ser_imp->_if_inl) {
		std::map<std::string, RC_INLINE_CTRL>::iterator it = ser_imp->_route_inline_ctrl_pools.find(signal);
		if (it != ser_imp->_route_inline_ctrl_pools.end()) {
			it->second( msg);
		}
	}
	else {
		ser_imp->thread_add_inline_ctrl(signal, msg);
	}
}
