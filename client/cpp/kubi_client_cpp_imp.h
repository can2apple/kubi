#ifndef Kubi_Websocket_cpp_imp_h_
#define Kubi_Websocket_cpp_imp_h_
#include "kubi_websocket.h"
#include "Kubi.pb.h"
#include <iostream>
#include <functional>
typedef  std::function<void(const kubi::KNode *) > RC_MSG_BACK;
typedef  std::function<void()> RC_VOID_BACK;
typedef  std::function<void(int)> RC_ERR_BACK;
class Kubi_Client {
public:
	int Start(const std::string &_uid, const std::string &_ip,int _port){
		uid = _uid;
		ip = _ip;
		port = _port;
		ws.set_recv_rc(imp_on_msg);
		ws.set_open_rc(imp_on_open);
		ws.set_close_rc(imp_on_close);
		ws.set_err_rc(imp_on_err);
		ws.SetRcTarget(this);
		ws.Start(ip.c_str(), port);
		return TryToConnect();
	}
	int  TryToConnect() {
		_ifOnEnd = false;
		heart_caculate_ind = 0;
		heart_has_start = false;
		heart_has_end = true;
		pack_id = 0;
		 //因为on 在enter之后
		_clear_not_pools();
		_clear_req_pools();
		return ws.TryToConnect();
		
	}
	void OnEnter(RC_VOID_BACK rc) {
		_onenter = rc;
	}
	/*void OnClose(RC_VOID_BACK rc) {
		_onclose = rc;
	}
	void OnErr(RC_ERR_BACK rc) {
		_onerr = rc;
	}*/
	void OnEnd(RC_VOID_BACK rc) {
		_onEnd = rc;
	}
	void Request(const std::string &route,const char *data_str,
		const unsigned char* data_bytes,int data_bytes_len, RC_MSG_BACK rc) {
		int packid= _get_pkg_id_add();
		kubi::KNode  node_pro ;
		node_pro.set_data_str(data_str);
		node_pro.set_data_bytes(data_bytes, data_bytes_len);
		node_pro.set_pkg_id(packid);
		node_pro.set_uid(uid);
		node_pro.set_route(route);
		node_pro.set_pkg_type(Kubi_utils::PT_REQUEST);
		_add_req_pools(packid, rc);
		_send_(node_pro);
	}
	void Notice(const std::string &route, const char *data_str,
		const unsigned char* data_bytes, int data_bytes_len) {
		
		kubi::KNode  node_pro;
		node_pro.set_data_str(data_str);
		node_pro.set_data_bytes(data_bytes, data_bytes_len);
		node_pro.set_pkg_id(0);
		node_pro.set_uid(uid);
		node_pro.set_route(route);
		node_pro.set_pkg_type(Kubi_utils::PT_NOTICE);
		_send_(node_pro);
	}
	void OnNotice(const std::string &route, RC_MSG_BACK rc) {
		_add_not_pools(route, rc);
	}
	void Close() {
		ws.Reg_Close();
	}
	void Loop() {
		ws.BackToThread();
	}

private:
	Kubi_Websocket ws;
	std::string ip = "127.0.0.1";
	int port = 8080;
	std::string uid="";
	int pack_id = 0;
	std::mutex _pack_id_lock;
	RC_VOID_BACK _onenter=NULL;
	RC_VOID_BACK _onclose = NULL;
	RC_VOID_BACK _onEnd = NULL;
	bool _ifOnEnd = false;
	RC_ERR_BACK _onerr = NULL;
	std::mutex _req_lock;
	std::map<int, RC_MSG_BACK > _req_pools = std::map<int, RC_MSG_BACK >();
	std::mutex _not_lock;
	std::map<std::string, RC_MSG_BACK > _not_pools = std::map<std::string, RC_MSG_BACK >();
	void _send_(const kubi::KNode & node_pro) {
		int32_t size = node_pro.ByteSize();
		void *buffer = (void*)malloc(size);
		node_pro.SerializePartialToArray(buffer, size);
		ws.Reg_Send((const char*)buffer, size);
		free(buffer);
	}

	void _send_enter() {
		kubi::KNode  node_pro;
		node_pro.set_pkg_type(Kubi_utils::PT_ENTER);
		node_pro.set_pkg_id(0);
		node_pro.set_uid(uid.c_str());
		node_pro.set_route("");
		node_pro.set_data_str("");
		node_pro.set_data_bytes("");
		_send_(node_pro);
	}

	void _service(const char *data,int len) {
		heart_reset_stage();
		kubi::KNode np;
		if (np.ParseFromArray(data, len)) {
			Kubi_utils::PACK_TYPE key = (Kubi_utils::PACK_TYPE)np.pkg_type();
			if (key == Kubi_utils::PT_HEART) {
				heart_send();
			}
			else if (key == Kubi_utils::PT_ENTER)
			{
				int heart_max = 4;
				sscanf(np.data_str().c_str(), "%d", &(heart_max));
				this->heart_time_max = heart_max;
				if (this->heart_time_max < 0) {
					this->heart_time_max = 0;
				}
				if (heart_time_max > 0) {
					heart_init_timer();
				}
			
				if (_onenter != NULL) {
					_onenter();
				}
			}
			else if (key == Kubi_utils::PT_NOTICE) {
				std::pair<bool, RC_MSG_BACK> res = _get_not_rc(np.route());
				if (res.first) {
					res.second(&np);
				}
			}
			else if (key == Kubi_utils::PT_REQUEST) {
				std::pair<bool, RC_MSG_BACK> res = _get_req_rc(np.pkg_id());
				if (res.first) {
					res.second(&np);
				}
			}
			else if (Kubi_utils::PT_EXIT) {
				Close();
			}
		}
		else {
			Close();
		}
	}
	int _get_pkg_id_add() {
		_pack_id_lock.lock();
		pack_id++;
		if (pack_id > 65535) {
			pack_id = 0;
		}
		_pack_id_lock.unlock();
		return pack_id;
	}
	void _clear_req_pools() {
		_req_lock.lock();
		_req_pools.clear();
		_req_lock.unlock();
	}
	void _add_req_pools(int package_id, RC_MSG_BACK fun) {
		_req_lock.lock();
		_req_pools.insert(std::pair<int, RC_MSG_BACK>(package_id, fun));
		_req_lock.unlock();
	}
	std::pair<bool, RC_MSG_BACK> _get_req_rc(int package_id) {
		_req_lock.lock();
		RC_MSG_BACK fun;
		bool if_has = false;
		std::map<int, RC_MSG_BACK >::iterator it;
		it = _req_pools.find(package_id);
		if (it != _req_pools.end()) {
			if_has = true;
			fun = it->second;
			_req_pools.erase(it);
		}
		_req_lock.unlock();
		return std::pair<bool, RC_MSG_BACK>(if_has, fun);
	}
	void _add_not_pools(const std::string &route, RC_MSG_BACK fun) {
		_not_lock.lock();
		_not_pools.insert(std::pair<std::string, RC_MSG_BACK>(route, fun));
		_not_lock.unlock();
	}
	std::pair<bool, RC_MSG_BACK> _get_not_rc(const std::string &route) {
		_not_lock.lock();
		RC_MSG_BACK fun;
		bool if_has = false;
		std::map<std::string, RC_MSG_BACK >::iterator it;
		it = _not_pools.find(route);
		if (it != _not_pools.end()) {
			if_has = true;
			fun = it->second;
		}
		_not_lock.unlock();
		return std::pair<bool, RC_MSG_BACK>(if_has, fun);
	}
	void _clear_not_pools() {
		_not_lock.lock();
		_not_pools.clear();
		_not_lock.unlock();
	}
private:
	int heart_caculate_ind = 0;
	int heart_time_max = 4;
	std::atomic<bool> heart_has_start = false;
	std::atomic<bool> heart_has_end = true;
	std::mutex  heart_req_lock;
	uv_thread_t heart_run_thread;
	
	void heart_send() {
		if (heart_time_max > 0) {
			kubi::KNode  node_pro;
			node_pro.set_pkg_type(Kubi_utils::PT_HEART);
			node_pro.set_pkg_id(0);
			node_pro.set_uid(uid.c_str());
			node_pro.set_route("");
			node_pro.set_data_str("");
			node_pro.set_data_bytes("");
			_send_(node_pro);
		}
	}
	void heart_init_timer() {
		if (heart_time_max > 0) {
			heart_has_start = true;
			heart_has_end = false;
			heart_caculate_ind = 0;
			uv_thread_create(&heart_run_thread, on_time_heart_start, this);
		}
	}
	bool heart_check_stage() {
		if (heart_time_max > 0) {
			heart_req_lock.lock();
			if (this->heart_caculate_ind == this->heart_time_max) {
				this->heart_caculate_ind = 0;
				heart_req_lock.unlock();
				return true;
			}
			else {
				this->heart_caculate_ind++;
			}
			heart_req_lock.unlock();
		}
		return false;
	}
	void heart_reset_stage() {
		if (heart_time_max > 0) {
			heart_req_lock.lock();
			this->heart_caculate_ind = 0;
			heart_req_lock.unlock();
		}
	}
	void heart_reg_stop() {
		if (heart_time_max > 0) {
			heart_has_start = false;
			while (1) {
				uv_sleep(10);
				if (heart_has_end) {
					break;
				}
			}
		}
	}
private:
	static void on_time_heart_start(void * data);
	static void imp_on_open(void *data);
	static void imp_on_err(void *data, int stage);
	static void imp_on_close(void *data);
	static void imp_on_msg(void *data, const char *msg, unsigned int len);

};

#endif
