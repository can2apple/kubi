
#ifndef _h_can_Kubillity_ws_Node_
#define _h_can_Kubillity_ws_Node_
#include"kubi_utils.h"
class Kubi_Session;
class Kubi_ws {
public:
	Kubi_ws() {
	}
   ~Kubi_ws() {
	}
	http_parser hp_parser;
	websocket_parser ws_parser;
	bool is_shaker_ended = false;
	bool is_sec_key = false;
	std::string sec_key = "";
	int opcode = 0;
	int final_code = 0;
	int mask_code = 0;
	bool if_frame_ready = false;
	std::string data = "";
	std::vector<kubi::KNode*> * read_buffs=NULL;
	Kubi_Session *link_sess;

public:
	/**
	*code_change: 101 1002
	*/
	static std::string help_make_handshake(Kubi_ws *s, int code_change);
	static bool help_parse_handshake(Kubi_ws *s, const char *data, size_t len);
	static void help_parse_data(Kubi_ws *s, const char *data, size_t len,std::vector<kubi::KNode*> * read_buffs);
	static std::vector<uv_buf_t*> help_make_data( char *data, size_t len);
	static uv_buf_t* help_make_pong(const char *data, size_t len);
	static uv_buf_t* help_make_ping(const char *data, size_t len);
	static void hp_init_parser(http_parser *hp_parser, void *data);
	static void hp_init_settings();
	static void ws_init_settings();
	static void ws_init_parser(websocket_parser *ws_parser, void *data);
private:
	static websocket_parser_settings* ws_settings;
	static int ws_frame_header(websocket_parser * parser);
	static int ws_frame_body(websocket_parser * parser, const char *at, size_t size);
	static int ws_frame_end(websocket_parser * parser);
//------------------------------------------------------------------------------------------
	static http_parser_settings *http_settings;
	static int hp_on_message_begin(http_parser *parser);
	static int hp_on_headers_complete(http_parser *parser);
	static int hp_on_url(http_parser *parser, const char *at, size_t length);
	static int hp_on_header_field(http_parser *parser, const char *at, size_t length);
	static int hp_on_header_value(http_parser *parser, const char *at, size_t length);
	static int hp_on_body(http_parser *parser, const char *at, size_t length);
	static int hp_on_message_complete(http_parser *parser);
	
};
#endif