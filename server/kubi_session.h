
#ifndef _h_can_Kubillity_Session_Node_
#define _h_can_Kubillity_Session_Node_
#include"kubi_utils.h"
#include"kubi_ws.h"
class Kubi_Server;
class Kubi_Session {
public:
	enum CON_STAGE_TYPE
	{
		CON_STAGE_BEGIN=0,
		CON_STAGE_HANDSHA,
		CON_STAGE_ENTER
	};
	Kubi_Session() {
		_ws.link_sess = this;
		Kubi_ws::hp_init_parser(&_ws.hp_parser,&_ws);
		Kubi_ws::ws_init_parser(&_ws.ws_parser, &_ws);
	}
	////只有一部分
	//Kubi_Session(const Kubi_Session &other) {
	//	
	//}
	//Kubi_Session & operator=(const Kubi_Session &other) {
	//	
	//	return *this;
	//}
	/*void JustCopyIneed(const std::string &p_uid,const std::string &p_ip,int p_port) {
		uid = p_uid;
		ip = p_ip;
		port = p_port;
	}*/
   ~Kubi_Session() {
	}
	std::string GetUid()const {
		return uid;
	}
	Kubi_Server *  LinkServer() {
		return link_server;
	}
	std::string GetIp() const{
		return ip;
	}
	int GetPort() const {
		return port;
	}

public:
	void _set_link_server(Kubi_Server *l) {
		link_server = l;
	}
	bool _check_heart() {
		if (connect_stage == 2) {
			if (this->heart_caculate_ind == Kubi_utils::C_HEART_TIME) {
				this->heart_caculate_ind = 0;
				return true;
			}
			else {
				this->heart_caculate_ind++;
			}
		}
		return false;
	}
	void _reset_heart() {
		this->heart_caculate_ind = 0;
	}
	int _get_stage() {
		return connect_stage;
	}
	void _reset_con_notlogin_heart() {
		this->connect_but_not_login_ind = 0;
	}
	bool _check_con_notlogin_heart() {
		if (connect_stage != 2) {
			if (this->connect_but_not_login_ind == Kubi_utils::C_STAY_BUT_NOT_LOGIN_TIME) {
				this->connect_but_not_login_ind = 0;
				return true;
			}
			else {
				this->connect_but_not_login_ind++;
			}
		}
		return false;
	}
	uv_tcp_t * _get_luv_handle() {
		return &luv_handle;
	}
	void _set_uid(std::string _uid) {
		uid = _uid;
	}
	void _set_stage(CON_STAGE_TYPE stage) {
		 connect_stage = stage;//0 1 2:  
	}
	void _make_ip_port() {
		struct sockaddr peername;
		int namelen;
		uv_tcp_getpeername(&luv_handle, &peername, &namelen);
		char check_ip[17];
		struct sockaddr_in check_addr = *(struct sockaddr_in*) (&peername);
		uv_ip4_name(&check_addr, (char*)check_ip, sizeof check_ip);
		port = ntohs(check_addr.sin_port);
		char dst_ip[32];
		memcpy((dst_ip), check_ip, strlen(check_ip) + 1);
		ip = dst_ip;
	}
	int _help_parse_data(const char *data,int len,std::vector<kubi::KNode*> * read_buffs){
		  Kubi_ws::help_parse_data(&(this->_ws), data, len, read_buffs);
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
	int _help_parse_handshake(const char *data, int len) {
		bool re=Kubi_ws::help_parse_handshake(&_ws,data, len);
		if (re) {
			connect_stage = CON_STAGE_HANDSHA;
			return 0;
		}
		else {
			if (_ws.hp_parser.nread > Kubi_utils::C_HANDSHAKE_MAX_SIZE) {
				return 1;
			}
		}
		return -1;
	}
	std::string _help_make_handshake(int code=101) {
		return Kubi_ws::help_make_handshake(&(this->_ws), code);
	}
	Kubi_ws _ws;
private:
	Kubi_Server *link_server = NULL;
	std::string uid = "";
	uv_tcp_t luv_handle;
	int heart_caculate_ind = 0;
	int connect_but_not_login_ind = 0;
	CON_STAGE_TYPE connect_stage = CON_STAGE_BEGIN;//0 1 2:  
	std::string ip="";
	int port = 0;

	
};
#endif