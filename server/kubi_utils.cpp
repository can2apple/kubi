#include"kubi_utils.h"
#include <codecvt>  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int Kubi_utils::C_TMP_MAX_SIZE = 1024 * 1024 * 2;
int Kubi_utils::C_HEART_TIME = 4;
int Kubi_utils::C_STAY_BUT_NOT_LOGIN_TIME = 2;
int Kubi_utils::C_HANDSHAKE_MAX_SIZE = 1024;
int Kubi_utils::C_FRAME_MAX_SIZE= 65535-14-1;//wei:4+4+1+7+64+32//
//int Kubi_utils::C_FRAME_MAX_SIZE =-1;//wei:4+4+1+7+64+32//
static std::string can_unicode_2_ansi(const std::wstring & wstr)
{
	std::string ret;
	std::mbstate_t state = {};
	const wchar_t *src = wstr.data();
	size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< char[] > buff(new char[len + 1]);
		len = std::wcsrtombs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	return ret;
}
static std::wstring ansi_2_unicode(const std::string & str)
{
	std::wstring ret;
	std::mbstate_t state = {};
	const char *src = str.data();
	size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< wchar_t[] > buff(new wchar_t[len + 1]);
		len = std::mbsrtowcs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	return ret;
}
static std::string can_unicode_2_utf8(const std::wstring & wstr)
{
	std::string ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.to_bytes(wstr);
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}
static std::wstring can_utf8_2_unicode(const std::string & str)
{
	std::wstring ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.from_bytes(str);
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

 std::string Kubi_utils::utf8_2_gb2312(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	std::string  s = std::string(str);
	delete[] str;
	return s;
}
std::string Kubi_utils::gb2312_2_utf8(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	std::string  s = std::string(str);
	delete[] str;
	return s;
}

std::string Kubi_utils::utf8_2_ansi(const std::string & str)
{
	return can_unicode_2_ansi(can_utf8_2_unicode(str));
}
std::string Kubi_utils::ansi_2_utf8(const std::string & str)
{
	return can_unicode_2_utf8(ansi_2_unicode(str));
}
