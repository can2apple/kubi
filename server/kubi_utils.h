
#ifndef _h_can_Kubillity_utils_
#define _h_can_Kubillity_utils_
extern "C" {
	#include <uv.h>
	#include "crypto/sha1.h"
	#include "crypto/base64_encoder.h"
    #include "http_parser.h"
	#include "websocket_parser.h"
}
#include <iostream>
#include <vector>
#include <map>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <time.h>
#include "Kubi.pb.h"

struct Kubi_utils
{
public:
	enum PACK_TYPE
	{
		PT_HEART = 0,//心跳
		PT_ENTER,//登陆
		PT_REQUEST,//请求
		PT_NOTICE,//正常通知
		PT_EXIT,//退出
		PT_WS_TEX,
		PT_WS_BIN,
		PT_WS_PING,
		PT_WS_PONG,
		PT_WS_CLOSE,
		PT_WS_ERR
	};

	static int C_HEART_TIME;//心跳包最大秒数
	static int C_TMP_MAX_SIZE;//交换缓存包的最大空间，超过这个空间没有找到合适的协议 则会丢弃协议包
	static int C_STAY_BUT_NOT_LOGIN_TIME;//连接上但不进行登陆的连接停留时间
	static int C_HANDSHAKE_MAX_SIZE;//检查握手最大的字节数
	static int C_FRAME_MAX_SIZE;

	static std::string utf8_2_gb2312(const char* utf8);
	static std::string gb2312_2_utf8(const char* gb2312);
	static std::string utf8_2_ansi(const std::string & str);
	static std::string ansi_2_utf8(const std::string & str);

	
};

#endif