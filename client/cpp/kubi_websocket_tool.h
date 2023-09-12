
#ifndef _h_can_Kubillity_ws_Node_
#define _h_can_Kubillity_ws_Node_
#include"kubi_utils.h"
class Kubi_Websocket;
class Kubi_Websocket_tool {
public:
	class BrNode
	{
	public:
		std::string data;
		int type;
	};
	enum PACK_TYPE
	{
		PT_CONNECT_FAILD=0,
		PT_WS_HAND_SUCC,
		PT_WS_HAND_FAILD,
		PT_WS_TEX,
		PT_WS_BIN,
		PT_WS_PING,
		PT_WS_PONG,
		PT_WS_CLOSE,
		PT_WS_ERR
	};
	Kubi_Websocket_tool() {
		static uint8_t key[16];
		int base64_len=0;
		help_make_random(key, sizeof(key));
		char* base_buf = base64_encode((uint8_t*)key, sizeof(key), &base64_len);
		this->web_socket_key = std::string(base_buf, base64_len);
		base64_encode_free(base_buf);

		static char* wb_migic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		static char key_migic[256];
		static char sha1_key_migic[SHA1_DIGEST_SIZE];
		int sha1_size = 0;
		sprintf(key_migic, "%s%s", web_socket_key.c_str(), wb_migic);
		crypt_sha1((unsigned char*)key_migic, strlen(key_migic), (unsigned char*)&sha1_key_migic, &sha1_size);
		base64_len = 0;
		base_buf = base64_encode((uint8_t*)sha1_key_migic, sha1_size, &base64_len);
		this->sec_websocket_accept = std::string(base_buf, base64_len);
		base64_encode_free(base_buf);
	}
   ~Kubi_Websocket_tool() {}
	websocket_parser ws_parser;
	int opcode = 0;
	int final_code = 0;
	int mask_code = 0;
	bool if_frame_ready = false;
	std::string data = "";
	std::vector<BrNode*> * read_buffs=NULL;
	Kubi_Websocket *link_sess=NULL;
	std::string web_socket_key = "";
	std::string sec_websocket_accept = "";
public:
	static void help_make_random(void *buffer, size_t len);
	static std::string help_client_make_handshake(Kubi_Websocket_tool *s, std::string wk);
	static int help_parse_client_handshake(Kubi_Websocket_tool *s, const char *data, size_t len);
	static void help_parse_data(Kubi_Websocket_tool *s, const char *data, size_t len,std::vector<BrNode*> * read_buffs);
	static std::vector<uv_buf_t*> help_make_data( char *data, size_t len);
	static uv_buf_t* help_make_pone(const char *data, size_t len);
	static uv_buf_t* help_make_ping(const char *data, size_t len);
	static void ws_init_settings();
	static void ws_init_parser(websocket_parser *ws_parser, void *data);
private:
	static websocket_parser_settings* ws_settings;
	static int ws_frame_header(websocket_parser * parser);
	static int ws_frame_body(websocket_parser * parser, const char *at, size_t size);
	static int ws_frame_end(websocket_parser * parser);
//------------------------------------------------------------------------------------------
	
	
};
#endif