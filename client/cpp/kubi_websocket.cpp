#include"kubi_websocket.h"

void Kubi_Websocket::thread_client_start(void * data) {
	Kubi_Websocket *ns = (Kubi_Websocket*)data;
	uv_run(ns->luv_loop, UV_RUN_DEFAULT);
	uv_loop_close(ns->luv_loop);
	free(ns->luv_loop);
	ns->luv_loop = NULL;
	ns->_clear_data_pool();
	Kubi_Websocket_tool::BrNode* np = new Kubi_Websocket_tool::BrNode();
	np->data = "";
	if (ns->con_stat== KUBI_WS_STATION::NS_CONNECT_FAILED) {
		np->type = Kubi_Websocket_tool::PT_CONNECT_FAILD;
	}
	else if (ns->con_stat == KUBI_WS_STATION::WS_ST_HANDSHA_ERR) {
		np->type = Kubi_Websocket_tool::PT_WS_HAND_FAILD;
	}
	else if (ns->con_stat == KUBI_WS_STATION::WS_ST_PARSER_ERR) {
		np->type = Kubi_Websocket_tool::PT_WS_ERR;
	}
	else if (ns->con_stat == KUBI_WS_STATION::WS_ST_CLOSE) {
		np->type = Kubi_Websocket_tool::PT_WS_CLOSE;
	}
	ns->_add_data(np);
}
void Kubi_Websocket::on_connection(uv_connect_t* server, int status) {
	 Kubi_Websocket *nc = (Kubi_Websocket*)server->handle->data;
	 if (status == 0) {
		nc->con_stat = NS_CONNECT_SUCCESS;
		uv_read_start((uv_stream_t*)&nc->luv_handle, on_uv_alloc_buf, on_after_read);
		nc->_send_handshake();
	 }
	 else {
		nc->con_stat = NS_CONNECT_FAILED;
	 }
}
void Kubi_Websocket::on_uv_alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}
void Kubi_Websocket::on_after_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	Kubi_Websocket *cn = (Kubi_Websocket *)stream->data;
	if (nread < 0) {
		if (nread == UV_EOF) {
		}
		else if (nread == UV_ECONNRESET) {
		}
		else {
		}
		if (cn->con_stat == KUBI_WS_STATION::NS_HANDSHA_SUCCESS||
			cn->con_stat == KUBI_WS_STATION::NS_BEGIN) {
			cn->con_stat = KUBI_WS_STATION::WS_ST_CLOSE;
		}
	}
	else if (0 == nread) {
	}
	else {
		cn->parse(buf->base, nread);
	}
	free(buf->base);
}
void Kubi_Websocket::on_after_write(uv_write_t *req, int status) {
	 uv_buf_t *luv_writebuffer = (uv_buf_t*)req->data;
	 free(luv_writebuffer->base);
	 free(luv_writebuffer);
	 free(req);
}

void Kubi_Websocket::on_pro_close(uv_async_t *handle) {
	_Imp_uv_data_node * dn_t = (_Imp_uv_data_node *)handle->data;
	Kubi_Websocket* cn_t = (Kubi_Websocket*)dn_t->data01;
	uv_shutdown(&(cn_t->shutdown_req), (uv_stream_s*)&(cn_t->luv_handle), on_shutdown_cb);// 这里会主动调用uv_loop_close 无需手动去调用
	uv_close((uv_handle_t*)&(cn_t->luv_handle), on_client_close);// 这里会主动调用uv_loop_close 无需手动去调用
	free(dn_t);
	uv_close((uv_handle_t*)handle, on_async_close);
}

 void Kubi_Websocket::on_send_async_ctrl_msg(uv_async_t *handle) {
	 _Imp_uv_data_node * asn = (_Imp_uv_data_node *)handle->data;
	 Kubi_Websocket* cn = (Kubi_Websocket*)(asn->data01);
	 uv_buf_t *luv_writebuffer = (uv_buf_t*)asn->data02;
	 uv_write_t *reqw = (uv_write_t*)malloc(sizeof *reqw);
	 reqw->data = luv_writebuffer;
	 uv_write(reqw, (uv_stream_t*)(&(cn->luv_handle)), luv_writebuffer, 1, on_after_write);
	 free(asn);
	 uv_close((uv_handle_t*)handle, on_async_close);
}
 void Kubi_Websocket::on_send_async(uv_async_t *handle) {
	_Imp_uv_data_node * asn = (_Imp_uv_data_node *)handle->data;
	 Kubi_Websocket* cn = (Kubi_Websocket*)(asn->data01);
	 uv_buf_t *luv_writebuffer = (uv_buf_t*)asn->data02;
	 cn->_send_frame(luv_writebuffer->base, luv_writebuffer->len);
	 free(luv_writebuffer->base);
	 free(luv_writebuffer);
	 free(asn);
	 uv_close((uv_handle_t*)handle, on_async_close);
 }
 void Kubi_Websocket::on_shutdown_cb(uv_shutdown_t* req, int status) {
	
 }
 void Kubi_Websocket::on_client_close(uv_handle_t *handle) {
	  Kubi_Websocket * cn = (Kubi_Websocket *)handle->data;
 }

 void Kubi_Websocket::on_async_close(uv_handle_t *handlet) {
	 uv_async_t *handle = (uv_async_t *)handlet;
	 free(handle);
	 handle = NULL;
 }
//#include<opencv2/opencv.hpp>
//#include<json/json.h>
//  
// Kubi_Websocket client;
//
// void imp_on_open(void *data) {
//	 printf("open\n");
// }
// void imp_no_err(void *data, int stage) {
//    printf("%d\n", stage);
// }
// void imp_on_close(void *data) {
//	 printf("close\n");
// }
// void imp_on_msg(void *data,const char *msg,unsigned int len) {
//	 printf("msg:%s\n",std::string(msg,len).c_str());
// }

 //void rc_net_station(void *data, KUBI_NET_STATION stage) {
	// printf("%d\n", stage);
 //
 //}

 //static bool help_isntspace(const char& ch) {
	// return !isspace(ch); 
 //}
 //static const std::string help_trim_str(const std::string& s) {
	//std::string::const_iterator iter1 = std::find_if(s.begin(), s.end(), help_isntspace);
	//std::string::const_iterator iter2 = find_if(s.rbegin(), s.rend(), help_isntspace).base();
	//return iter1 < iter2 ? std::string(iter1, iter2) : std::string("");
	//
 //}



// int main1(int argc, char **argv)
 //{

	 //std::map<std::string, std::string> keys;
	 //keys.insert(std::pair<std::string, std::string>("Upgrade", ""));
	 //keys.insert(std::pair<std::string, std::string>("Connection", ""));
	 //keys.insert(std::pair<std::string, std::string>("Sec-WebSocket-Accept", ""));

	 //std::string sec_websocket_accept = "";


	 //std::string buff = "";
	 //std::string data = "";
	 //data="HTTP/1.1 101 Switching Protocols\r\n";
	 //data += "Upgrade: websocket\r\n";
	 //data += "Connection: Upgrade\r\n";
	 //data += "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n\r\n";
	
	 //buff.append(data.data(), data.length());
	 //int len = buff.length();
	 //if (len >=127) {
		// char data_end[5];
		// memcpy(data_end,buff.data()+(buff.length()-4),4);
		// data_end[4] = '\0';
		// bool if_find_end = false;
		// if (strcmp(data_end,"\r\n\r\n") == 0) {
		//	 if_find_end = true;
		// }
		// bool if_find_head = false;
		// if (if_find_end) {
		//	 char data_head[5];
		//	 memcpy(data_head, buff.data() + 0, 4);
		//	 data_head[4] = '\0';
		//	 if (strcmp(data_head, "HTTP") == 0) {
		//		 if_find_head = true;
		//	 }
		// }
		// if (if_find_head&&if_find_end) {
		//	int offect = 0;
		//	int ind = buff.find("\r\n", offect);
		//	if (ind != -1) {
		//		std::string header = buff.substr(offect, ind- offect);
		//		bool if_header_fit = false;
		//		if (header == "HTTP/1.1 101 Switching Protocols") {
		//			if_header_fit = true;
		//		}
		//		if (if_header_fit) {
		//			
		//			while (1) {
		//				offect = ind + 2;
		//				ind=buff.find("\r\n", offect);
		//				if (ind != -1) {
		//					std::string item = buff.substr(offect, ind- offect);
		//					int t_ind=item.find(":");
		//					std::string key= item.substr(0, t_ind);
		//					if (key.length() > 0) {
		//						std::string value = item.substr(t_ind + 1, item.length() - (t_ind + 1));
		//						value = help_trim_str(value);
		//						keys[key] = value;
		//						printf("%s,%s\n", key.c_str(), value.c_str());
		//					}
		//				}
		//				else {
		//					break;
		//				}

		//			}
		//			bool if_all_key_fit = true;
		//			std::map<std::string,std::string>::iterator it;
		//			for (it = keys.begin(); it != keys.end(); it++)
		//			{
		//				if (it->first == "Upgrade") {
		//					if (it->second!="websocket"){
		//						if_all_key_fit = false;
		//						break;
		//					}
		//				}
		//				if (it->first == "Connection") {
		//					if (it->second != "Upgrade") {
		//						if_all_key_fit = false;
		//						break;
		//					}
		//				}
		//				if (it->first == "Sec-WebSocket-Accept") {
		//					if (it->second != sec_websocket_accept) {
		//						if_all_key_fit = false;
		//						break;
		//					}
		//				}
		//			}
		//			if (if_all_key_fit) {
		//				return true;
		//			}
		//			
		//		}
		//	}
		// }
		//
	 //}
	 //if (len > 127 + 64) {
	 //   //err!
	 //}
	 //return false;
	 //client.Start("127.0.0.1", 8080);
	 //client.set_open_rc(imp_on_open);
	 //client.set_err_rc(imp_no_err);
	 //client.set_close_rc(imp_on_close);
	 //client.set_recv_rc(imp_on_msg);
	 //while (1) {
		// client.BackToThread();
		// cv::waitKey(1);
	 //}
	 //getchar();
	 //return 0;
// }