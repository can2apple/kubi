#ifndef _h_can_kubi_websocket_
#define _h_can_kubi_websocket_
#include"kubi_utils.h"
#include"kubi_websocket_tool.h"
enum KUBI_WS_STATION
{
	NS_BEGIN = 0,
	NS_CONNECT_SUCCESS,
	NS_CONNECT_FAILED,
	NS_HANDSHA_SUCCESS,
	WS_ST_HANDSHA_ERR,
	WS_ST_PARSER_ERR,
	WS_ST_CLOSE
};

class Kubi_Websocket {
public:
	struct _Imp_uv_data_node
	{
		void *data01 = NULL;
		void *data02 = NULL;
		void *data03 = NULL;
	};
	typedef void(*R_ON_OPEN)(void *);
	typedef void(*R_RECV)(void *,const char *,unsigned int);
	typedef void(*R_ON_ERR)(void *, int);
	typedef void(*R_ON_CLOSE)(void *);
	Kubi_Websocket() {};
   ~Kubi_Websocket() {};
    void set_open_rc(R_ON_OPEN rc) {
		_open_rc = rc;
    }
	void set_recv_rc(R_RECV rc) {
		_recv_rc = rc;
	}
	void set_err_rc(R_ON_ERR rc) {
		_err_rc = rc;
	}
	void set_close_rc(R_ON_CLOSE rc) {
		_close_rc = rc;
	}
	void SetRcTarget(void * target) {
		_rc_target = target;
	}
	int  Start(const char *_ip, int _port){
		if (con_stat!= NS_BEGIN) {
			return 1;
		}
		ip = _ip;
		port = _port;
		uv_ip4_addr(ip.c_str(), port, &luv_addr);
		_ws.link_sess = this;
		Kubi_Websocket_tool::ws_init_settings();
		Kubi_Websocket_tool::ws_init_parser(&_ws.ws_parser, &_ws);
		//TryToConnect();
		return 0;
	}
	int  TryToConnect() {
		if (con_stat == NS_HANDSHA_SUCCESS) {
			return 1;
		}
		con_stat = NS_BEGIN;
		luv_handle.data = this;
		luv_loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
		uv_loop_init(luv_loop);
		uv_tcp_init(luv_loop, &luv_handle);
		uv_tcp_connect(&luv_connect_req, &luv_handle, (const sockaddr*)&luv_addr, on_connection);
		uv_thread_create(&luv_run_thread, thread_client_start, this);
		return 0;
	}
	void Reg_Close() {
		if(con_stat == NS_BEGIN) {
			return ;
		}
		uv_async_t *control_async = (uv_async_t *)malloc(sizeof(uv_async_t));
		uv_async_init(luv_loop, control_async, on_pro_close);
		_Imp_uv_data_node *dn_t = (_Imp_uv_data_node *)malloc(sizeof(_Imp_uv_data_node));
		dn_t->data01 = this;
		control_async->data = dn_t;
		uv_async_send(control_async);
	}
	int  Reg_Send(const  char*data_byte, int data_byte_len) {
		if (con_stat != NS_HANDSHA_SUCCESS) {
			return -1;
		}
		_send_async(data_byte, data_byte_len);
		return 0;
	}
	void BackToThread() {
		std::vector<Kubi_Websocket_tool::BrNode*> _data_pool_tmp;
		if (_lock_data_pool.try_lock())
		{
			_data_pool_tmp = std::vector<Kubi_Websocket_tool::BrNode*>(_data_pool);
			_data_pool.clear();
			_lock_data_pool.unlock();
		}
		for (int i = 0; i < _data_pool_tmp.size(); i++) {
			Kubi_Websocket_tool::BrNode* dn = _data_pool_tmp[i];
			Kubi_Websocket_tool::PACK_TYPE type = (Kubi_Websocket_tool::PACK_TYPE) dn->type;
			switch (type)
			{
			case Kubi_Websocket_tool::PACK_TYPE::PT_CONNECT_FAILD:
				if (_err_rc != NULL) {
					_err_rc(_rc_target, type);
				}

				break;
			case Kubi_Websocket_tool::PACK_TYPE::PT_WS_HAND_FAILD:
				if (_err_rc != NULL) {
					_err_rc(_rc_target, type);
				}

				break;
			case Kubi_Websocket_tool::PACK_TYPE::PT_WS_ERR:
				if (_err_rc != NULL) {
					_err_rc(_rc_target, type);
				}

				break;
			case Kubi_Websocket_tool::PACK_TYPE::PT_WS_CLOSE:
				if (_close_rc != NULL) {
					_close_rc(_rc_target);
				}
				break;
			case Kubi_Websocket_tool::PACK_TYPE::PT_WS_HAND_SUCC:
				if (_open_rc != NULL) {
					_open_rc(_rc_target);
				}
				break;
			case Kubi_Websocket_tool::PACK_TYPE::PT_WS_BIN:
				if (_recv_rc != NULL) {
					_recv_rc(_rc_target, dn->data.data(), dn->data.length());
				}
				break;
			case Kubi_Websocket_tool::PACK_TYPE::PT_WS_TEX:
				if (_recv_rc != NULL) {
					_recv_rc(_rc_target, dn->data.data(), dn->data.length());
				}
				break;
			default:
				break;
			}
			delete dn;
		}
		_data_pool_tmp.clear();

	}
private:
	std::mutex _lock_data_pool;
	std::vector<Kubi_Websocket_tool::BrNode*> _data_pool;
	void *_rc_target=NULL;
	R_ON_OPEN _open_rc = NULL;
	R_RECV _recv_rc = NULL;
	R_ON_ERR _err_rc = NULL;
	R_ON_CLOSE _close_rc = NULL;

	KUBI_WS_STATION con_stat = NS_BEGIN;
	int port = 3001;
	std::string ip = "127.0.0.1";
	uv_loop_t* luv_loop = NULL;
	uv_tcp_t luv_handle;
	uv_connect_t luv_connect_req;
	uv_shutdown_t shutdown_req;
	uv_thread_t luv_run_thread;
	struct sockaddr_in luv_addr;
	Kubi_Websocket_tool _ws;

	void _clear_data_pool() {
		_lock_data_pool.lock();
		for (int i = 0; i < _data_pool.size(); i++) {
			Kubi_Websocket_tool::BrNode* dn = _data_pool[i];
			delete dn;
			
		}
		_data_pool.clear();
		_lock_data_pool.unlock();
	}
	void _add_data(Kubi_Websocket_tool::BrNode* br) {
		_lock_data_pool.lock();
		_data_pool.push_back(br);
		_lock_data_pool.unlock();
	}
	void _send_data( const char *data, int len) {
		uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
		luv_writebuffer->base = (char*)malloc(len);
		luv_writebuffer->len = len;
		memcpy(luv_writebuffer->base, data, len);
		uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
		reqw->data = luv_writebuffer;
		uv_write(reqw, (uv_stream_t*)(&luv_handle), luv_writebuffer, 1, on_after_write);
	}
	void _send_pong(const char *data, int len) {
		uv_buf_t *luv_writebuffer = Kubi_Websocket_tool::help_make_pone((char *)data, len);
		uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
		reqw->data = luv_writebuffer;
		uv_write(reqw, (uv_stream_t*)(&luv_handle), luv_writebuffer, 1, on_after_write);
	}
	void _send_ping_async(const char *data, int len) {
		uv_buf_t *luv_writebuffer = Kubi_Websocket_tool::help_make_ping((char *)data, len);
		uv_async_t *luv_send_async = (uv_async_t *)malloc(sizeof(uv_async_t));
		uv_async_init(luv_loop, luv_send_async, on_send_async_ctrl_msg);
		_Imp_uv_data_node *asn = (_Imp_uv_data_node *)malloc(sizeof(_Imp_uv_data_node));
		asn->data01 = this;
		asn->data02 = luv_writebuffer;
		luv_send_async->data = asn;
		uv_async_send(luv_send_async);
	}
	void _send_ping(const char *data, int len) {
		uv_buf_t *luv_writebuffer = Kubi_Websocket_tool::help_make_ping((char *)data, len);
		uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
		reqw->data = luv_writebuffer;
		uv_write(reqw, (uv_stream_t*)(&luv_handle), luv_writebuffer, 1, on_after_write);
	}
	void _send_frame( const char *data, int len) {
		std::vector<uv_buf_t*> f_ts = Kubi_Websocket_tool::help_make_data((char*)data, len);
		for (int i = 0; i < f_ts.size(); i++) {
			uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
			uv_buf_t *luv_writebuffer = f_ts[i];
			reqw->data = luv_writebuffer;
			uv_write(reqw, (uv_stream_t*)(&luv_handle), luv_writebuffer, 1, on_after_write);
		}
	}
	void _send_async(const char *data, int len) {
		uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
		luv_writebuffer->base = (char *)malloc(len);
		luv_writebuffer->len = len;
		memcpy(luv_writebuffer->base, data, len);
		uv_async_t *luv_send_async = (uv_async_t *)malloc(sizeof(uv_async_t));
		uv_async_init(luv_loop, luv_send_async, on_send_async);
		_Imp_uv_data_node *asn = (_Imp_uv_data_node *)malloc(sizeof(_Imp_uv_data_node));
		asn->data01 = this;
		asn->data02 = luv_writebuffer;
		luv_send_async->data = asn;
		uv_async_send(luv_send_async);
	}
	int _help_parse_handshake(const char *data, int len) {
		int re = Kubi_Websocket_tool::help_parse_client_handshake(&_ws, data, len);
		if (re==0) {
			_ws.data = "";
			return 0;
		}
		else if(re==1) {
			if (_ws.data.length() > Kubi_utils::C_HANDSHAKE_MAX_SIZE) {
				_ws.data = "";
				return 1;
			}
		}
		return -1;
	}
	int _help_parse_data(const char *data, int len, std::vector<Kubi_Websocket_tool::BrNode*> * read_buffs) {
		Kubi_Websocket_tool::help_parse_data(&(this->_ws), data, len, read_buffs);
		if (read_buffs->size() > 0) {
			if (_ws.final_code != WS_FINAL_FRAME) {
				return 1;
			}
			if (_ws.opcode == WS_OP_BINARY) {

			}
			else if (_ws.opcode == WS_OP_CLOSE) {

			}
			else if (_ws.opcode == WS_OP_PING) {

			}
			else if (_ws.opcode == WS_OP_PONG) {

			}
			else {
				return 1;
			}
		}
		return 0;
	}
	std::string _help_make_handshake() {
		return Kubi_Websocket_tool::help_client_make_handshake(&(this->_ws), this->_ws.web_socket_key);
	}
	void _send_handshake() {
		std::string data=_help_make_handshake();
		_send_data(data.data(), data.length());
	}
	void parse(const char *data ,int len) {
		if (con_stat == NS_CONNECT_SUCCESS) {
			int re = _help_parse_handshake(data, len);
			if (re == 0) {
				con_stat = NS_HANDSHA_SUCCESS;
				Kubi_Websocket_tool::BrNode* np = new Kubi_Websocket_tool::BrNode();
				np->data = "";
				np->type = Kubi_Websocket_tool::PT_WS_HAND_SUCC;
				_add_data(np);

			}
			else if(re==1){
				con_stat = WS_ST_HANDSHA_ERR;
				Reg_Close();
			}
			else if (re == -1) {
			   //keep going
			}
		}
		else if(con_stat == NS_HANDSHA_SUCCESS){
			std::vector<Kubi_Websocket_tool::BrNode*> *read_buffs = new std::vector<Kubi_Websocket_tool::BrNode*>();
			_help_parse_data(data, len, read_buffs);
			for (int i = 0; i < read_buffs->size(); i++) {
				on_serice(read_buffs->at(i));
				//_add_data(read_buffs->at(1));
			}
			read_buffs->clear();
			delete read_buffs;
		}
	}
	void on_serice(Kubi_Websocket_tool::BrNode* np) {
		Kubi_Websocket_tool::PACK_TYPE key = (Kubi_Websocket_tool::PACK_TYPE)np->type;
		if (key == Kubi_Websocket_tool::PT_WS_CLOSE) {
			Reg_Close();
			delete np;
		}
		else if (key == Kubi_Websocket_tool::PT_WS_ERR) {
			con_stat= WS_ST_PARSER_ERR;
			Reg_Close();
			delete np;
		}
		else if (key == Kubi_Websocket_tool::PT_WS_PING) {
			_send_pong(np->data.data(),np->data.length());
			delete np;
		}
		else if (key == Kubi_Websocket_tool::PT_WS_PONG){
			delete np;
		}
		else if (key == Kubi_Websocket_tool::PT_WS_TEX) {
			//delete np;
			_add_data(np);
		}
		else if (key == Kubi_Websocket_tool::PT_WS_BIN) {
			//delete np;
			_add_data(np);
		}
	}
private:
	static void thread_client_start(void * data);
	static void on_connection(uv_connect_t* server, int status);
	static void on_after_write(uv_write_t *req, int status);
	static void on_send_async(uv_async_t *handle);
	static void on_send_async_ctrl_msg(uv_async_t *handle);
	static void on_pro_close(uv_async_t *handle);
	static void on_uv_alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void on_after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void on_client_close(uv_handle_t *handle);
	static void on_shutdown_cb(uv_shutdown_t* req, int status);
	static void on_async_close(uv_handle_t *handle);

};

#endif