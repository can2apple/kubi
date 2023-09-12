
#ifndef _h_can_Kubillity_Server_py_imp_
#define _h_can_Kubillity_Server_py_imp_
#include"kubi_ser.h"
#include<pybind11/pybind11.h>
#include<pybind11/numpy.h>
#include<pybind11/stl.h>
namespace py = pybind11;

class Kubi_Ser_Py_Imp
{
private:
	//typedef  std::function<void()> RC_SERVER_CLOSE;
	//typedef  std::function<void(const std::string& uid, const std::string& client_ip, int port)> RC_CLIENT_ENTER;
	//typedef  std::function<void(const std::string& uid)> RC_CLIENT_LEAVE;
	//typedef  std::function<void(const std::string& uid, const std::string& data_str, const std::string& data_bytes) > RC_NOTIFY_FUN;
	//typedef  std::function<void(const std::string& uid, int pkg_id, const std::string& data_str, const std::string& data_bytes) > RC_REQUEST_FUN;
	//typedef  std::function<void(const std::string& msg)> RC_INLINE_CTRL;
	typedef   py::function RC_SERVER_CLOSE;
	typedef   py::function RC_CLIENT_ENTER;
	typedef   py::function RC_CLIENT_LEAVE;
	typedef   py::function RC_REQUEST_FUN;
	typedef   py::function RC_NOTIFY_FUN;
	typedef   py::function RC_INLINE_CTRL;

	struct BdNodeInlineCtrl
	{
		std::string signal = "";
		std::string msg = "";
		RC_INLINE_CTRL rc;
	};
	struct BdNodeSerSta
	{
		RC_SERVER_CLOSE rc;
	};
	struct BdNodeClientEnter
	{
		std::string uid = "";
		std::string ip = "";
		int port = -1;
		RC_CLIENT_ENTER rc;
	};
	struct BdNodeClientLeave
	{
		std::string uid = "";
		RC_CLIENT_LEAVE rc;
	};
	struct BdNodeReq
	{
		std::string uid = "";
		const kubi::KNode *in = NULL;
		RC_REQUEST_FUN rc;
		void *data = NULL;
	};
	struct BdNodeNotify
	{
		std::string uid = "";
		const kubi::KNode *in = NULL;
		RC_NOTIFY_FUN rc;
		void *data = NULL;
	};
public:
	void SetHeartTime(int heartTime) {
		_server.SetHeartTime(heartTime);
	}
	void SetNotEnterStageTime(int heartTime) {
		_server.SetNotEnterStageTime(heartTime);
	}
	int Start(int port,bool if_inl) {
		_server.SetRcTarget(this);
		_server.WhenServerClose(_handle_server_stage);
		_server.WhenClientEnter(_handle_client_enter);
		_server.WhenClientLeave(_handle_client_exite);
		_server.SetClientRequestRc(_handle_request);
		_server.SetClientNotifyRc(_handle_notify);
		_server.SetInlineCtrl(_handle_inline_ctrl);
		_if_inl = if_inl;
		if (_if_inl) {
			return _server.Start_inl(port);
		}
		else {
			return _server.Start(port);
		}
	}
	void Ping(const std::string& uid, const std::string & data_str) {
		if (_if_inl) {
			_server.Send_ping(uid, data_str.c_str(), strlen(data_str.c_str()));
		}
		else {
			_server.Send_ping_async(uid, data_str.c_str(), strlen(data_str.c_str()));
		}
	}

	void Notify(const std::string &uid, const std::string &route, const std::string&data_str,
		const py::bytes &data_byte) {
		char *buffer = NULL;
		ssize_t length = 0;
		if (PYBIND11_BYTES_AS_STRING_AND_SIZE(data_byte.ptr(), &buffer, &length) == 0){
			if (_if_inl) {
				_server.Send(uid.c_str(), route, data_str.c_str(), (const unsigned char*)buffer, length);
			}
			else {
				_server.Reg_Send(uid.c_str(), route, data_str.c_str(), (const unsigned char*)buffer, length);
			}
		}
		else {
			if (_if_inl) {
				_server.Send(uid.c_str(), route, data_str.c_str(), NULL, 0);
			}
			else {
				_server.Reg_Send(uid.c_str(), route, data_str.c_str(), NULL, 0);
			}
		}
	}
	void Response(const std::string& route, const std::string& uid, int pkg_id, const std::string& data_str,
		const py::bytes& data_byte) {
		char* buffer = NULL;
		ssize_t length = 0;
		if (PYBIND11_BYTES_AS_STRING_AND_SIZE(data_byte.ptr(), &buffer, &length) == 0) {
			if (_if_inl) {
				_server.Response(uid.c_str(), route, pkg_id,data_str.c_str(), (const unsigned char*)buffer, length);
			}
			else {
				_server.Response(uid.c_str(), route, pkg_id,data_str.c_str(), (const unsigned char*)buffer, length);
			}
		}
		else {
			if (_if_inl) {
				_server.Response(uid.c_str(), route, pkg_id, data_str.c_str(), NULL, 0);
			}
			else {
				_server.Response(uid.c_str(), route, pkg_id, data_str.c_str(), NULL, 0);
			}
		}
	}
	void SendCtrlAsync(const std::string& signal, const std::string& msg) {
		_server.Send_inline_async(signal, msg.data(), msg.length());
	}
	void RegClose() {
		_server.Reg_Close();
	}
	void RegCloseClient(const std::string &uid){
		_server.Reg_Close_Client(uid);
	}
	void OnServerClose(RC_SERVER_CLOSE rc) {
		_fun_server_close = rc;
	}
	void OnClientEnter(RC_CLIENT_ENTER rc) {
		_fun_client_enter = rc;
	}
	void OnClientLeave(RC_CLIENT_LEAVE rc) {
		_fun_client_leave = rc;
	}
	void RegistRequestRoute(const std::string &route, RC_REQUEST_FUN rc) {
		std::map<std::string, RC_REQUEST_FUN>::iterator it;
		it = _route_request_pools.find(route);
		if (it == _route_request_pools.end()) {
			_route_request_pools.insert(std::pair<std::string, RC_REQUEST_FUN>(route, rc));
		}
	}
	void RegistNotifyRoute(std::string route, RC_NOTIFY_FUN rc) {
		std::map<std::string, RC_NOTIFY_FUN>::iterator it;
		it = _route_notify_pools.find(route);
		if (it == _route_notify_pools.end()) {
			_route_notify_pools.insert(std::pair<std::string, RC_NOTIFY_FUN>(route, rc));
		}
	}
	void RegistInlineCtrlRoute(std::string signal, RC_INLINE_CTRL rc) {
		std::map<std::string, RC_INLINE_CTRL>::iterator it;
		it = _route_inline_ctrl_pools.find(signal);
		if (it == _route_inline_ctrl_pools.end()) {
			_route_inline_ctrl_pools.insert(std::pair<std::string, RC_INLINE_CTRL>(signal, rc));
		}
	}
	void RunInYourThread() {
		if (!_if_inl) {
			thread_run_inline_ctrl();
			thread_run_ser_pool();
			thread_run_client_enter();
			thread_run_client_leave();
			thread_run_notifys();
			thread_run_reqs();
		}
	}
private:
	bool _if_inl = false;
	RC_SERVER_CLOSE _fun_server_close;
	RC_CLIENT_ENTER _fun_client_enter;
	RC_CLIENT_LEAVE _fun_client_leave;
	std::map<std::string, RC_INLINE_CTRL > _route_inline_ctrl_pools = std::map<std::string, RC_INLINE_CTRL >();
    std::map<std::string,  RC_REQUEST_FUN > _route_request_pools= std::map<std::string, RC_REQUEST_FUN >();
    std::map<std::string, RC_NOTIFY_FUN> _route_notify_pools= std::map<std::string, RC_NOTIFY_FUN >();
	//-------back to main thread--------
	std::vector<BdNodeInlineCtrl*> pool_inline_ctrl = std::vector< BdNodeInlineCtrl*>();
	std::vector<BdNodeSerSta*> pool_ser_stages = std::vector< BdNodeSerSta*>();
	std::vector<BdNodeClientEnter*> pool_client_enters = std::vector< BdNodeClientEnter*>();
	std::vector<BdNodeClientLeave*> pool_client_leaves = std::vector< BdNodeClientLeave*>();
	std::vector<BdNodeReq*> pool_reqs = std::vector< BdNodeReq*>();
	std::vector<BdNodeNotify*> pool_notifys = std::vector< BdNodeNotify*>();
	std::mutex rc_lock_ser;
	std::mutex rc_lock_inline_ctrl;
	std::mutex rc_lock_client_enter;
	std::mutex rc_lock_client_leave;
	std::mutex rc_lock_client_request;
	std::mutex rc_lock_client_notify;

	void thread_add_inline_ctrl(const std::string& signal, const std::string& msg) {
		std::map<std::string, RC_INLINE_CTRL>::iterator it = this->_route_inline_ctrl_pools.find(signal);
		if (it != this->_route_inline_ctrl_pools.end()) {

			BdNodeInlineCtrl* bds = new BdNodeInlineCtrl();
			bds->signal = signal;
			bds->msg = msg;
			bds->rc = it->second;
			rc_lock_inline_ctrl.lock();
			pool_inline_ctrl.push_back(bds);
			rc_lock_inline_ctrl.unlock();
		}
	}
	void thread_run_inline_ctrl() {
		std::vector<BdNodeInlineCtrl*> pool_tmp;
		if (rc_lock_inline_ctrl.try_lock()) {
			pool_tmp = std::vector<BdNodeInlineCtrl*>(pool_inline_ctrl);
			pool_inline_ctrl.clear();
			rc_lock_inline_ctrl.unlock();
		}
		for (int i = 0; i < pool_tmp.size(); i++) {
			BdNodeInlineCtrl* t = pool_tmp[i];
			t->rc(t->msg);
			delete t;
		}
		pool_tmp.clear();
	}

	void thread_add_ser_pool() {
		BdNodeSerSta* bds = new BdNodeSerSta();
		bds->rc = _fun_server_close;
		rc_lock_ser.lock();
		pool_ser_stages.push_back(bds);
		rc_lock_ser.unlock();
	}
	void thread_run_ser_pool() {
		if (rc_lock_ser.try_lock()) {
			for (int i = 0; i < pool_ser_stages.size(); i++){
				BdNodeSerSta* t = pool_ser_stages[i];
				t->rc();
				delete t;
			}
			pool_ser_stages.clear();
			rc_lock_ser.unlock();
		}
	}
	void thread_add_client_enter(const std::string &uid, const std::string &ip, int port) {
		BdNodeClientEnter *bds = new BdNodeClientEnter();
		bds->uid = uid;
		bds->ip = ip;
		bds->port = port;
		bds->rc = _fun_client_enter;
		rc_lock_client_enter.lock();
		pool_client_enters.push_back(bds);
		rc_lock_client_enter.unlock();
	}
	void thread_run_client_enter() {
		std::vector<BdNodeClientEnter*> pool_tmp;
		if (rc_lock_client_enter.try_lock()) {
			pool_tmp = std::vector<BdNodeClientEnter*>(pool_client_enters);
			pool_client_enters.clear();
			rc_lock_client_enter.unlock();
		}
		for (int i = 0; i < pool_tmp.size(); i++) {
			BdNodeClientEnter* t = pool_tmp[i];
			t->rc(t->uid, t->ip, t->port);
			delete t;
		}
		pool_tmp.clear();
	}
	void thread_add_client_leave(const std::string &uid) {
		BdNodeClientLeave *bds = new BdNodeClientLeave();
		bds->uid = uid;
		bds->rc = _fun_client_leave;
		rc_lock_client_leave.lock();
		pool_client_leaves.push_back(bds);
		rc_lock_client_leave.unlock();
	}
	void thread_run_client_leave() {
		  std::vector<BdNodeClientLeave*> pool_tmp;
		  if (rc_lock_client_leave.try_lock()) {
			  pool_tmp = std::vector<BdNodeClientLeave*>(pool_client_leaves);
			  pool_client_leaves.clear();
			  rc_lock_client_leave.unlock();
		  }
		  for (int i = 0; i < pool_tmp.size(); i++) {
			  BdNodeClientLeave* t = pool_tmp[i];
			  t->rc(t->uid);
			  delete t;
		  }
		  pool_tmp.clear();
	}
	void thread_add_reqs(const std::string &uid,const std::string &route,const kubi::KNode *in) {
		std::map<std::string, RC_REQUEST_FUN>::iterator it = this->_route_request_pools.find(route);
		if (it != this->_route_request_pools.end()) {
			
			BdNodeReq *bds = new BdNodeReq();
			bds->uid =uid;
			bds->in = in;
			bds->rc = it->second;
			rc_lock_client_request.lock();
			pool_reqs.push_back(bds);
			rc_lock_client_request.unlock();
		}
	}
	void thread_run_reqs() {
		std::vector<BdNodeReq*> pool_reqs_tmp;
		if (rc_lock_client_request.try_lock()) {
			pool_reqs_tmp = std::vector< BdNodeReq*>(pool_reqs);
			pool_reqs.clear();
			rc_lock_client_request.unlock();
		}
		for (int i = 0; i < pool_reqs_tmp.size(); i++) {
			BdNodeReq* t = pool_reqs_tmp[i];
			py::bytes byte_t(t->in->data_bytes());
			t->rc(t->uid,  t->in->pkg_id(), t->in->data_str(), byte_t);
			delete (kubi::KNode *)t->in;
			delete t;
		}
		pool_reqs_tmp.clear();
	}

	void thread_add_notify(const std::string &uid, const std::string &route, const kubi::KNode *in) {
		std::map<std::string, RC_NOTIFY_FUN>::iterator it = this->_route_notify_pools.find(route);
		if (it != this->_route_notify_pools.end()) {

			BdNodeNotify *bds = new BdNodeNotify();
			bds->uid =uid;
			bds->in = in;
			bds->rc = it->second;
			rc_lock_client_notify.lock();
			pool_notifys.push_back(bds);
			rc_lock_client_notify.unlock();
		}
	}

	void thread_run_notifys() {
		std::vector<BdNodeNotify*> pool_not_tmp;
		if (rc_lock_client_notify.try_lock()) {
			pool_not_tmp = std::vector< BdNodeNotify*>(pool_notifys);
			pool_notifys.clear();
			rc_lock_client_notify.unlock();
		}
		for (int i = 0; i < pool_not_tmp.size(); i++) {
			BdNodeNotify* t = pool_not_tmp[i];
			py::bytes byte_t(t->in->data_bytes());
			t->rc(t->uid, t->in->data_str(), byte_t);
			delete (kubi::KNode *)t->in;
			delete t;
		}
		pool_not_tmp.clear();
	}
	//--------------------------
	Kubi_Server _server;
	static void _handle_server_stage(void *target);
	static void _handle_client_enter(void *target, const std::string &uid,const std::string &ip,int port);
	static void _handle_client_exite(void *target, const std::string &uid);
	static void _handle_request(void *target, const std::string &uid,const char *route, const kubi::KNode *in_t);
	static void _handle_notify(void *target, const std::string &uid, const char *route, const kubi::KNode *in_t);
	static void _handle_inline_ctrl(void* target, const std::string& signal, const std::string& msg);
};

#endif