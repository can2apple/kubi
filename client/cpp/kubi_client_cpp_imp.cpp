
#include"Kubi_client_cpp_imp.h"

void Kubi_Client::on_time_heart_start(void * data) {
	Kubi_Client *cn = (Kubi_Client*)data;
	while (1) {
		if (cn->heart_has_start == false) {
			break;
		}
		if (cn->heart_check_stage()) {
			cn->Close();
			break;
		}
		uv_sleep(1000);
	}
	cn->heart_has_end = true;
}
void Kubi_Client::imp_on_open(void *data) {
	Kubi_Client *cli = (Kubi_Client*)data;
	cli->_send_enter();
	//printf("open\n");
}
void Kubi_Client::imp_on_err(void *data, int stage) {
	Kubi_Client *cli = (Kubi_Client*)data;
	if (cli->_onerr != NULL) {
		cli->_onerr(stage);
	}
	if (cli->_onEnd != NULL) {
		if (cli->_ifOnEnd == false) {
			cli->heart_reg_stop();
			cli->_ifOnEnd = true;
			cli->_onEnd();
		}
		
	}
}
void Kubi_Client::imp_on_close(void *data) {
	Kubi_Client *cli = (Kubi_Client*)data;
	
	if (cli->_onclose != NULL) {
		cli->_onclose();
	}
	if (cli->_ifOnEnd == false) {
		cli->heart_reg_stop();
		cli->_ifOnEnd = true;
		cli->_onEnd();
	}
}
void Kubi_Client::imp_on_msg(void *data, const char *msg, unsigned int len) {
	Kubi_Client *cli = (Kubi_Client*)data;
	cli->_service(msg, len);
	//printf("msg:%s\n", std::string(msg, len).c_str());
}

