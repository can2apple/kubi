
#ifndef _h_can_Kubillity_Server_cpp11_imp_
#define _h_can_Kubillity_Server_cpp11_imp_
#include"kubi_ser.h"
#include <functional>
class Kubi_Ser_Cpp11_Imp
{
private:
	typedef  std::function<void()> RC_SERVER_CLOSE;
	typedef  std::function<void(const std::string &uid,const std::string &client_ip,int port)> RC_CLIENT_ENTER;
	typedef  std::function<void(const std::string &uid)> RC_CLIENT_LEAVE;
	//typedef  std::function<void(const std::string& ,const kubi::KNode *,kubi::KNode *) > RC_REQUEST_FUN;
	//typedef  std::function<void(const std::string&,const kubi::KNode *) > RC_NOTIFY_FUN;
	typedef  std::function<void(const std::string&uid, const std::string &data_str,const std::string &data_bytes) > RC_NOTIFY_FUN;
	typedef  std::function<void(const std::string& uid,int pkg_id ,const std::string& data_str, const std::string& data_bytes) > RC_REQUEST_FUN;
	typedef  std::function<void(const std::string& msg)> RC_INLINE_CTRL;
	
	struct BdNodeSerSta
	{
		RC_SERVER_CLOSE rc;
	};
	struct BdNodeInlineCtrl
	{
		std::string signal = "";
		std::string msg = "";
		RC_INLINE_CTRL rc;
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
		//kubi::KNode *to = NULL;
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

	void Ping(const std::string &uid,const char *data_str) {
		if (_if_inl) {
			_server.Send_ping(uid, data_str, strlen(data_str));
		}
		else {
			_server.Send_ping_async(uid, data_str, strlen(data_str));
		}
	}
	//void PingInl(const std::string &uid, const char *data_str) {
	//	_server.Send_ping(uid, data_str, strlen(data_str));
	//}
	void Notify(const std::string &uid, const std::string &route, const char *data_str,
		const unsigned char* data_byte, int data_byte_len){
		if (_if_inl) {
			_server.Send(uid, route, data_str, data_byte, data_byte_len);
		}
		else {
			_server.Reg_Send(uid, route, data_str, data_byte, data_byte_len);
		}
	}
	void Response(const std::string& route, const std::string& uid ,int pkg_id,const char* data_str,
		const unsigned char* data_byte, int data_byte_len) {
		if (_if_inl) {
			_server.Response(uid, route, pkg_id, data_str, data_byte, data_byte_len);
		}
		else {
			_server.Reg_Response(uid, route, pkg_id, data_str, data_byte, data_byte_len);
		}
	}

	void SendCtrlAsync(const std::string& signal, const std::string& msg) {
		_server.Send_inline_async(signal, msg.data(), msg.length());
	}
	/*void NotifyInl(const std::string &uid, const std::string &route, const char *data_str,
		const unsigned char* data_byte, int data_byte_len) {
		if (_if_inl) {
			_server.Send(uid, route, data_str, data_byte, data_byte_len);
		}
	}*/
	void RegClose() {
		_server.Reg_Close();
	}
	void RegCloseClient(const std::string &uid){
		_server.Reg_Close_Client(uid);
	}
	/*Kubi_Session * GetClient(const std::string &uid) const {
		return _server.GetClient(uid);
	}*/
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
	void RegistInlineCtrlRoute(std::string route, RC_INLINE_CTRL rc) {
		std::map<std::string, RC_INLINE_CTRL>::iterator it;
		it = _route_inline_ctrl_pools.find(route);
		if (it == _route_inline_ctrl_pools.end()) {
			_route_inline_ctrl_pools.insert(std::pair<std::string, RC_INLINE_CTRL>(route, rc));
		}
	}
	void RunInYourThread() {
		if (!_if_inl) {
			thread_run_inline_ctrl();
			thread_run_ser_pool();
			thread_run_client_enter();
			thread_run_client_leave();
			thread_run_reqs();
			thread_run_notifys();
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
	std::vector<BdNodeSerSta*> pool_ser_stages = std::vector< BdNodeSerSta*>();
	std::vector<BdNodeInlineCtrl*> pool_inline_ctrl = std::vector< BdNodeInlineCtrl*>();
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
		/*if (rc_lock_client_enter.try_lock()) {
			for (int i = 0; i < pool_client_enters.size(); i++) {
				BdNodeClientEnter* t = pool_client_enters[i];
				t->rc(t->uid,t->ip,t->port);
				delete t;
			}
			pool_client_enters.clear();
			rc_lock_client_enter.unlock();
		}*/
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

		/*	if (rc_lock_client_leave.try_lock()) {
				for (int i = 0; i < pool_client_leaves.size(); i++) {
					BdNodeClientLeave* t = pool_client_leaves[i];
					t->rc(t->uid);
					delete t;
				}
				pool_client_leaves.clear();
				rc_lock_client_leave.unlock();
			}*/
	}
	void thread_add_reqs(const std::string &uid, const std::string &route, const kubi::KNode *in) {
		std::map<std::string, RC_REQUEST_FUN>::iterator it = this->_route_request_pools.find(route);
		if (it != this->_route_request_pools.end()) {

			BdNodeReq *bds = new BdNodeReq();
			bds->uid = uid;
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
			t->rc(t->uid,t->in->pkg_id(), t->in->data_str(),t->in->data_bytes());
			delete (kubi::KNode *)t->in;
			delete t;
		}
		pool_reqs_tmp.clear();
	}
	void thread_add_notify(const std::string &uid, const std::string &route, const kubi::KNode *in) {
		std::map<std::string, RC_NOTIFY_FUN>::iterator it = this->_route_notify_pools.find(route);
		if (it != this->_route_notify_pools.end()) {

			BdNodeNotify *bds = new BdNodeNotify();
			bds->uid = uid;
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
			t->rc(t->uid, t->in->data_str(), t->in->data_bytes());
			//t->rc(t->uid, t->in);
			delete (kubi::KNode *)t->in;
			delete t;
		}
		pool_not_tmp.clear();

		/*	if (rc_lock_client_notify.try_lock()) {
				for (int i = 0; i < pool_notifys.size(); i++) {
					BdNodeNotify* t = pool_notifys[i];
					t->rc(t->uid, t->in);
					delete (kubi::KNode *)t->in;
					delete t;
				}
				pool_notifys.clear();
				rc_lock_client_notify.unlock();
			}*/
	}
	/*void thread_add_client_enter(const std::string &uid, const std::string &ip, int port) {
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
		if (rc_lock_client_enter.try_lock()) {
			for (int i = 0; i < pool_client_enters.size(); i++) {
				BdNodeClientEnter* t = pool_client_enters[i];
				t->rc(t->uid,t->ip,t->port);
				delete t;
			}
			pool_client_enters.clear();
			rc_lock_client_enter.unlock();
		}
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
		if (rc_lock_client_leave.try_lock()) {
			for (int i = 0; i < pool_client_leaves.size(); i++) {
				BdNodeClientLeave* t = pool_client_leaves[i];
				t->rc(t->uid);
				delete t;
			}
			pool_client_leaves.clear();
			rc_lock_client_leave.unlock();
		}
	}
	void thread_add_reqs(const std::string &uid,const std::string &route,const kubi::KNode *in, kubi::KNode *to) {
		std::map<std::string, RC_REQUEST_FUN>::iterator it = this->_route_request_pools.find(route);
		if (it != this->_route_request_pools.end()) {
			
			BdNodeReq *bds = new BdNodeReq();
			bds->uid =uid;
			bds->in = in;
			bds->to = to;
			bds->rc = it->second;
			rc_lock_client_request.lock();
			pool_reqs.push_back(bds);
			rc_lock_client_request.unlock();
		}
	}
	void thread_run_reqs() {
		if (rc_lock_client_request.try_lock()) {
			for (int i = 0; i < pool_reqs.size(); i++) {
				BdNodeReq* t = pool_reqs[i];
				t->rc(t->uid,t->in,t->to);
				_server.Reg_Send(t->uid, (t->to));
				delete (kubi::KNode *)t->in;
				delete t->to;
				delete t;
			}
			pool_reqs.clear();
			rc_lock_client_request.unlock();
		}
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
		if (rc_lock_client_notify.try_lock()) {
			for (int i = 0; i < pool_notifys.size(); i++) {
				BdNodeNotify* t = pool_notifys[i];
				t->rc(t->uid, t->in);
				delete (kubi::KNode *)t->in;
				delete t;
			}
			pool_notifys.clear();
			rc_lock_client_notify.unlock();
		}
	}*/
	//--------------------------
	Kubi_Server _server;
	static void _handle_server_stage(void *target);
	static void _handle_inline_ctrl(void* target, const std::string& signal, const std::string&msg );
	static void _handle_client_enter(void *target, const std::string &uid,const std::string &ip,int port);
	static void _handle_client_exite(void *target, const std::string &uid);
	static void _handle_request(void *target, const std::string &uid,const char *route, const kubi::KNode *in_t);
	static void _handle_notify(void *target, const std::string &uid, const char *route, const kubi::KNode* in_t);
};

#endif