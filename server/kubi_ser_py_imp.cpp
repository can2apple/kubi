#include"kubi_ser_py_imp.h"

//void Kubi_Ser_Py_Imp::_handle_server_stage(void *target) {
//	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
//	ser_imp->thread_add_ser_pool();
//}
//void Kubi_Ser_Py_Imp::_handle_client_enter(void *target, const std::string &uid, const std::string &ip, int port) {
//	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
//	ser_imp->thread_add_client_enter(uid,ip,port);
//}
//void Kubi_Ser_Py_Imp::_handle_client_exite(void *target, const std::string &uid) {
//	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
//	ser_imp->thread_add_client_leave(uid);
//}
//void Kubi_Ser_Py_Imp::_handle_request(void *target, const std::string &uid, const char *route, const kubi::KNode  *in_t,  kubi::KNode *out_t) {
//	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
//	ser_imp->thread_add_reqs(uid, route, in_t, out_t);
//}
//void Kubi_Ser_Py_Imp::_handle_notify(void *target, const std::string &uid, const char *route, const kubi::KNode *in_t) {
//	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
//	ser_imp->thread_add_notify(uid, route,in_t);
//}
//
void Kubi_Ser_Py_Imp::_handle_server_stage(void *target) {
	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
	if (ser_imp->_if_inl) {
		ser_imp->_fun_server_close();
	}
	else {
		ser_imp->thread_add_ser_pool();
	}

}
void Kubi_Ser_Py_Imp::_handle_client_enter(void *target, const std::string &uid, const std::string &ip, int port) {
	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
	if (ser_imp->_if_inl) {
		ser_imp->_fun_client_enter(uid, ip, port);
	}
	else {
		ser_imp->thread_add_client_enter(uid, ip, port);
	}
}
void Kubi_Ser_Py_Imp::_handle_client_exite(void *target, const std::string &uid) {
	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
	if (ser_imp->_if_inl) {
		ser_imp->_fun_client_leave(uid);
	}
	else {
		ser_imp->thread_add_client_leave(uid);
	}

}
void Kubi_Ser_Py_Imp::_handle_request(void *target, const std::string &uid, const char *route, const kubi::KNode  *in_t) {
	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
	if (ser_imp->_if_inl) {
		std::map<std::string, RC_REQUEST_FUN>::iterator it = ser_imp->_route_request_pools.find(route);
		if (it != ser_imp->_route_request_pools.end()) {
			py::bytes byte_t(in_t->data_bytes());
			it->second(uid,  in_t->pkg_id(), in_t->data_str(), byte_t);
			delete in_t;
			
		}
	}
	else {
		ser_imp->thread_add_reqs(uid, route, in_t);
	}
}
void Kubi_Ser_Py_Imp::_handle_notify(void *target, const std::string &uid, const char *route, const kubi::KNode *in_t) {
	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
	if (ser_imp->_if_inl) {
		std::map<std::string, RC_NOTIFY_FUN>::iterator it = ser_imp->_route_notify_pools.find(route);
		if (it != ser_imp->_route_notify_pools.end()) {
			py::bytes byte_t(in_t->data_bytes());
			it->second(uid, in_t->data_str(), byte_t);
			delete in_t;
		}
	}
	else {
		ser_imp->thread_add_notify(uid, route, in_t);
	}
}
void Kubi_Ser_Py_Imp::_handle_inline_ctrl(void* target, const std::string& signal, const std::string& msg) {
	Kubi_Ser_Py_Imp* ser_imp = (Kubi_Ser_Py_Imp*)target;
	if (ser_imp->_if_inl) {
		std::map<std::string, RC_INLINE_CTRL>::iterator it = ser_imp->_route_inline_ctrl_pools.find(signal);
		if (it != ser_imp->_route_inline_ctrl_pools.end()) {
			it->second(msg);
		}
	}
	else {
		ser_imp->thread_add_inline_ctrl(signal, msg);
	}
}

static void* help_get_py_bytes_pointer(const py::bytes &byte_t) {
	char *buffer = NULL;
	ssize_t length = 0;
	if (PYBIND11_BYTES_AS_STRING_AND_SIZE(byte_t.ptr(), &buffer, &length) == 0) {
	}
	return buffer;
}
static const py::bytes help_get_node_bytes(const kubi::KNode &pro) {
	return py::bytes(pro.data_bytes().data(), pro.data_bytes().length());
}
static const py::bytes help_test_bytes(const std::string& pro) {
	py::bytes byte_t(pro);
	printf("-> %s\n", pro.c_str());
	return byte_t;
}

PYBIND11_MODULE(kubi_ser_py, m) {
	py::class_<Kubi_Ser_Py_Imp>(m, "Kubi_Ser_Py_Imp")
		.def(py::init<>())
		.def("SetHeartTime", &Kubi_Ser_Py_Imp::SetHeartTime)
		.def("SetNotEnterStageTime", &Kubi_Ser_Py_Imp::SetNotEnterStageTime)
		.def("Start", &Kubi_Ser_Py_Imp::Start)
		.def("Ping", &Kubi_Ser_Py_Imp::Ping)
		.def("Notify", &Kubi_Ser_Py_Imp::Notify)
		.def("Response", &Kubi_Ser_Py_Imp::Response)
		.def("SendCtrlAsync", &Kubi_Ser_Py_Imp::SendCtrlAsync)
		.def("RegClose", &Kubi_Ser_Py_Imp::RegClose)
		.def("RegCloseClient", &Kubi_Ser_Py_Imp::RegCloseClient)
		.def("RegistNotifyRoute", &Kubi_Ser_Py_Imp::RegistNotifyRoute)
		.def("RegistRequestRoute", &Kubi_Ser_Py_Imp::RegistRequestRoute)
		.def("RegistInlineCtrlRoute", &Kubi_Ser_Py_Imp::RegistInlineCtrlRoute)
		.def("OnClientEnter", &Kubi_Ser_Py_Imp::OnClientEnter)
		.def("OnClientLeave", &Kubi_Ser_Py_Imp::OnClientLeave)
		.def("OnServerClose", &Kubi_Ser_Py_Imp::OnServerClose)
		.def("RunInYourThread", &Kubi_Ser_Py_Imp::RunInYourThread)
		;
	m.def("help_test_bytes", &help_test_bytes);
}
