#include"kubi_websocket_tool.h"
#include"kubi_websocket.h"

websocket_parser_settings * Kubi_Websocket_tool:: ws_settings = NULL;
void Kubi_Websocket_tool::ws_init_settings() {
	ws_settings =(websocket_parser_settings*) malloc(sizeof(websocket_parser_settings));
	websocket_parser_settings_init(ws_settings);
	ws_settings->on_frame_header = ws_frame_header;
	ws_settings->on_frame_body = ws_frame_body;
	ws_settings->on_frame_end = ws_frame_end;
}
void Kubi_Websocket_tool::ws_init_parser(websocket_parser *ws_parser, void *data) {
	websocket_parser_init(ws_parser);
	ws_parser->data = data;
}

int Kubi_Websocket_tool::ws_frame_header(websocket_parser * parser) {
	Kubi_Websocket_tool *ws = (Kubi_Websocket_tool *)parser->data;
	websocket_flags op= (websocket_flags)(parser->flags & WS_OP_MASK);
	ws->final_code= parser->flags & WS_FIN;
	if (op != WS_OP_CONTINUE) {
		ws->opcode = op;
	}
	return 0;
}
int Kubi_Websocket_tool::ws_frame_body(websocket_parser * parser, const char *at, size_t size) {
	Kubi_Websocket_tool *ws = (Kubi_Websocket_tool *)parser->data;
	ws->mask_code= parser->flags & WS_HAS_MASK;
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
int Kubi_Websocket_tool::ws_frame_end(websocket_parser * parser) {
	Kubi_Websocket_tool *ws = (Kubi_Websocket_tool *)parser->data;
	if (ws->final_code == WS_FINAL_FRAME){
		BrNode *k = new BrNode();
		k->data = std::string(ws->data.data(), ws->data.length());
		if (ws->opcode == WS_OP_CLOSE){
			k->type=Kubi_Websocket_tool::PT_WS_CLOSE;
		}
		else if(ws->opcode == WS_OP_PING){
			k->type = Kubi_Websocket_tool::PT_WS_PING;
		}
		else if (ws->opcode == WS_OP_PONG){
			k->type = Kubi_Websocket_tool::PT_WS_PONG;
		}
		else if (ws->opcode == WS_OP_TEXT){
			k->type = Kubi_Websocket_tool::PT_WS_TEX;
		}
		else if (ws->opcode == WS_OP_BINARY){
			k->type = Kubi_Websocket_tool::PT_WS_BIN;
		}
		else {
			k->type = Kubi_Websocket_tool::PT_WS_ERR;
		}
		ws->read_buffs->push_back(k);
		ws->data.clear();
	}
	return 0;
}
void Kubi_Websocket_tool::help_parse_data(Kubi_Websocket_tool *s, const char *data, size_t len, std::vector<BrNode*> * read_buffs) {
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
std::vector<uv_buf_t*> Kubi_Websocket_tool::help_make_data( char *data, size_t len) {
	int cout = 0;
	int ind = 0;
	int t_len = len;
	std::vector<uv_buf_t*> f_t = std::vector<uv_buf_t*>();
	while (1) {
		if (Kubi_utils::C_FRAME_MAX_SIZE >0 )
		{
			int new_len = t_len - Kubi_utils::C_FRAME_MAX_SIZE;
			uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
			uint8_t mask[4];
			help_make_random(mask, sizeof(mask));
			if (new_len >= 0) {
				websocket_flags flage = (websocket_flags)(WS_OP_BINARY | WS_HAS_MASK | WS_OP_CONTINUE);
				if (cout != 0) {
					flage = (websocket_flags)(WS_HAS_MASK |WS_OP_CONTINUE);
				}
				size_t frame_len = websocket_calc_frame_size(flage, Kubi_utils::C_FRAME_MAX_SIZE);
				char * frame = (char *)malloc(sizeof(char) * frame_len);
				websocket_build_frame(frame, (websocket_flags)(flage), (char*)mask, data + ind, Kubi_utils::C_FRAME_MAX_SIZE);
				luv_writebuffer->len = frame_len;
				luv_writebuffer->base = frame;
				f_t.push_back(luv_writebuffer);
				ind = Kubi_utils::C_FRAME_MAX_SIZE;
			}
			else {
				websocket_flags flage = (websocket_flags)(WS_OP_BINARY | WS_HAS_MASK | WS_FINAL_FRAME);
				if (cout != 0) {
					flage = (websocket_flags)(WS_HAS_MASK | WS_FINAL_FRAME);
				}
				size_t frame_len = websocket_calc_frame_size(flage, t_len);
				char * frame = (char *)malloc(sizeof(char) * frame_len);
				websocket_build_frame(frame, flage, (char*)mask, data + ind, t_len);
				luv_writebuffer->len = frame_len;
				luv_writebuffer->base = frame;
				f_t.push_back(luv_writebuffer);
				ind = t_len;
				break;
			}
			t_len = new_len;
			cout++;
		}
		else {
			websocket_flags flage = (websocket_flags)(WS_OP_BINARY | WS_HAS_MASK | WS_FINAL_FRAME);
			uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
			uint8_t mask[4];
			help_make_random(mask, sizeof(mask));
			size_t frame_len = websocket_calc_frame_size(flage, t_len);
			char * frame = (char *)malloc(sizeof(char) * frame_len);
			websocket_build_frame(frame, flage, (char*)mask, data , t_len);
			luv_writebuffer->len = frame_len;
			luv_writebuffer->base = frame;
			f_t.push_back(luv_writebuffer);
			
			break;
		}
		
	}
	return f_t;
}

uv_buf_t* Kubi_Websocket_tool::help_make_pone(const char *data, size_t len) {
	uint8_t mask[4];
	help_make_random(mask, sizeof(mask));
	uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	size_t frame_len = websocket_calc_frame_size((websocket_flags)(WS_OP_PONG| WS_FINAL_FRAME | WS_HAS_MASK), len);
	char * frame = (char *)malloc(sizeof(char) * frame_len);
	websocket_build_frame(frame, (websocket_flags)(WS_OP_PONG| WS_FINAL_FRAME | WS_HAS_MASK), (char*)mask, data, len);
	luv_writebuffer->len = frame_len;
	luv_writebuffer->base = frame;
	return luv_writebuffer;
}
uv_buf_t* Kubi_Websocket_tool::help_make_ping(const char *data, size_t len) {
	uint8_t mask[4];
	help_make_random(mask, sizeof(mask));
	uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	size_t frame_len = websocket_calc_frame_size((websocket_flags)(WS_OP_PING| WS_FINAL_FRAME | WS_HAS_MASK), len);
	char * frame = (char *)malloc(sizeof(char) * frame_len);
	websocket_build_frame(frame, (websocket_flags)(WS_OP_PING| WS_FINAL_FRAME | WS_HAS_MASK), (char*)mask, data, len);
	luv_writebuffer->len = frame_len;
	luv_writebuffer->base = frame;
	return luv_writebuffer;
}
//uv_buf_t* Kubi_Websocket_tool::help_make_pone(char *data, size_t len) {
//	uint8_t mask[4];
//	help_make_random(mask, sizeof(mask));
//	uv_buf_t *luv_writebuffer = (uv_buf_t*)malloc(sizeof(uv_buf_t));
//	size_t frame_len = websocket_calc_frame_size((websocket_flags)(WS_OP_PONG | WS_FINAL_FRAME | WS_HAS_MASK), len);
//	char * frame = (char *)malloc(sizeof(char) * frame_len);
//	websocket_build_frame(frame, (websocket_flags)(WS_OP_PONG | WS_FINAL_FRAME | WS_HAS_MASK), (char*)mask, data, frame_len);
//	luv_writebuffer->len = frame_len;
//	luv_writebuffer->base = frame;
//	return luv_writebuffer;
//}

 void Kubi_Websocket_tool::help_make_random(void *buffer, size_t len)
{
	uint8_t *bytes = (uint8_t*)buffer;
	uint8_t *bytes_end = bytes + len;
	for (; bytes < bytes_end; bytes++)
		*bytes = rand() & 0xff;
}

std::string Kubi_Websocket_tool::help_client_make_handshake(Kubi_Websocket_tool *s,std::string wk) {
	static char send_client[512];
	static char *wb_accept_p_01 =
		"GET /chat HTTP/1.1\r\n"
		"Host: can.kubi.com\r\n"
		"Upgrade: websocket\r\n"
		"Sec-WebSocket-Key: ";
	static char *wb_accept_p_02 =
		"\r\n"
		"Origin: http://haha.can.kubi.com\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"\r\n";
	sprintf(send_client, "%s%s%s", wb_accept_p_01, wk.c_str(), wb_accept_p_02);
	return send_client;
}
static bool help_isntspace(const char& ch) {
	return !isspace(ch);
}

static const std::string help_trim_str(const std::string& s) {
	std::string::const_iterator iter1 = std::find_if(s.begin(),s.end(),help_isntspace);
	std::string::const_iterator iter2 = find_if(s.rbegin(),s.rend(),help_isntspace).base();
	return iter1 < iter2 ? std::string(iter1, iter2) : std::string("");
}
int Kubi_Websocket_tool::help_parse_client_handshake(Kubi_Websocket_tool *s, const char *data, size_t lent) {
	s->data.append(data, lent);
	int len = s->data.length();
	if (len < 127) {
		return -1;
	}
	if (len >= 127&& len<127+64) {
		char data_end[5];
		memcpy(data_end, s->data.data() + (s->data.length() - 4), 4);
		data_end[4] = '\0';
		bool if_find_end = false;
		if (strcmp(data_end, "\r\n\r\n") == 0) {
			if_find_end = true;
		}
		bool if_find_head = false;
		if (if_find_end) {
			char data_head[5];
			memcpy(data_head, s->data.data() + 0, 4);
			data_head[4] = '\0';
			if (strcmp(data_head, "HTTP") == 0) {
				if_find_head = true;
			}
		}
		if (if_find_head&&if_find_end) {
			int offect = 0;
			int ind = s->data.find("\r\n", offect);
			if (ind != -1) {
				std::string header = s->data.substr(offect, ind - offect);
				bool if_header_fit = false;
				if (header == "HTTP/1.1 101 Switching Protocols") {
					if_header_fit = true;
				}
				if (if_header_fit) {
					std::map<std::string, std::string> keys;
					keys.insert(std::pair<std::string, std::string>("Upgrade", ""));
					keys.insert(std::pair<std::string, std::string>("Connection", ""));
					keys.insert(std::pair<std::string, std::string>("Sec-WebSocket-Accept", ""));
					while (1) {
						offect = ind + 2;
						ind = s->data.find("\r\n", offect);
						if (ind != -1) {
							std::string item = s->data.substr(offect, ind - offect);
							int t_ind = item.find(":");
							std::string key = item.substr(0, t_ind);
							if (key.length() > 0) {
								std::string value = item.substr(t_ind + 1, item.length() - (t_ind + 1));
								value = help_trim_str(value);
								keys[key] = value;
							}
						}
						else {
							break;
						}
					}
					bool if_all_key_fit = true;
					std::map<std::string, std::string>::iterator it;
					for (it = keys.begin(); it != keys.end(); it++)
					{
						if (it->first == "Upgrade") {
							if (it->second != "websocket") {
								if_all_key_fit = false;
								break;
							}
						}
						if (it->first == "Connection") {
							if (it->second != "Upgrade") {
								if_all_key_fit = false;
								break;
							}
						}
						if (it->first == "Sec-WebSocket-Accept") {
							if (it->second != s->sec_websocket_accept) {
								if_all_key_fit = false;
								break;
							}
						}
					}
					if (if_all_key_fit) {
						return 0;
					}
				}
			}
		}
	}
	return 1;
}
