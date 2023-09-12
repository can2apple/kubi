import json
from kubi_ser import Kubi_ser
import os
import multiprocessing


class FileHelp:
    def __init__(self) -> None:
        pass
    @staticmethod
    def help_windows_to_linux(windowsPath):
        windowsPath_h = windowsPath.split(':')[1] 
        windowsPath_w = windowsPath_h.split("\\")
        del windowsPath_w[0]
        linuxPath1 = "/".join(windowsPath_w)
        hh = windowsPath.split(':')[0]
        hh =  hh +":"+ "/"
        linuxPath = hh + linuxPath1 
        return linuxPath
    @staticmethod
    def help_get_curr_path():
        str=os.path.split(os.path.realpath(__file__))[0]
        re=FileHelp.help_windows_to_linux(str)
        return re
    
class User:
    def __init__(self):
        self.uid = ""
        self.name=""
        self.role=""
    def set_base_msg(self,uid,name,role):
        self.uid=uid
        self.name=name
        self.role=role
    def to_json_str(self):
        json_tuple = {}
        json_tuple["uid"]=self.uid
        json_tuple["name"] = self.name
        json_tuple["role"] = self.role
        return json.dumps(json_tuple)
    def parse_from_json_str(self,msg):
        json_tuple = json.loads(msg)
        self.uid = json_tuple["uid"]
        self.name = json_tuple["name"]
        self.role = json_tuple["role"]
    pass

class Task:
    def __init__(self):
        self.uid=""
        '''#{'task':"what", 'opt':"null",'stage':0}'''
        self.task={}
        self.img_datas=b''
        self.img_datas_des=[]
        self.img_datas_div = []
        pass
    def push_bytes(self,img_datas):
        self.img_datas_div.append(img_datas)
        self.img_datas_des.append(len(img_datas))
        if len(self.img_datas)==0:
            self.img_datas=img_datas
        else:
            self.img_datas= self.img_datas+img_datas
        pass

    def set_task_stage(self,stage):
        self.task["stage"]=stage
        pass
    def set_task_opt(self,opt):
        self.task["opt"]=opt
        pass
    def set_task_task(self,task):
        self.task["task"]=task
        pass
    def get_task_stage(self):
        return self.task["stage"]

    def get_task_opt(self):
        return self.task["opt"]

    def get_task_task(self):
        return self.task["task"]

    def empty_data(self):
        self.img_datas = b''
        self.img_datas_des = []
        self.img_datas_div = []
        pass
    def to_json_str(self):
        """
        use rule:
        :return:{"uid":"XXX",task:{'task':"what1",'stage':0}}
        """
        json_tuple={'uid':self.uid}
        json_tuple["task"]=self.task
        json_tuple["img_des"]=[]
        for i in range(len(self.img_datas_des)):
            json_tuple["img_des"].append(self.img_datas_des[i])
        return json.dumps(json_tuple)

    
    def parse_from_json_str(self,json_str,datas):
        """
            use rule:
            :param json_str:{"uid":"XXX",task:{'task':"what1",'stage':0}}
            :return:void
        """
        json_tuple=None
        self.img_datas_div.clear()
        try:
            json_tuple=json.loads(json_str)
        except EOFError:
            print('cannot parse json_str')
        else:
            self.uid=json_tuple["uid"]
            self.task = json_tuple["task"]
            self.img_datas_des=json_tuple["img_des"]
            self.img_datas=datas
            ind = 0
            for des in self.img_datas_des:
                self.img_datas_div.append(self.img_datas[ind:des])
                ind = ind + des
        pass
    pass

class Server:
    
    def __init__(self,port) -> None:
  
        self.gatherers=[]
        self.caculaters=[]
        self.showers=[]
        
        self.g_ser=Kubi_ser()
        self.g_ser.SetNotEnterStageTime(-1)
        self.g_ser.SetHeartTime(-1)
        self.g_ser.OnClientEnter(self._on_enter)
        self.g_ser.OnClientLeave(self._on_leave)
        self.g_ser.RegistRequestRoute("on_login",self._on_login)
        self.g_ser.RegistNotifyRoute("on_cap",self.on_cap)
        self.g_ser.RegistNotifyRoute("on_caculater_result",self.on_caculater_result)        
        self.g_ser.Start(port,True)
        pass
    
    
    def _help_add_user(self,pool,it):
        can_add=True
        for i in range(len(pool)):
            if pool[i].uid == it.uid:
                can_add=False
                break
        if can_add==True:
            pool.append(it)
        return can_add
 
    def _help_del_user(self,pool,uid):
        for i in range(len(pool)):
            if pool[i].uid==uid:
                dle= pool[i]
                pool.remove(dle)
                return  dle
        return None
    
    def _help_get_user(self,pool,uid):
        for i in range(len(pool)):
            if pool[i].uid==uid:
                dle= pool[i]
                return  dle
        return None

    def _on_enter(self,uid:str,ip:str,port:int):
        print(uid+" enter,ip:"+ip)
        pass
    def _on_leave(self,uid:str):
        print(uid+" leave")
        self._help_del_user(self.gatherers,uid)
        self._help_del_user(self.caculaters,uid)
        self._help_del_user(self.showers,uid)

        
    def _on_login(self,uid:str,pkg_id:int,data_str:str,data_bytes):
        user=User()
        user.parse_from_json_str(data_str)
        if user.role=="gatherer":
            self._help_add_user(self.gatherers,user)
        elif user.role=="caculater":
            self._help_add_user(self.caculaters,user)
        else:  
            self._help_add_user(self.showers,user)
       
        rsq={}
        rsq["code"] ="000"
        rsq["msg"]="welcome python bind server!"
        self.g_ser.Response("on_login",uid,pkg_id,json.dumps(rsq),b'')
        pass


    def on_cap(self,str_uid,str_data,bytes_data):
        if  len(self.caculaters)>0:
            for u in self.caculaters:
                self.g_ser.Notify(u.uid,"on_cap",str_data,bytes_data)
        pass
    
    def on_caculater_result(self,str_uid,str_data,bytes_data):
        if  len(self.showers)>0:
            for u in self.showers:
                self.g_ser.Notify(u.uid,"on_caculater_result",str_data,bytes_data)
        pass

    def Loop(self):
        self.g_ser.RunInYourThread()
        pass
    
    def ReClose(self):
        self.g_ser.RegClose()
        pass


def main():
    server= Server(8080)
    # while True:
    #     server.Loop()
    # server.ReClose()

if __name__ == '__main__':
    multiprocessing.freeze_support()
    print("server has open")
    main()
    pass

