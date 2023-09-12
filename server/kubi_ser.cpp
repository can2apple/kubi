#include"kubi_ser.h"

 //---------------------------------
void Kubi_Server::on_time_heart(uv_timer_t *handle) {
	  Kubi_Server *sev_t = (Kubi_Server*)handle->data;	 
	  if (Kubi_utils::C_STAY_BUT_NOT_LOGIN_TIME > 0) {
		  sev_t->_check_and_del_session_not_login();
	  }
	  if (Kubi_utils::C_HEART_TIME > 0) {
		  sev_t->_check_and_del_in_heart();
	  }
	 
 }

void Kubi_Server::thread_server_start(void * data) {
	Kubi_Server *sev_t = (Kubi_Server*)data;
	sev_t->stage = 2;
	Kubi_ws::hp_init_settings();
	Kubi_ws::ws_init_settings();
	uv_run(sev_t->luv_loop, UV_RUN_DEFAULT);
	//ok server will be stop!
	sev_t->stage = 0;
	sev_t->_clients.clear();
	uv_loop_close(sev_t->luv_loop);
	free(sev_t->luv_loop);
	sev_t->luv_loop = NULL;
	if (sev_t->_handle_server_stage != NULL) {
		sev_t->_handle_server_stage(sev_t->_rc_target);
	}
}
void Kubi_Server::on_connection(uv_stream_t* server, int status) {
	Kubi_Session *cn_t = new Kubi_Session();
	Kubi_Server *sev_t = (Kubi_Server*)server->data;
	uv_tcp_init(sev_t->luv_loop, cn_t->_get_luv_handle());
	uv_accept(server, (uv_stream_t*)cn_t->_get_luv_handle());
	cn_t->_set_link_server(sev_t);
	cn_t->_make_ip_port();
	cn_t->_get_luv_handle()->data = cn_t;
	sev_t->_push_session_not_login(cn_t);
	uv_read_start((uv_stream_t*)cn_t->_get_luv_handle(), on_uv_alloc_buf, on_after_read);
}

void Kubi_Server::on_uv_alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}
void Kubi_Server::on_after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	Kubi_Session *cn_t = (Kubi_Session *)stream->data;
	Kubi_Server *sev_t = (Kubi_Server*)cn_t->LinkServer();
	/* Error or EOF */
	if (nread < 0) {
		if (nread == UV_EOF) {
			
		}
		else if (nread == UV_ECONNRESET) {
			
		}
		else {
			
		}
		sev_t->_delete_session_not_login(cn_t);
		sev_t->_delete_client(cn_t);
	}
	else if (0 == nread) {
		/* Everything OK, but nothing read. */
	}
	else {
		if (cn_t->_get_stage()== Kubi_Session::CON_STAGE_BEGIN) {
			int re=cn_t->_help_parse_handshake(buf->base, nread);
			if (re==0) {
				cn_t->_set_stage(Kubi_Session::CON_STAGE_HANDSHA);
				std::string pak= cn_t->_help_make_handshake(101);
				sev_t->_send_data(cn_t,pak.c_str(), pak.length());

			}else if(re==1) {
				sev_t->_delete_session_not_login(cn_t);
			}
			else if (re == -1) {
			//keep going
			}
			
		}else {
			std::vector<kubi::KNode*> * read_buffs = new std::vector<kubi::KNode*>();
			cn_t->_help_parse_data(buf->base, nread, read_buffs);
			for (int i = 0; i < read_buffs->size(); i++) {
				sev_t->pro_serice(cn_t, read_buffs->at(i));
			}
			read_buffs->clear();
			delete read_buffs;
		}
	}
	free(buf->base);
}

void Kubi_Server::on_after_write_inl(uv_write_t *req, int status) {
	 uv_buf_t *luv_writebuffer= (uv_buf_t *)req->data;
	 free(luv_writebuffer->base);
	 free(luv_writebuffer);
	 free(req);
}
void Kubi_Server::on_ser_close_async(uv_async_t *handle) {
	_Imp_uv_data_node * dn_t = (_Imp_uv_data_node *)handle->data;
	Kubi_Server  *sev_t = NULL;
	sev_t = (Kubi_Server*)dn_t->data01;
	uv_close((uv_handle_t*)&(sev_t->luv_server), on_server_after_close);
	free(dn_t);
	uv_close((uv_handle_t*)handle, on_async_close);
}

void Kubi_Server::on_client_close_async(uv_async_t *handle) {
	_Imp_uv_data_node * dn_t = (_Imp_uv_data_node *)handle->data;
	Kubi_Server *sev_t = NULL;
	Kubi_Session* cn_t = NULL;
	std::string *uid = (std::string *)dn_t->data01;
	sev_t = (Kubi_Server*)dn_t->data02;
	cn_t = sev_t->GetClient((*uid));
	if (cn_t != NULL) {
		sev_t->_delete_client(cn_t);
	}
	delete uid;
	free(dn_t);
	uv_close((uv_handle_t*)handle, on_async_close);
}
void Kubi_Server::on_async_inline_ctrl(uv_async_t* handle) {
	_Imp_uv_data_node* dn_t = (_Imp_uv_data_node*)handle->data;
	std::string* signal = (std::string*)dn_t->data01;
	uv_buf_t* luv_writebuffer = (uv_buf_t*)dn_t->data02;
	Kubi_Server* sev_t = (Kubi_Server*)dn_t->data03;
	if (sev_t->_inline_ctrl_rc != NULL) {
		sev_t->_inline_ctrl_rc(sev_t->_rc_target, *signal,std::string(luv_writebuffer->base, luv_writebuffer->len));
	}
	free(luv_writebuffer->base);
	free(luv_writebuffer);
	free(dn_t);
	delete signal;
	uv_close((uv_handle_t*)handle, on_async_close);
}
void Kubi_Server::on_send_async_ctrl_msg(uv_async_t *handle){
	_Imp_uv_data_node * dn_t = (_Imp_uv_data_node *)handle->data;
	std::string *uid = (std::string *)dn_t->data01;
	uv_buf_t *luv_writebuffer = (uv_buf_t*)dn_t->data02;
	Kubi_Server *sev_t = (Kubi_Server*)dn_t->data03;
	Kubi_Session* cn = sev_t->GetClient((*uid));
	if (cn != NULL) {
		uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
		reqw->data = luv_writebuffer;
		uv_write(reqw, (uv_stream_t*)(((Kubi_Session *)cn)->_get_luv_handle()), luv_writebuffer, 1, on_after_write_inl);
	}
	free(dn_t);
	delete uid;
	uv_close((uv_handle_t*)handle, on_async_close);
}
void Kubi_Server::on_send_async(uv_async_t *handle) {
	_Imp_uv_data_node * dn_t = (_Imp_uv_data_node *)handle->data;
	std::string *uid = (std::string *)dn_t->data01;
	uv_buf_t *luv_writebuffer = (uv_buf_t*)dn_t->data02;
	Kubi_Server *sev_t = (Kubi_Server*)dn_t->data03;
	Kubi_Session* cn = sev_t->GetClient((*uid));
	if (cn != NULL) {
		sev_t->_send_frame(cn,luv_writebuffer->base, luv_writebuffer->len);
	}
	free(luv_writebuffer->base);
	free(luv_writebuffer);
	free(dn_t);
	delete uid;
	uv_close((uv_handle_t*)handle, on_async_close);
}
void Kubi_Server::on_server_after_close(uv_handle_t *handle) {
	Kubi_Server *sev_t = (Kubi_Server*)handle->data;
	sev_t->_delete_all_client();
	sev_t->_delete_all_session_not_login();
}
void Kubi_Server::on_client_after_close(uv_handle_t *handle) {
	Kubi_Session *cn = (Kubi_Session*)handle->data;
	delete cn;
}

void Kubi_Server::on_async_close(uv_handle_t *handlet) {
	uv_async_t *handle = (uv_async_t *)handlet;
	free(handle);
	handle = NULL;
}

//#include<iostream>
//#include<json/json.h>
//#include <conio.h>
//#include <glog/logging.h>
//Kubi_Server ser;
//int main(int argc, char **argv) {
//	FLAGS_log_dir = "./log/";
//	FLAGS_logtostderr = true;  //设置日志消息是否转到标准输出而不是日志文件 true只是cmd打印
//	FLAGS_alsologtostderr = false;  //设置日志消息除了日志文件之外是否标准输出 false cmd不打印 error还是打印的
// 	FLAGS_colorlogtostderr = true;  //设置记录到标准输出的颜色消息（如果终端支持）
//	FLAGS_log_prefix = true;  //设置日志前缀是否应该添加到每行输出
//	FLAGS_logbufsecs = 10;  //设置可以缓冲日志的最大秒数，0指实时输出
//	FLAGS_max_log_size = 100;  //设置最大日志文件大小（以MB为单位）
//	FLAGS_stop_logging_if_full_disk = true;  //设置是否在磁盘已满时避免日志记录到磁盘
//	ser.Start(8080);
//	
//	while (1) {
//		
//		if (_kbhit()) {
//			char ch = _getch();
//			if (ch == 27) { break; }
//		}
//		Sleep(1);
//	}
//	return 0;
//}

//#include<iostream>
//#include<json/json.h>
//#include<opencv2/opencv.hpp>
//
//bool _help_bridge_node_to_json( const Kubillity_Help::BridgeNode &in, Json::Value &dst) {
//	Json::Reader reader;
//	if (reader.parse(in.data_str, dst)) {
//		return true;
//	}
//	return false;
//}
//
//
//
//int main(int argc,char **argv) {
//
//	FLAGS_log_dir = "./log/";
//	FLAGS_logtostderr = true;  //设置日志消息是否转到标准输出而不是日志文件 true只是cmd打印
//	FLAGS_alsologtostderr = false;  //设置日志消息除了日志文件之外是否标准输出 false cmd不打印 error还是打印的
// 	FLAGS_colorlogtostderr = true;  //设置记录到标准输出的颜色消息（如果终端支持）
//	FLAGS_log_prefix = true;  //设置日志前缀是否应该添加到每行输出
//	FLAGS_logbufsecs = 10;  //设置可以缓冲日志的最大秒数，0指实时输出
//	FLAGS_max_log_size = 100;  //设置最大日志文件大小（以MB为单位）
//	FLAGS_stop_logging_if_full_disk = true;  //设置是否在磁盘已满时避免日志记录到磁盘
//
//	
//
//	Kubi_Server ser;
//	ser.WhenServerClose([]()->void{
//		LOG(WARNING) << "Server close!";
//	});
//	ser.WhenClientEnter([](const Session*node)->void{
//		LOG(WARNING) << "Session has connect to server!";
//	});
//	ser.WhenClientLeave([](const Session*node)->void {
//		LOG(WARNING) << "Session has Leave to server!";
//	});
//
//
//	ser.OnRequest("login", [](const Session*node,const Kubillity_Help::BridgeNode &in, Kubillity_Help::BridgeNode &to)->void {
//		Json::Value v_in;
//		if (_help_bridge_node_to_json(in, v_in)) {
//			std::string name= v_in["name"].asString();
//			LOG(INFO) << name.c_str();
//
//			Json::Value v_rsp;
//			v_rsp["code"] = 123;
//			v_rsp["msg"] = "welcome";
//
//			to.data_str = v_rsp.toStyledString();
//			Kubillity_Help::BridgeNode msg;
//			msg.data_str = v_rsp.toStyledString();
//
//			((Session*)node)->LinkServer()->Send((Session*)node,"say_hello" ,msg);
//		}
//		
//	});
//
//	ser.OnRequest("show_img", [](const Session*node, const Kubillity_Help::BridgeNode &in, Kubillity_Help::BridgeNode &to)->void {
//		to.data_byte = in.data_byte;
//	});
//
//
//	ser.Start(8080);
//	getchar();
//	google::ShutdownGoogleLogging();
//	return 0;
//}