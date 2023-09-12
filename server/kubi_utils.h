
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
		PT_HEART = 0,//����
		PT_ENTER,//��½
		PT_REQUEST,//����
		PT_NOTICE,//����֪ͨ
		PT_EXIT,//�˳�
		PT_WS_TEX,
		PT_WS_BIN,
		PT_WS_PING,
		PT_WS_PONG,
		PT_WS_CLOSE,
		PT_WS_ERR
	};

	static int C_HEART_TIME;//�������������
	static int C_TMP_MAX_SIZE;//��������������ռ䣬��������ռ�û���ҵ����ʵ�Э�� ��ᶪ��Э���
	static int C_STAY_BUT_NOT_LOGIN_TIME;//�����ϵ������е�½������ͣ��ʱ��
	static int C_HANDSHAKE_MAX_SIZE;//������������ֽ���
	static int C_FRAME_MAX_SIZE;

	static std::string utf8_2_gb2312(const char* utf8);
	static std::string gb2312_2_utf8(const char* gb2312);
	static std::string utf8_2_ansi(const std::string & str);
	static std::string ansi_2_utf8(const std::string & str);

	
};

#endif