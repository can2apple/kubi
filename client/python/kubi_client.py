import websocket
import threading
import _thread
import time
from Kubi_pb2 import KNode
class PACK_TYPE:
    PT_HEART = 0
    PT_ENTER=1
    PT_REQUEST=2
    PT_NOTICE=3
    PT_EXIT=4
class BR_TYPE:
    PT_OPEN = 0
    PT_MSG = 1
    PT_ERR=2
    PT_CLOSE=3

class KNodeImp:
    def __init__(self):
        # self.uid=""
        # self.pkg_type =PACK_TYPE.PT_HEART
        # self.pkg_id =0
        # self.route =""
        self.data_str = ""
        self.data_bytes = b''
        pass

    def FromkNode(self, kn):
        # self.uid = kn.uid
        # self.pkg_type = kn.pkg_type
        # self.pkg_id = kn.pkg_id
        # self.route = kn.route
        self.data_str = kn.data_str
        self.data_bytes = kn.data_bytes
        pass
    # def TokNode(self):
    #     kn=KNode()
    #     # kn.uid = self.uid
    #     # kn.pkg_type =self.pkg_type
    #     # kn.pkg_id = self.pkg_id
    #     # kn.route = self.route
    #     kn.data_str =self.data_str
    #     kn.data_bytes = self.data_bytes
    #     return kn
        pass
    pass
class Kubi_Websocket:
    @staticmethod
    def __on_open(ws):
        tar = ws.__dict__["target"]
        tmp={"stype":int(BR_TYPE.PT_OPEN),"data":0}
        tar.__add_data(tmp)
        tar.__stage = 1
    @staticmethod
    def __on_message(ws, message):
        tar = ws.__dict__["target"]
        tmp = {"stype": int(BR_TYPE.PT_MSG), "data": message}
        tar.__add_data(tmp)
    @staticmethod
    def __on_error(ws, error):
        tar = ws.__dict__["target"]
        tar.__stage = 3
        # tmp = {"stype": int(BR_TYPE.PT_ERR), "data":0}
        # tar.__add_data(tmp)
    @staticmethod
    def __on_close(ws):
        tar = ws.__dict__["target"]
        # tmp = {"stype":int( BR_TYPE.PT_CLOSE), "data": 0}
        # tar.__add_data(tmp)
        tar.__stage=2
    @staticmethod
    def __thread_client_start(tar):
        tar.__ws.run_forever()
        tmp = {"stype": int(BR_TYPE.PT_ERR), "data":0}
        tar.__add_data(tmp)
        pass
    def __init__(self):
        websocket.enableTrace(False)
        self.__data_pool=[]
        self.__ws=None
        self.__open_rc=None
        self.__recv_rc = None
        self.__err_rc = None
        self.__close_rc = None
        self.__ip=""
        self.__rc_target=None
        self.__queueLock = threading.Lock()
        self.__stage=0
        pass

    def __add_data(self,br):
        self.__queueLock.acquire()
        self.__data_pool.append(br)
        self.__queueLock.release()
        pass
    def BackToThread(self):
        data_pool_tmp=[]
        self.__queueLock.acquire()
        data_pool_tmp= self.__data_pool.copy()
        self.__data_pool.clear()
        self.__queueLock.release()
        for node in data_pool_tmp:
            stype = node["stype"]
            if stype == int(BR_TYPE.PT_OPEN):
                if self.__open_rc != None:
                    self.__open_rc(self.__rc_target)
            elif stype == int(BR_TYPE.PT_MSG):
                if self.__recv_rc != None:
                    self.__recv_rc(self.__rc_target, node["data"])
            elif stype == int(BR_TYPE.PT_ERR):
                if self.__err_rc != None:
                    self.__err_rc(self.__rc_target, node["data"])
                    break
            else:
                if self.__close_rc != None:
                    self.__close_rc(self.__rc_target)
                    break
        data_pool_tmp.clear()

        # self.__queueLock.acquire()
        # for node in self.__data_pool:
        #     stype = node["stype"]
        #     if stype == int(BR_TYPE.PT_OPEN):
        #         if self.__open_rc != None:
        #             self.__open_rc( self.__rc_target)
        #     elif stype == int(BR_TYPE.PT_MSG):
        #         if self.__recv_rc != None:
        #             self.__recv_rc( self.__rc_target,node["data"])
        #     elif stype == int(BR_TYPE.PT_ERR):
        #         if self.__err_rc != None:
        #             self.__err_rc( self.__rc_target,node["data"])
        #             break
        #     else:
        #         if self.__close_rc != None:
        #             self.__close_rc( self.__rc_target)
        #             break
        # self.__data_pool.clear()
        # self.__queueLock.release()

        pass

    def SetRcTarget(self,ypr):
        self.__rc_target = ypr
        pass
    def Close(self):
        if self.__stage==1:
           self.__ws.close()
    def Start(self,ip):
        self.__ip=ip
        # return self.TryToConnect()
        pass
    def TryToConnect(self):
        self.__data_pool.clear()
        self.__ws = websocket.WebSocketApp(
                                    self.__ip,
                                    on_open=Kubi_Websocket.__on_open,
                                    on_message=Kubi_Websocket.__on_message,
                                    on_error=Kubi_Websocket.__on_error,
                                    on_close=Kubi_Websocket.__on_close)
        self.__ws.__dict__["target"]=self
        self.__stage = 0
        # self.__ws.run_forever()
        _thread.start_new_thread(Kubi_Websocket.__thread_client_start, (self,))
        pass
    def Send(self,arr):
        # OPCODE_TEXT = 0x1
        OPCODE_BINARY = 0x2
        if self.__stage == 1:
           self.__ws.send(arr,OPCODE_BINARY)
    def set_open_rc(self,rc):
        self.__open_rc=rc
    def set_recv_rc(self,rc):
        self.__recv_rc=rc
    def set_err_rc(self, rc):
        self.__err_rc = rc
    def set_close_rc(self, rc):
        self.__close_rc = rc
    pass
class Kubi_Client:
    @staticmethod
    def __on_time_heart_start(tar):
        while True:
            if tar.__heart_has_start==False:
                break
            if tar.__heart_check_stage()==True:
                tar.Close()
                break
            time.sleep(1)
        tar.__heart_has_end=True
        pass
    @staticmethod
    def __imp_on_open(tar):
        tar.__send_enter()
        pass
    @staticmethod
    def __imp_no_err(tar,err_type):
        if tar.__on_end!=None:
           if tar.__if_end == False:
               tar.__heart_reg_stop()
               tar.__if_end=True
               tar.__on_end()
        pass
    @staticmethod
    def __imp_on_close(tar):
        if tar.__on_end!=None:
           if tar.__if_end == False:
               tar.__heart_reg_stop()
               tar.__if_end=True
               tar.__on_end()
        pass
    @staticmethod
    def __imp_on_msg(tar, data):
        tar.__service(data)
        pass
    def __init__(self):
        self.__uid=""
        self.__ip=""
        self.__pack_id=0
        self.__req_pools = {}
        self.__not_pools = {}
        self.__heart_caculate_ind = 0
        self.__heart_time_max = 4
        self.__heart_has_start=False
        self.__heart_has_end=False
        self.__pack_id_lock = threading.Lock()
        self.__req_lock = threading.Lock()
        self.__not_lock = threading.Lock()
        self.__heart_req_lock = threading.Lock()
        self.__ws = Kubi_Websocket()
        self.__ws.SetRcTarget(self)
        self.__ws.set_open_rc(Kubi_Client.__imp_on_open)
        self.__ws.set_recv_rc(Kubi_Client.__imp_on_msg)
        self.__ws.set_err_rc(Kubi_Client.__imp_no_err)
        self.__ws.set_close_rc(Kubi_Client.__imp_on_close)
        self.__on_end=None
        self.__on_enter = None
        self.__if_end=False
        pass
    def Start(self,uid,ip):
        self.__uid =uid
        self.__ip = ip
        self.__ws.Start(self.__ip)
        return self.TryToConnect()
        pass
    def TryToConnect(self):
         #因爲on 在enter之後
        self.__clear_not_pools()
        self.__clear_req_pools()
        self.__if_end = False
        self.__heart_caculate_ind = 0
        self.__heart_has_start = False
        self.__heart_has_end = True
        self.__pack_id=0
        return self.__ws.TryToConnect()
        pass
    def Request(self,route,data_str,data_bytes,rc):
        pid=self.__get_pkg_id_add()
        kn = KNode()
        kn.uid = self.__uid
        kn.pkg_type = int(PACK_TYPE.PT_REQUEST)
        kn.pkg_id = pid
        kn.route = route
        if data_str!=None:
            kn.data_str = data_str
        else:
            kn.data_str=""
        if data_bytes != None:
            kn.data_bytes = data_bytes
        else:
            kn.data_bytes = b''
        self.__add_req_pools(pid,rc)
        self.__send(kn)
        pass
    def Notice(self, route, data_str, data_bytes):
        kn = KNode()
        kn.uid = self.__uid
        kn.pkg_type = int(PACK_TYPE.PT_NOTICE)
        kn.pkg_id = 0
        kn.route = route
        if data_str != None:
            kn.data_str = data_str
        else:
            kn.data_str = ""
        if data_bytes != None:
            kn.data_bytes = data_bytes
        else:
            kn.data_bytes = b''
        self.__send(kn)
        pass
    def OnNotice(self,route,rc):
        self.__add_not_pools(route,rc)
        pass
    def Loop(self):
        self.__ws.BackToThread()
        pass
    def Close(self):
        self.__ws.Close()
        pass
    def OnEnd(self,rc):
        self.__on_end=rc
        pass
    def OnEnter(self, rc):
        self.__on_enter = rc
        pass
    def __service(self,msg):
        self.__heart_reset_stage()
        kn2 = KNode()
        if kn2.ParseFromString(msg)>0:
            key=kn2.pkg_type
            if key==int(PACK_TYPE.PT_HEART):
                self.__heart_send()
            elif key==int(PACK_TYPE.PT_ENTER):
                heart_max=int(kn2.data_str)
                self.__heart_time_max=heart_max
                if self.__heart_time_max<0:
                    self.__heart_time_max=0
                if self.__heart_time_max >0:
                   self.__heart_init_timer()
                if self.__on_enter!=None:
                    self.__on_enter()
            elif key ==int( PACK_TYPE.PT_REQUEST):
                rc=self.__get_req_rc(kn2.pkg_id)
                if rc!=None:
                    rc(kn2)
            elif key == int(PACK_TYPE.PT_NOTICE):
                rc = self.__get_not_rc(kn2.route)
                if rc != None:
                    rc(kn2)
            else:
                self.Close()
        else:
            self.Close()
        pass
    def __send(self,node):
        arr = node.SerializeToString()
        self.__ws.Send(arr)
        pass
    def __send_enter(self):
        kn = KNode()
        kn.uid = self.__uid
        kn.pkg_type = int(PACK_TYPE.PT_ENTER)
        kn.pkg_id = 0
        kn.route = ""
        kn.data_str = ""
        kn.data_bytes = b''
        self.__send(kn)
        pass
    def __heart_send(self):
        if self.__heart_time_max > 0:
            kn = KNode()
            kn.uid = self.__uid
            kn.pkg_type = int(PACK_TYPE.PT_HEART)
            kn.pkg_id = 0
            kn.route = ""
            kn.data_str = ""
            kn.data_bytes = b''
            self.__send(kn)
    def __add_req_pools(self, pid, rc):
        self.__req_lock.acquire()
        self.__req_pools[pid] = rc
        self.__req_lock.release()
        pass
    def __get_req_rc(self,pid):
        rs=None
        self.__req_lock.acquire()
        rs=self.__req_pools.pop(pid,None)
        self.__req_lock.release()
        return rs
    def __clear_req_pools(self):
        self.__req_lock.acquire()
        self.__req_pools.clear()
        self.__req_lock.release()
        pass

    def __add_not_pools(self, route, rc):
        self.__not_lock.acquire()
        self.__not_pools[route] = rc
        self.__not_lock.release()
        pass
    def __get_not_rc(self,route):
        rs=None
        self.__not_lock.acquire()
        rs=self.__not_pools.get(route,None)
        self.__not_lock.release()
        return rs
    def __clear_not_pools(self):
        self.__not_lock.acquire()
        self.__not_pools.clear()
        self.__not_lock.release()
        pass

    def __get_pkg_id_add(self):
        self.__pack_id_lock.acquire()
        self.__pack_id= self.__pack_id+1
        if self.__pack_id > 65535:
            self.__pack_id  = 0
        self.__pack_id_lock.release()
        return self.__pack_id
        pass
    def __heart_init_timer(self):
        if self.__heart_time_max > 0:
            self.__heart_caculate_ind = 0
            self.__heart_has_start = True
            self.__heart_has_end = False
            _thread.start_new_thread(Kubi_Client.__on_time_heart_start, (self,))

        pass
    def __heart_check_stage(self):
        if self.__heart_time_max > 0:
            re=False
            self.__heart_req_lock.acquire()
            if self.__heart_caculate_ind==self.__heart_time_max:
                self.__heart_caculate_ind = 0
                re=True
            else:
                self.__heart_caculate_ind= self.__heart_caculate_ind+1
            self.__heart_req_lock.release()
        return re
        pass
    def __heart_reset_stage(self):
        if self.__heart_time_max > 0:
            self.__heart_req_lock.acquire()
            self.__heart_caculate_ind = 0
            self.__heart_req_lock.release()
        pass
    def __heart_reg_stop(self):
        if self.__heart_time_max > 0:
            self.__heart_has_start = False
            while True :
                time.sleep(0.01)
                if self.__heart_has_end==True:
                   break
    pass



