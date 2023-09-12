
#include"Kubi_client_cpp_imp.h"


#include<opencv2/opencv.hpp>
#include<json/json.h>

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
			name = msg["name"].asCString();
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
	int stage = 0;

};

class Task
{
public:
	std::string uid = "";
	TaskNode task;
	unsigned char* img_datas = NULL;
	unsigned int img_datas_ind = 0;
	std::vector<int> img_datas_des;
	std::vector<std::string*> img_datas_div;
	Task() {
		img_datas_des = std::vector<int>();
		img_datas_div = std::vector<std::string*>();
	}
	void push_bytes(unsigned char* data, int len)
	{
		std::string* tmp = new std::string((char*)data, len);
		img_datas_div.push_back(tmp);
		img_datas_des.push_back(tmp->length());
		if (img_datas != NULL)
		{
			img_datas = (unsigned char*)realloc(img_datas, img_datas_ind + len);
			memcpy(img_datas + img_datas_ind, data, len);
			img_datas_ind += len;
		}
		else
		{
			img_datas = (unsigned char*)malloc(len);
			memcpy(img_datas, data, len);
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
	std::string to_json_str() {
		Json::Value msg;
		msg["uid"] = uid;
		Json::Value tasknode;
		tasknode["opt"] = task.opt;
		tasknode["task"] = task.task;
		tasknode["stage"] = task.stage;
		msg["task"] = tasknode;
		msg["img_des"] = Json::Value();
		for (int i = 0; i < img_datas_des.size(); i++) {
			msg["img_des"].append(img_datas_des[i]);
		}
		std::string res(msg.toStyledString());
		return res;
	}
	bool parse_from_json_str(const std::string& json_str, const unsigned char* img_bytes, int len) {
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
				std::string* tmp = new std::string((char*)(img_bytes + ind_), tmp_len);
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

bool _help_bridge_node_to_json(std::string data_str, Json::Value& dst) {
	Json::Reader reader;
	if (reader.parse(data_str, dst)) {
		return true;
	}
	return false;
}

int main_test_task(int argc, char** argv) {

	cv::Mat src = cv::Mat (512, 512, CV_8UC3, cv::Scalar(255,0,0));
	cv::imshow("src", src);


	std::vector<uchar> data_encode;
	cv::imencode(".jpg", src, data_encode);
	Task task01;
	task01.push_bytes(data_encode.data(), data_encode.size());
	
	Task task02;
	task02.parse_from_json_str(task01.to_json_str(), task01.img_datas, task01.img_datas_ind);
	std::vector<uchar> buff = std::vector<uchar>(task02.img_datas_div[0]->size());
	memcpy(buff.data(), task02.img_datas_div[0]->data(), task02.img_datas_div[0]->size());
	cv::Mat  img = cv::imdecode(buff, CV_LOAD_IMAGE_COLOR);
	cv::imshow("img", img);


	cv::waitKey(0);
	getchar();
	return 0;
}

int help_div(const char* str1, const char* delims, char*** dst) {
	int count = 0;
	char** dst_tmp = NULL;
	int dst_tmp_len = 0;
	char* dst_tmp_each = NULL;
	char* result = NULL;
	char* str = (char*)malloc(strlen(str1) + 1);
	memcpy(str, str1, strlen(str1) + 1);
	result = strtok(str, delims);
	dst_tmp = (char**)malloc(sizeof(char*) * 20);
	dst_tmp_len = 20;
	while (result != NULL) {
		if (dst_tmp_len <= count) {
			dst_tmp = (char**)realloc(dst_tmp, sizeof(char*) * (dst_tmp_len + 5));
			dst_tmp_len += 5;
		}
		dst_tmp_each = (char*)malloc(sizeof(char) * (strlen(result) + 1));
		memcpy(dst_tmp_each, result, strlen(result) + 1);
		dst_tmp[count] = dst_tmp_each;
		//-------------------------------------------------------
		count++;
		result = strtok(NULL, delims);

	}
	(*dst) = dst_tmp;
	free(str);
	return count;
}
int help_get_file_delim(const char* src) {
	std::string flage01(src);
	int flageId = flage01.find_last_of("/");
	if (flageId >= 0) {
		return 1;
	}
	flageId = flage01.find_last_of("\\");
	if (flageId >= 0) {
		return 2;
	}
	return 0;
}
std::string HelpGetAppDir() {
	char** list_str = NULL;
	int count = 0;
	int i = 0;
	char dst_tmp[512];
	char dst[512];
	memcpy(dst_tmp, _pgmptr, strlen(_pgmptr) + 1);
	int f = help_get_file_delim(dst_tmp);
	std::string delim = "/";
	if (f == 2) {
		delim = "\\";
	}
	count = help_div(dst_tmp, delim.c_str(), &list_str);
	if (count > 0) {
		sprintf(dst, "%s", list_str[0]);
		for (i = 1; i < count - 1; i++) {
			sprintf(dst, "%s/%s", dst, list_str[i]);
		}
	}
	for (int i = 0; i < count; i++) {
		free(list_str[i]);
	}
	free(list_str);
	return std::string(dst);
}


Kubi_Client client;
User user;
bool if_has_connect = false;
int main(int argc, char **argv) {


	user.uid = "from_cpp";
	user.name = "cpp";
	user.role = "gatherer";
	client.Start(user.uid, "ws://127.0.0.1", 8080);
	client.OnEnter([]()->void{
		client.Request("on_login", user.to_json_str().c_str(), NULL, 0, [](const kubi::KNode* in) ->void {
			if_has_connect = true;
			Json::Value resb;
			_help_bridge_node_to_json(in->data_str(), resb);
			printf("%s\n", resb["msg"].asCString());
		});
		client.OnNotice("on_msg", [](const kubi::KNode* in) ->void {
			//do you want:
		});
	
		
	});
	
	client.OnEnd([]()->void {
		if_has_connect = false;
		printf("OnEnd...\n");
		client.TryToConnect();
	});


	cv::VideoCapture cap;
	cap.open(HelpGetAppDir()+"/jntm.mp4");
	int video_counter = 0;
	while (1) {
		client.Loop();
		cv::Mat frame;
		if (cap.read(frame)) {
			video_counter += 1;
			if (video_counter == int(cap.get(cv::CAP_PROP_FRAME_COUNT) - 1)) {
				video_counter = 0;
				cap.set(cv::CAP_PROP_POS_FRAMES, 0);
				continue;
			}
			cv::imshow("video", frame);
			if (if_has_connect) {
				std::vector<uchar> data_encode;
				cv::imencode(".jpg", frame, data_encode);
				Task task01;
				task01.task.task = "cap_frame";
				task01.push_bytes(data_encode.data(), data_encode.size());
				client.Notice("on_cap", task01.to_json_str().c_str(), task01.img_datas, task01.img_datas_ind);

			}
		}

		
		int key=cv::waitKey(10);
		if (key == 27) {
			break;
		}
	}
	cv::destroyWindow("video");
	return 0;
}