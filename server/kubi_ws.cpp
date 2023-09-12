#include"kubi_ws.h"
#include"kubi_session.h"

websocket_parser_settings * Kubi_ws:: ws_settings = NULL;
void Kubi_ws::ws_init_settings() {
	ws_settings =(websocket_parser_settings*) malloc(sizeof(websocket_parser_settings));
	websocket_parser_settings_init(ws_settings);
	ws_settings->on_frame_header = ws_frame_header;
	ws_settings->on_frame_body = ws_frame_body;
	ws_settings->on_frame_end = ws_frame_end;
}
void Kubi_ws::ws_init_parser(websocket_parser *ws_parser, void *data) {
	websocket_parser_init(ws_parser);
	ws_parser->data = data;
}
int Kubi_ws::ws_frame_header(websocket_parser * parser) {
	Kubi_ws *ws = (Kubi_ws *)parser->data;
	websocket_flags op= (websocket_flags)(parser->flags & WS_OP_MASK);
	ws->final_code= parser->flags & WS_FIN;
	ws->mask_code = parser->flags & WS_HAS_MASK;
	
	if (op != WS_OP_CONTINUE) {
		ws->opcode = op;
	}
	return 0;
}
int Kubi_ws::ws_frame_body(websocket_parser * parser, const char *at, size_t size) {
	Kubi_ws *ws = (Kubi_ws *)parser->data;
	//ws->mask_code= parser->flags & WS_HAS_MASK;
	if (ws->mask_code== WS_HAS_MASK) {
		char *dst = (char*)malloc(size);
		websocket_parser_decode(dst, at, size, parser);
		ws->data.append(dst, size);
		free(dst);
	}
	else {
		ws->data.append(at,size);
	}
	return 0;
}
int Kubi_ws::ws_frame_end(websocket_parser * parser) {
	Kubi_ws *ws = (Kubi_ws *)parser->data;
	if (ws->final_code == WS_FINAL_FRAME) {
		//printf("ws_frame_end\n");
		//浏览器或许会发送这些信息
		kubi::KNode* k = new kubi::KNode();
		k->set_data_bytes("");
		k->set_data_str("");
		k->set_pkg_id(0);
		k->set_uid(ws->link_sess->GetUid());
		k->set_route("");
		
		if (ws->opcode == WS_OP_CLOSE) {
		
			k->set_data_str(std::string(ws->data.data(), ws->data.length()));
			k->set_pkg_type(Kubi_utils::PT_WS_CLOSE);
		}
		else if(ws->opcode == WS_OP_PING){
			k->set_data_str(std::string(ws->data.data(), ws->data.length()));
			k->set_pkg_type(Kubi_utils::PT_WS_PING);
		}
		else if (ws->opcode == WS_OP_PONG) {
			k->set_data_str(std::string(ws->data.data(), ws->data.length()));
			k->set_pkg_type(Kubi_utils::PT_WS_PONG);
		
		}
		else if (ws->opcode == WS_OP_TEXT) {
		
			k->set_data_str(std::string(ws->data.data(), ws->data.length()));
			k->set_pkg_type(Kubi_utils::PT_WS_TEX);
		}
		//----------------------------
		else if (ws->opcode == WS_OP_BINARY) {
			
			if (k->ParseFromArray(ws->data.data(), ws->data.length())) {
			}
			else {
				k->set_pkg_type(Kubi_utils::PT_WS_ERR);
			}
		}
		else {
			k->set_pkg_type(Kubi_utils::PT_WS_ERR);
		}
		ws->read_buffs->push_back(k);
		ws->data.clear();
	
	}
	return 0;
}
void Kubi_ws::help_parse_data(Kubi_ws *s, const char *data, size_t len, std::vector<kubi::KNode*> * read_buffs) {

	int nreaded = 0;
	int nreading = len;
	int nread_offect = 0;
	s->read_buffs = read_buffs;
	while (1) {
		nreaded = websocket_parser_execute(&(s->ws_parser), ws_settings, data + nread_offect, nreading);
		nread_offect += nreaded;
		nreading = nreading - nreaded;
		if (nreading <= 0) {
			break;
		}
	}
}
std::vector<uv_buf_t*> Kubi_ws::help_make_data( char *data, size_t len) {
	int cout = 0;
	int ind = 0;
	int t_len = len;
	std::vector<uv_buf_t*> f_t = std::vector<uv_buf_t*>();
	while (1) {
		if (Kubi_utils::C_FRAME_MAX_SIZE > 0) 
		{
			int new_len = t_len - Kubi_utils::C_FRAME_MAX_SIZE;
			uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
			if (new_len >= 0) {
				websocket_flags flage = (websocket_flags)(WS_OP_BINARY | WS_OP_CONTINUE);
				if (cout != 0) {
					flage = (websocket_flags)(  WS_OP_CONTINUE);
				}
				size_t frame_len = websocket_calc_frame_size(flage, Kubi_utils::C_FRAME_MAX_SIZE);
				char * frame = (char *)malloc(sizeof(char) * frame_len);
				websocket_build_frame(frame, flage, NULL, data + ind, Kubi_utils::C_FRAME_MAX_SIZE);
				luv_writebuffer->len = frame_len;
				luv_writebuffer->base = frame;
				f_t.push_back(luv_writebuffer);
				ind += Kubi_utils::C_FRAME_MAX_SIZE;

			}
			else {
				websocket_flags flage = (websocket_flags)(WS_OP_BINARY | WS_FINAL_FRAME);
				if (cout != 0) {
					flage = (websocket_flags)(WS_FINAL_FRAME);
				}
				size_t frame_len = websocket_calc_frame_size(flage, t_len);
				char * frame = (char *)malloc(sizeof(char) * frame_len);
				websocket_build_frame(frame, flage, NULL, data + ind, t_len);
				luv_writebuffer->len = frame_len;
				luv_writebuffer->base = frame;
				f_t.push_back(luv_writebuffer);
				ind += t_len;
				break;
			}
			t_len = new_len;
			cout++;
		}
		else {
			websocket_flags flage = (websocket_flags)(WS_OP_BINARY | WS_FINAL_FRAME);
			uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
			size_t frame_len = websocket_calc_frame_size(flage, t_len);
			char * frame = (char *)malloc(sizeof(char) * frame_len);
			websocket_build_frame(frame, flage, NULL, data, t_len);
			luv_writebuffer->len = frame_len;
			luv_writebuffer->base = frame;
			f_t.push_back(luv_writebuffer);
			break;
		}
		
	}
	return f_t;
}
uv_buf_t* Kubi_ws::help_make_pong(const char *data, size_t len) {
	uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	size_t frame_len = websocket_calc_frame_size((websocket_flags)(WS_OP_PONG | WS_FINAL_FRAME), len);
	char * frame = (char *)malloc(sizeof(char) * frame_len);
	websocket_build_frame(frame, (websocket_flags)(WS_OP_PONG  | WS_FINAL_FRAME), NULL, data, len);
	luv_writebuffer->len = frame_len;
	luv_writebuffer->base = frame;
	return luv_writebuffer;
}
uv_buf_t* Kubi_ws::help_make_ping(const char *data, size_t len) {
	uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	size_t frame_len = websocket_calc_frame_size((websocket_flags)(WS_FINAL_FRAME |WS_OP_PING), len);
	char * frame = (char *)malloc(sizeof(char) * frame_len);
	websocket_build_frame(frame, (websocket_flags)(WS_FINAL_FRAME | WS_OP_PING), NULL, data, len);
	luv_writebuffer->len = frame_len;
	luv_writebuffer->base = frame;
	return luv_writebuffer;
}
http_parser_settings * Kubi_ws::http_settings =NULL;
std::string Kubi_ws::help_make_handshake(Kubi_ws *s, int code_change) {
	static char *wb_accept_p_01 =
		"HTTP/1.1 ";
	static char *wb_accept_p_02 =
		" Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Accept: ";

	static char *wb_accept_e =
		"\r\n"
		"WebSocket-Protocol:chat\r\n\r\n";
	static char* wb_migic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	static char key_migic[256];
	static char sha1_key_migic[SHA1_DIGEST_SIZE];
	static char send_client[256];

	int sha1_size;
	//printf("%s\n", s->sec_key.c_str());
	sprintf(key_migic, "%s%s", s->sec_key.c_str(), wb_migic);
	crypt_sha1((unsigned char*)key_migic, strlen(key_migic), (unsigned char*)&sha1_key_migic, &sha1_size);
	int base64_len;
	char* base_buf = base64_encode((uint8_t*)sha1_key_migic, sha1_size, &base64_len);
	sprintf(send_client, "%s%d%s%s%s", wb_accept_p_01, code_change, wb_accept_p_02, base_buf, wb_accept_e);
	base64_encode_free(base_buf);
	return send_client;

}
bool Kubi_ws::help_parse_handshake(Kubi_ws *s, const char *data, size_t len){
	int readed=http_parser_execute(&s->hp_parser, http_settings, data, len);
	if (s->is_sec_key == true && s->sec_key != ""&&s->is_shaker_ended) {
		return true;
	}
	return false;
}
void Kubi_ws::hp_init_settings() {
	http_settings =(http_parser_settings*) malloc(sizeof(http_parser_settings));
	http_settings->on_message_begin = hp_on_message_begin;
	http_settings->on_url = hp_on_url;
	http_settings->on_header_field = hp_on_header_field;
	http_settings->on_header_value = hp_on_header_value;
	http_settings->on_headers_complete = hp_on_headers_complete;
	http_settings->on_body = hp_on_body;
	http_settings->on_message_complete = hp_on_message_complete;
}
void Kubi_ws::hp_init_parser(http_parser *hp_parser, void *data) {
	hp_parser->data = data;
	http_parser_init(hp_parser, HTTP_REQUEST);

}
int Kubi_ws::hp_on_message_begin(http_parser *parser) {
	return 0;
}
int Kubi_ws::hp_on_url(http_parser *parser, const char *at, size_t length) {
	return 0;
}
int Kubi_ws::hp_on_header_field(http_parser *parser, const char *at, size_t length) {
	Kubi_ws *ws = (Kubi_ws *)parser->data;
	if (strncmp(at, "Sec-WebSocket-Key", length) == 0) {
		ws->is_sec_key = true;
	}
	return 0;
}
int Kubi_ws::hp_on_header_value(http_parser *parser, const char *at, size_t length) {
	Kubi_ws *ws = (Kubi_ws *)parser->data;
	//printf("%d,%s\n", ws->is_sec_key, std::string(at, length).c_str());
	if (ws->is_sec_key&&ws->sec_key=="") {
		ws->sec_key = std::string(at, length);
	}
	return 0;
}
int Kubi_ws::hp_on_body(http_parser *parser, const char *at, size_t length) {
	return 0;
}
int Kubi_ws::hp_on_headers_complete(http_parser *parser) {
	return 0;
}
int Kubi_ws::hp_on_message_complete(http_parser *parser) {
	Kubi_ws *ws = (Kubi_ws *)parser->data;
	ws->is_shaker_ended = true;
	return 0;
}