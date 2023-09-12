
#ifndef _h_can_kubi_Server_h_
#define _h_can_kubi_Server_h_

#include "Kubi.pb.h"
#include "kubi_utils.h"
#include "Kubi_Session.h"
class Kubi_Server
{
public:
	typedef void(*VOID_RC)(void*);
	typedef void(*CLIENT_LEAVE_RC)(void*,  const std::string &);
	typedef void(*CLIENT_ENTER_RC)(void*, const std::string &, const std::string &,int);
	typedef void(*HANDLE_RECV_RC)(void*, const std::string &,const char *, const kubi::KNode *);
	typedef void(*HANDLE_RECV_SEND_RC)(void*, const std::string &, const char *, const kubi::KNode *);
	typedef void(*INLINE_CTRL_RC)(void*, const std::string&, const std::string&);
	struct _Imp_uv_data_node 
	{
		void *data01 = NULL;
		void *data02 = NULL;
		void *data03 = NULL;
	};
public:
	void SetRcTarget(void * target) {
		_rc_target = target;
	}
	void SetInlineCtrl(INLINE_CTRL_RC handle_ctrl) {
		_inline_ctrl_rc = handle_ctrl;
	}
	void WhenServerClose(VOID_RC handle_server_stage) {
		_handle_server_stage = handle_server_stage;
	}
	void WhenClientLeave(CLIENT_LEAVE_RC handle_client_exit) {
		_handle_client_exit = handle_client_exit;
	}
	void WhenClientEnter(CLIENT_ENTER_RC handle_client_enter) {
		_handle_client_enter = handle_client_enter;
	}
	void SetClientRequestRc(HANDLE_RECV_SEND_RC rc) {
		_request_rc = rc;
	}
	void SetClientNotifyRc(HANDLE_RECV_RC rc) {
		_notify_rc = rc;
	}
	void SetHeartTime(int heartTime) {
		Kubi_utils::C_HEART_TIME = heartTime;
	}
	void SetNotEnterStageTime(int heartTime) {
		Kubi_utils::C_STAY_BUT_NOT_LOGIN_TIME = heartTime;
	}
	int Start_inl(int port_) {
		port = port_;
		if (stage != 0) {
			return 1;
		}
		luv_loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
		uv_loop_init(luv_loop);
		uv_tcp_init(luv_loop, &luv_server);
		uv_timer_init(luv_loop, &timer);
		struct sockaddr_in addr;
		uv_ip4_addr("0.0.0.0", port, &addr);
		uv_tcp_bind(&luv_server, (const struct sockaddr*)&addr, 0);
		luv_server.data = this;
		int re = uv_listen((uv_stream_t*)&luv_server, SOMAXCONN, on_connection);
		if (re) {
			return 2;
		}
		timer.data = this;
		uv_timer_start(&timer, on_time_heart, 1000, 1000);
		this->stage = 2;
		Kubi_ws::hp_init_settings();
		Kubi_ws::ws_init_settings();
		uv_run(this->luv_loop, UV_RUN_DEFAULT);
		//ok server will be stop!
		this->stage = 0;
		this->_clients.clear();
		uv_loop_close(this->luv_loop);
		free(this->luv_loop);
		this->luv_loop = NULL;
		if (this->_handle_server_stage != NULL) {
			this->_handle_server_stage(this->_rc_target);
		}
		return 0;
	}
	int Start(int port_) {
		port = port_;
		if (stage != 0) {
			return 1;
		}
		luv_loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
		uv_loop_init(luv_loop);
		uv_tcp_init(luv_loop, &luv_server);
		uv_timer_init(luv_loop, &timer);
		uv_thread_create(&luv_run_thread, thread_server_start, this);
		struct sockaddr_in addr;
		uv_ip4_addr("0.0.0.0", port, &addr);
		uv_tcp_bind(&luv_server, (const struct sockaddr*)&addr, 0);
		luv_server.data = this;
		int re = uv_listen((uv_stream_t*)&luv_server, SOMAXCONN, on_connection);
		if (re) {
			return 2;
		}
		timer.data = this;
		uv_timer_start(&timer, on_time_heart, 1000, 1000);
		stage = 1;
		return 0;
	}
	void Reg_Close() {
		uv_async_t *control_async = (uv_async_t *)malloc(sizeof(uv_async_t));
		uv_async_init(luv_loop, control_async, on_ser_close_async);
		_Imp_uv_data_node *dn_t = (_Imp_uv_data_node *)malloc(sizeof(_Imp_uv_data_node));
		dn_t->data01 = this;
		control_async->data = dn_t;
		uv_async_send(control_async);
	}

	void Reg_Close_Client(const std::string &uid)
	{
		uv_async_t *control_async = (uv_async_t *)malloc(sizeof(uv_async_t));
		uv_async_init(luv_loop, control_async, on_client_close_async);
		_Imp_uv_data_node *dn_t = (_Imp_uv_data_node *)malloc(sizeof(_Imp_uv_data_node));
		dn_t->data01 = new std::string(uid);
		dn_t->data02 = this;
		control_async->data = dn_t;
		uv_async_send(control_async);
	}
	Kubi_Session * GetClient(const std::string &uid) const {
		std::unordered_map<std::string, Kubi_Session*>::const_iterator it;
		it =_clients.find(uid); 
		if (it != _clients.end())
		{
			return it->second;
		}

		return NULL;
	}
	/*std::vector<Kubi_Session*> GetClients()const  {
		return _clients;
	}*/
	Kubi_Server() {};
   ~Kubi_Server() {};



   void Response(const std::string& uid, const std::string& route,int pkg_id, const char* data_str,
	   const unsigned char* data_byte, int data_byte_len) {
	   const Kubi_Session* cn = GetClient(uid);
	   if (cn == NULL) {
		   return;
	   }
	   kubi::KNode  node_pro;
	   node_pro.set_pkg_type(Kubi_utils::PT_REQUEST);
	   node_pro.set_pkg_id(pkg_id);
	   node_pro.set_uid(cn->GetUid().c_str());
	   node_pro.set_route(route.c_str());
	   node_pro.set_data_str(data_str);
	   if (data_str == NULL) {
		   node_pro.set_data_str("");
	   }
	   else {
		   node_pro.set_data_str(data_str);
	   }
	   if (data_byte_len == 0 || data_byte == NULL) {
		   node_pro.set_data_bytes("", data_byte_len);
	   }
	   else {
		   node_pro.set_data_bytes(data_byte, data_byte_len);
	   }
	   _Send(cn, &node_pro);
   }

   void Response(const std::string& uid, const kubi::KNode* node_pro) {
	   const Kubi_Session* cn = GetClient(uid);
	   if (cn == NULL) {
		   return;
	   }
	   _Send(cn, node_pro);
   }

   void Send(const std::string &uid, const std::string &route, const char *data_str,
	   const unsigned char* data_byte, int data_byte_len) {
	   const Kubi_Session *  cn = GetClient(uid);
	   if (cn == NULL) {
		   return;
	   }
	   kubi::KNode  node_pro;
	   node_pro.set_pkg_type(Kubi_utils::PT_NOTICE);
	   node_pro.set_pkg_id(0);
	   node_pro.set_uid(cn->GetUid().c_str());
	   node_pro.set_route(route.c_str());
	   node_pro.set_data_str(data_str);
	   if (data_str == NULL) {
		   node_pro.set_data_str("");
	   }
	   else {
		   node_pro.set_data_str(data_str);
	   }
	   if (data_byte_len == 0 || data_byte == NULL) {
		   node_pro.set_data_bytes("", data_byte_len);
	   }
	   else {
		   node_pro.set_data_bytes(data_byte, data_byte_len);
	   }
	   _Send(cn, &node_pro);
   }

   void Send(const std::string &uid, const kubi::KNode * node_pro) {
	   const Kubi_Session *  cn = GetClient(uid);
	   if (cn == NULL) {
		   return;
	   }
	   _Send(cn, node_pro);
   }

   void Reg_Response(const std::string& uid, const kubi::KNode* node_pro) {
	   int32_t size = node_pro->ByteSize();
	   void* buffer = (void*)malloc(size);
	   node_pro->SerializePartialToArray(buffer, size);

	   uv_buf_t* luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	   luv_writebuffer->base = (char*)buffer;
	   luv_writebuffer->len = size;

	   uv_async_t* luv_send_async = (uv_async_t*)malloc(sizeof(uv_async_t));
	   uv_async_init(luv_loop, luv_send_async, on_send_async);
	   _Imp_uv_data_node* dn_t = (_Imp_uv_data_node*)malloc(sizeof(_Imp_uv_data_node));
	   dn_t->data01 = new std::string(uid);
	   dn_t->data02 = luv_writebuffer;
	   dn_t->data03 = this;
	   luv_send_async->data = dn_t;
	   uv_async_send(luv_send_async);
   }
   void Reg_Response(const std::string& uid, const std::string& route, int pkg_id, const char* data_str,
	   const unsigned char* data_byte, int data_byte_len) {
	   kubi::KNode  node_pro;
	   node_pro.set_pkg_type(Kubi_utils::PT_REQUEST);
	   node_pro.set_pkg_id(0);
	   node_pro.set_uid(uid.c_str());
	   node_pro.set_route(route.c_str());
	   if (data_str == NULL) {
		   node_pro.set_data_str("");
	   }
	   else {
		   node_pro.set_data_str(data_str);
	   }
	   if (data_byte_len == 0 || data_byte == NULL) {
		   node_pro.set_data_bytes("", data_byte_len);
	   }
	   else {
		   node_pro.set_data_bytes(data_byte, data_byte_len);
	   }
	   Reg_Send(uid, &node_pro);
   }

   void Reg_Send(const std::string& uid, const kubi::KNode* node_pro) {
	   int32_t size = node_pro->ByteSize();
	   void* buffer = (void*)malloc(size);
	   node_pro->SerializePartialToArray(buffer, size);

	   uv_buf_t* luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	   luv_writebuffer->base = (char*)buffer;
	   luv_writebuffer->len = size;

	   uv_async_t* luv_send_async = (uv_async_t*)malloc(sizeof(uv_async_t));
	   uv_async_init(luv_loop, luv_send_async, on_send_async);
	   _Imp_uv_data_node* dn_t = (_Imp_uv_data_node*)malloc(sizeof(_Imp_uv_data_node));
	   dn_t->data01 = new std::string(uid);
	   dn_t->data02 = luv_writebuffer;
	   dn_t->data03 = this;
	   luv_send_async->data = dn_t;
	   uv_async_send(luv_send_async);
   }
   void Reg_Send(const std::string &uid, const std::string &route, const char *data_str,
	   const unsigned char* data_byte, int data_byte_len) {
	   kubi::KNode  node_pro;
	   node_pro.set_pkg_type(Kubi_utils::PT_NOTICE);
	   node_pro.set_pkg_id(0);
	   node_pro.set_uid(uid.c_str());
	   node_pro.set_route(route.c_str());
	   if ( data_str == NULL) {
		   node_pro.set_data_str("");
	   }
	   else {
		   node_pro.set_data_str(data_str);
	   }
	   if (data_byte_len == 0 || data_byte == NULL) {
		   node_pro.set_data_bytes("", data_byte_len);
	   }
	   else {
		   node_pro.set_data_bytes(data_byte, data_byte_len);
	   }
	   Reg_Send(uid, &node_pro);
   }
   //just for control loop:
   void Send_inline_async(const std::string&signal, const char* data, int len) {
	   uv_buf_t* luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	   size_t frame_len =len;
	   char* frame = (char*)malloc(sizeof(char) * frame_len);
	   memcpy(frame,data, frame_len);
	   luv_writebuffer->len = frame_len;
	   luv_writebuffer->base = frame;
	
	   uv_async_t* luv_send_async = (uv_async_t*)malloc(sizeof(uv_async_t));
	   uv_async_init(luv_loop, luv_send_async, on_async_inline_ctrl);
	   _Imp_uv_data_node* dn_t = (_Imp_uv_data_node*)malloc(sizeof(_Imp_uv_data_node));
	   dn_t->data01 = new std::string(signal);
	   dn_t->data02 = luv_writebuffer;
	   dn_t->data03 = this;
	   luv_send_async->data = dn_t;
	   uv_async_send(luv_send_async);
   }
   void Send_ping_async(const std::string &uid, const char *data, int len) {
	   uv_buf_t *luv_writebuffer = Kubi_ws::help_make_ping((char *)data, len);
	   uv_async_t *luv_send_async = (uv_async_t *)malloc(sizeof(uv_async_t));
	   uv_async_init(luv_loop, luv_send_async, on_send_async_ctrl_msg);
	   _Imp_uv_data_node *dn_t = (_Imp_uv_data_node *)malloc(sizeof(_Imp_uv_data_node));
	   dn_t->data01 = new std::string(uid);
	   dn_t->data02 = luv_writebuffer;
	   dn_t->data03 = this;
	   luv_send_async->data = dn_t;
	   uv_async_send(luv_send_async);
   }
   void Send_ping(const std::string &uid,const char *data, int len) {
	   const Kubi_Session *  cn = GetClient(uid);
	   if (cn == NULL) {
		   return;
	   }
	   uv_buf_t *luv_writebuffer = Kubi_ws::help_make_ping((char *)data, len);
	   uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
	   reqw->data = luv_writebuffer;
	   uv_write(reqw, (uv_stream_t*)(((Kubi_Session *)cn)->_get_luv_handle()), luv_writebuffer, 1, on_after_write_inl);
   }

private:
	VOID_RC _handle_server_stage = NULL;
	CLIENT_LEAVE_RC _handle_client_exit = NULL;
	CLIENT_ENTER_RC _handle_client_enter = NULL;
	HANDLE_RECV_RC _notify_rc = NULL;
	HANDLE_RECV_SEND_RC _request_rc = NULL;
	INLINE_CTRL_RC _inline_ctrl_rc = NULL;

	std::atomic<int> stage = 0;
	int port = 3001;
	uv_loop_t* luv_loop = NULL;
	uv_tcp_t luv_server;
	uv_thread_t luv_run_thread;
	uv_timer_t timer;
	void *_rc_target = NULL;

	std::vector<Kubi_Session*> _session_not_login;
	std::unordered_map<std::string, Kubi_Session*> _clients;
	void _Send(const Kubi_Session * cn, const kubi::KNode * node_pro) {
		if (cn == NULL) {
			return;
		}
		int32_t size = node_pro->ByteSize();
		void *buffer = (void*)malloc(size);
		node_pro->SerializePartialToArray(buffer, size);
		_send_frame(cn, (char*)buffer, size);
		free(buffer);
	}
	void _Send(const Kubi_Session *  cn, const std::string &route, const char *data_str,
		const unsigned char* data_byte, int data_byte_len) {
		kubi::KNode  node_pro;
		node_pro.set_pkg_type(Kubi_utils::PT_NOTICE);
		node_pro.set_pkg_id(0);
		node_pro.set_uid(cn->GetUid().c_str());
		node_pro.set_route(route.c_str());
		node_pro.set_data_str(data_str);
		if (data_str == NULL) {
			node_pro.set_data_str("");
		}
		else {
			node_pro.set_data_str(data_str);
		}
		if (data_byte_len == 0 || data_byte == NULL) {
			node_pro.set_data_bytes("", data_byte_len);
		}
		else {
			node_pro.set_data_bytes(data_byte, data_byte_len);
		}
		_Send(cn, &node_pro);
	}
	void _send_data(const Kubi_Session * cn, const char *data, int len) {
		uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
		luv_writebuffer->base = (char*)malloc(len);
		luv_writebuffer->len = len;
		memcpy(luv_writebuffer->base, data, len);
		uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
		reqw->data = luv_writebuffer;
		uv_write(reqw, (uv_stream_t*)(((Kubi_Session *)cn)->_get_luv_handle()), luv_writebuffer, 1, on_after_write_inl);
	}
	void _send_pong(const Kubi_Session * cn, const char *data, int len) {
		uv_buf_t *luv_writebuffer = Kubi_ws::help_make_pong((char *)data, len);
		uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
		reqw->data = luv_writebuffer;
		uv_write(reqw, (uv_stream_t*)(((Kubi_Session *)cn)->_get_luv_handle()), luv_writebuffer, 1, on_after_write_inl);
	}
	

	void _send_frame(const Kubi_Session * cn, const char *data, int len) {
		std::vector<uv_buf_t*> f_ts = Kubi_ws::help_make_data((char*)data, len);
		for (int i = 0; i < f_ts.size(); i++) {
			uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
			uv_buf_t *luv_writebuffer = f_ts[i];
			reqw->data = luv_writebuffer;
			uv_write(reqw, (uv_stream_t*)(((Kubi_Session *)cn)->_get_luv_handle()), luv_writebuffer, 1, on_after_write_inl);
		}
	}
	
	void _send_heart(Kubi_Session *cn) {
		kubi::KNode  node_pro;
		node_pro.set_pkg_type(Kubi_utils::PT_HEART);
		node_pro.set_pkg_id(0);
		node_pro.set_uid(cn->GetUid().c_str());
		node_pro.set_route("");
		node_pro.set_data_str("");
		node_pro.set_data_bytes("");
		_Send(cn, &node_pro);
	}
	void  _push_client(Kubi_Session * nc) {
		if (nc->_get_stage() == Kubi_Session::CON_STAGE_ENTER) {
			Kubi_Session *t_client = GetClient(nc->GetUid());
			if (t_client != NULL) {
				_clients.erase(t_client->GetUid());
				uv_close((uv_handle_t*)(t_client->_get_luv_handle()), on_client_after_close);
				_clients.insert(std::pair<std::string, Kubi_Session*>(nc->GetUid(), nc));
			}
			else {
				_clients.insert(std::pair<std::string, Kubi_Session*>(nc->GetUid(), nc));
			}
		}
	}
	void  _delete_client(Kubi_Session * cn_t) {
		if (cn_t->_get_stage() == Kubi_Session::CON_STAGE_ENTER) {
			Kubi_Session *t_client = GetClient(cn_t->GetUid());
			if (t_client != NULL) {
				_clients.erase(t_client->GetUid());
			}
			if (_handle_client_exit != NULL) {
				_handle_client_exit(_rc_target, cn_t->GetUid());
			}
			uv_close((uv_handle_t*)(cn_t->_get_luv_handle()), on_client_after_close);
		}
	}
	void  _delete_all_client() {
		std::unordered_map<std::string, Kubi_Session*>::iterator it = _clients.begin();
		while (it != _clients.end()) {
			Kubi_Session* cn_t = it->second;
			if (_handle_client_exit != NULL) {
				_handle_client_exit(_rc_target, cn_t->GetUid());
			}
			uv_close((uv_handle_t*)(cn_t->_get_luv_handle()), on_client_after_close);
			_clients.erase(it++);
		}
	}
	void  _check_and_del_in_heart() {
		std::unordered_map<std::string, Kubi_Session*>::iterator it = _clients.begin();
		while (it != _clients.end()) {
			Kubi_Session* cn_t = it->second;
			bool ifHeart = cn_t->_check_heart();
			if (ifHeart) {
				_clients.erase(it++);
				if (_handle_client_exit != NULL) {
					_handle_client_exit(_rc_target, cn_t->GetUid());
				}
				uv_close((uv_handle_t*)(cn_t->_get_luv_handle()), on_client_after_close);
			}
			else {
				_send_heart(cn_t);
				it++;
			}
		}
	}
	void  _push_session_not_login(Kubi_Session * nc) {
		_session_not_login.push_back(nc);
	}
	int   _get_session_not_login_id(Kubi_Session * nc) {
		int ind = -1;
		for (int i = 0; i < _session_not_login.size(); i++) {
			Kubi_Session *t = _session_not_login[i];
			if (nc == t) {
				ind = i;
				break;
			}
		}
		return ind;
	}
	void  _delete_session_not_login(Kubi_Session * nc) {
		int ind = -1;
		for (int i = 0; i < _session_not_login.size(); i++) {
			Kubi_Session *t = _session_not_login[i];
			if (nc == t) {
				ind = i;
				break;
			}
		}
		if (ind != -1) {
			std::vector<Kubi_Session *>::iterator it = this->_session_not_login.begin() + ind;
			this->_session_not_login.erase(it);
			uv_close((uv_handle_t*)(nc->_get_luv_handle()), on_client_after_close);
		}
	}
	void  _delete_all_session_not_login() {
		std::vector<Kubi_Session*>::iterator it1;
		for (it1 = _session_not_login.begin(); it1 != _session_not_login.end(); ) {
			Kubi_Session *cn_t = (*it1);
			it1 = _session_not_login.erase(it1);
			uv_close((uv_handle_t*)(cn_t->_get_luv_handle()), on_client_after_close);
		}
	}
	void _check_and_del_session_not_login() {
		std::vector<Kubi_Session*>::iterator it1;
		for (it1 = _session_not_login.begin(); it1 !=_session_not_login.end(); ) {
			Kubi_Session *cn_t = (*it1);
			bool ifHeart = cn_t->_check_con_notlogin_heart();
			if (ifHeart) {
				it1 = _session_not_login.erase(it1);
				uv_close((uv_handle_t*)(cn_t->_get_luv_handle()), on_client_after_close);
			}
			else {
				++it1;
			}
		}
	}
	void  _swap_session_not_login(Kubi_Session * nc) {
		int ind = -1;
		for (int i = 0; i < _session_not_login.size(); i++) {
			Kubi_Session *t = _session_not_login[i];
			if (nc == t) {
				ind = i;
				break;
			}
		}
		if (ind != -1) {
			std::vector<Kubi_Session *>::iterator it = this->_session_not_login.begin() + ind;
			this->_session_not_login.erase(it);
		}
	}
private:
	static void thread_server_start(void * data);
	//-------
	static void on_time_heart(uv_timer_t *handle);
	static void on_connection(uv_stream_t* server, int status);
	static void on_uv_alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void on_after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void on_after_write_inl(uv_write_t *req, int status);
	static void on_async_inline_ctrl(uv_async_t* handle);
	static void on_send_async(uv_async_t *handle);
	static void on_send_async_ctrl_msg(uv_async_t *handle);
	static void on_ser_close_async(uv_async_t *handle);
	static void on_server_after_close(uv_handle_t *handle);
	static void on_client_close_async(uv_async_t *handle);
	static void on_client_after_close(uv_handle_t *handle);
	static void on_async_close(uv_handle_t *handle);
private:
	void pro_serice(Kubi_Session * nc,  kubi::KNode *np){
		nc->_reset_heart();
		nc->_reset_con_notlogin_heart();
		Kubi_utils::PACK_TYPE key = (Kubi_utils::PACK_TYPE)np->pkg_type();
		if (key == Kubi_utils::PT_WS_CLOSE) {
			nc->LinkServer()->_delete_client(nc);
			nc->LinkServer()->_delete_session_not_login(nc);
			delete np;
		}
		else if (key == Kubi_utils::PT_WS_ERR) {
			nc->LinkServer()->_delete_client(nc);
			nc->LinkServer()->_delete_session_not_login(nc);
			delete np;
		}
		else if (key == Kubi_utils::PT_WS_PING) {
			nc->LinkServer()->_send_pong(nc, np->data_str().data(), np->data_str().length());
			delete np;
		}
		else if (key == Kubi_utils::PT_WS_PONG) {
			delete np;
		}
		else if (key == Kubi_utils::PT_HEART){
			delete np;
		}
		else if (key == Kubi_utils::PT_WS_TEX) {
			delete np;
		}
		else if (key == Kubi_utils::PT_ENTER){
			nc->_set_stage(Kubi_Session::CON_STAGE_ENTER);
			nc->_set_uid(np->uid());
			nc->LinkServer()->_swap_session_not_login(nc);
			nc->LinkServer()->_push_client(nc);
			if (_handle_client_enter != NULL) {
				_handle_client_enter(_rc_target, nc->GetUid(), nc->GetIp(),nc->GetPort());
			}
			char heart_time_str[16];
			//登陆后会吧心跳发给前端
			sprintf(heart_time_str, "%d", Kubi_utils::C_HEART_TIME);
			kubi::KNode  node_pro;
			node_pro.set_pkg_type(Kubi_utils::PT_ENTER);
			node_pro.set_pkg_id(0);
			node_pro.set_uid(nc->GetUid());
			node_pro.set_route("");
			node_pro.set_data_str(heart_time_str);
			node_pro.set_data_bytes("");
			_Send(nc, &node_pro);
			delete np;
		}
		else if (key == Kubi_utils::PT_REQUEST) {
			if (_request_rc != NULL) {
				/*kubi::KNode  *node_pro=new kubi::KNode();
				node_pro->set_data_str("");
				node_pro->set_data_bytes("",0);
				node_pro->set_pkg_id(np->pkg_id());
				node_pro->set_uid(nc->GetUid());
				node_pro->set_route(np->route());
				node_pro->set_pkg_type(Kubi_utils::PT_REQUEST);*/
				_request_rc(_rc_target, nc->GetUid(), np->route().c_str(), np);
			   //_Send(nc, node_pro);//回到主线里再调用
			}
		}
		else if (key == Kubi_utils::PT_NOTICE) {
			if (_notify_rc != NULL) {
				_notify_rc(_rc_target, nc->GetUid(), np->route().c_str(), np);
			}
		}
		else if (key==Kubi_utils::PT_EXIT) {
			//登出：
			//这里主要是主动登出的用户 回调在:_delete_client()内
			nc->LinkServer()->_delete_client(nc);
			delete np;
		}
		
	}
};
#endif