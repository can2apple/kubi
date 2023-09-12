#include"kubi_ser_cpp11_imp.h"

#include<iostream>
#include<json/json.h>
#include <conio.h>
#include <glog/logging.h>


 class User
 {
 public:

	 std::string uid = "";
	 std::string name = "";
	 std::string role = "";
	 std::string to_json_str() {
		 Json::Value msg;
		 msg["uid"] = uid;
		 msg["name"] = name;
		 msg["role"] = role;
		 std::string res(msg.toStyledString());
		 return res;
	 };
	 bool parse_from_json_str(const std::string& js, const unsigned char* img_bytes, int len) {
		
		 Json::Reader reader;
		 Json::Value msg;
		 if (reader.parse(js, msg)) {
			 uid = msg["uid"].asCString();
			 name= msg["name"].asCString();
			 role = msg["role"].asCString();
			 return true;
		 }
		 return false;
	 }
 };

class TaskNode
{
public:
	 std::string opt = "null";
	 std::string task = "";
	 int stage = 0;//0 1 2 0:未开始 1成功 2失败

};

class Task
{
public:
	std::string uid="";
	TaskNode task;
	unsigned char* img_datas = NULL;
	unsigned int img_datas_ind = 0;
	std::vector<int> img_datas_des;
	std::vector<std::string*> img_datas_div;
	Task() {
		img_datas_des = std::vector<int>();
		img_datas_div = std::vector<std::string*>();
	}
	void push_bytes(unsigned char* data,int len)
	{
		std::string* tmp = new std::string((char*)data,len);
		img_datas_div.push_back(tmp);
		img_datas_des.push_back(tmp->length());
		if (img_datas != NULL)
		{
			img_datas = (unsigned char*)realloc(img_datas,img_datas_ind+len);
			memcpy(img_datas+ img_datas_ind, data, len);
			img_datas_ind += len;
		}
		else
		{
			img_datas =(unsigned char*)malloc(len) ;
			memcpy(img_datas,data,len);
			img_datas_ind = len;
		}
	}
	void empty_data() {
		if (img_datas != NULL) {
			free(img_datas);
			img_datas = NULL;
		}
		for (int i = 0; i < img_datas_div.size(); i++) {
			if (img_datas_div[i] != NULL) {
				free(img_datas_div[i]);
			}
		}
		img_datas_des.clear();
		img_datas_div.clear();
	}
	void set_task_stage(int stage) {
		 task.stage = stage;
	}
	std::string to_json_str(){
		Json::Value msg ;
		msg["uid"] = uid;
		Json::Value tasknode;
		tasknode["opt"] = task.opt;
		tasknode["task"] = task.task;
		tasknode["stage"] = task.stage;
		msg["task"] = tasknode;
		msg["img_des"] = Json::Value();
		for (int i = 0; i < img_datas_des.size(); i++){
			msg["img_des"].append(img_datas_des[i]);
		}
		std::string res(msg.toStyledString());
		return res;
	}
	bool parse_from_json_str(const std::string &json_str,const unsigned char* img_bytes,int len) {
		empty_data();
		img_datas = (unsigned char*)malloc(len);
		memcpy(img_datas, img_bytes, len);
		img_datas_ind = len;

		Json::Reader reader;
		Json::Value msg;
		if (reader.parse(json_str, msg)) {
			uid = msg["uid"].asCString();
			Json::Value tasknode = msg["task"];
			task.opt = tasknode["opt"].asCString();
			task.task = tasknode["task"].asCString();
			task.stage = tasknode["stage"].asInt();
			Json::Value img_des = msg["img_des"];
			for (int i = 0; i < img_des.size(); i++) {
				img_datas_des.push_back(img_des[i].asInt());
			}
			int ind_ = 0;
			for (int i = 0; i < img_datas_des.size(); i++) {
				int tmp_len = img_datas_des[i];
				std::string* tmp = new std::string((char*)(img_bytes+ ind_), tmp_len);
				img_datas_div.push_back(tmp);
				ind_ += tmp_len;
			}
			return true;
		}
		return false;
	}
	~Task() {
		empty_data();
	}
};

bool _help_bridge_node_to_json(std::string data_str, Json::Value &dst) {
	Json::Reader reader;
	if (reader.parse(data_str, dst)) {
		return true;
	}
	return false;
}


User * _help_add_user(std::vector<User> &_users, User &user) {
	 for(int i=0;i< _users.size();i++){
		 if (_users[i].uid == user.uid) {
			 return &_users[i];
		 }
	 }
	 _users.push_back(user);
	 return &_users[_users.size() - 1];
}

std::pair<bool, User> _help_del_user(std::vector<User>& _users,const std::string &uid) {
	int ind = -1;
	for (int i = 0; i < _users.size(); i++) {
		if (_users[i].uid == uid) {
			ind = i;
			break;
		}
	}
	if(ind!=-1){
		User tmp = _users[ind];
		std::vector<User>::iterator it = _users.begin() + ind;
		_users.erase(it);
		return std::pair<bool, User>(true, tmp);
	}
	return std::pair<bool, User>(false, User());
}

User* _help_get_user(std::vector<User>& _users, std::string &uid) {

	for (int i = 0; i < _users.size(); i++) {
		if (_users[i].uid==uid) {
			return &_users[i];
		}
	}
	return NULL;
}


Kubi_Ser_Cpp11_Imp ser;
std::vector<User> user_caculaters;
std::vector<User> user_shows;
std::vector<User> user_gatherers;


int main(int argc, char** argv) {



	//getchar();
	google::InitGoogleLogging(*argv);
	FLAGS_log_dir = "./log/";
	FLAGS_logtostderr = true;  //设置日志消息是否转到标准输出而不是日志文件 true只是cmd打印
	FLAGS_alsologtostderr = false;  //设置日志消息除了日志文件之外是否标准输出 false cmd不打印 error还是打印的
	FLAGS_colorlogtostderr = true;  //设置记录到标准输出的颜色消息（如果终端支持）
	FLAGS_log_prefix = true;  //设置日志前缀是否应该添加到每行输出
	FLAGS_logbufsecs = 10;  //设置可以缓冲日志的最大秒数，0指实时输出
	FLAGS_max_log_size = 100;  //设置最大日志文件大小（以MB为单位）
	FLAGS_stop_logging_if_full_disk = true;  //设置是否在磁盘已满时避免日志记录到磁盘


	ser.OnServerClose([]()->void {
		LOG(WARNING) << "Server close!";
		});
	ser.OnClientEnter([](const std::string& uid, const std::string& ip, int port)->void {
		LOG(WARNING) << "Kubi_Session has connect to server!";
		});
	ser.OnClientLeave([](const std::string& uid)->void {
		LOG(WARNING) << "Kubi_Session has Leave to server!";
		_help_del_user(user_caculaters, uid);
		_help_del_user(user_shows, uid);
		_help_del_user(user_gatherers, uid);
	

		});
	ser.RegistRequestRoute("on_login", [](const std::string& uid, int pkg_id, const std::string& data_str, const std::string& data_byts)->void {
		LOG(INFO) << "login";
		User tmp_user;
		tmp_user.parse_from_json_str(data_str, (unsigned char*)data_byts.data(), data_byts.size());
		if (tmp_user.role == "gatherer") {
			_help_add_user(user_gatherers, tmp_user);
		}
		else if (tmp_user.role == "caculater") {
			_help_add_user(user_caculaters, tmp_user);
		}
		else {
			_help_add_user(user_shows, tmp_user);
		}

		Json::Value v_rsp;
		v_rsp["code"] = 123;
		v_rsp["msg"] = "welcome";
		ser.Response("on_login", uid, pkg_id, v_rsp.toStyledString().c_str(), NULL, 0);
		
		});
	ser.RegistNotifyRoute("on_cap", [](const std::string& uid, const std::string& data_str, const std::string& data_byts)->void {
		if (user_caculaters.size() > 0) {
			for(int i=0; i<user_caculaters.size();i++){
				ser.Notify(user_caculaters[i].uid,"on_cap", data_str.c_str(), (unsigned char*)data_byts.data(), data_byts.size());
			}
		}
		
	});
	ser.RegistNotifyRoute("on_caculater_result", [](const std::string& uid, const std::string& data_str, const std::string& data_byts)->void {
		if (user_shows.size() > 0) {
			for (int i = 0; i < user_shows.size(); i++) {
				ser.Notify(user_shows[i].uid, "on_caculater_result", data_str.c_str(), (unsigned char*)data_byts.data(), data_byts.size());
			}
		}
		});


	
	ser.SetNotEnterStageTime(-1);
	ser.SetHeartTime(-1);
	ser.Start(8080, true);
	

	google::ShutdownGoogleLogging();
	return 0;
}

