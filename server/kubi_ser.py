from kubi_ser_py import Kubi_Ser_Py_Imp
class PACK_TYPE:
    PT_HEART = 0
    PT_ENTER=1
    PT_REQUEST=2
    PT_NOTICE=3
    PT_EXIT=4

class Kubi_ser:
    @staticmethod
    def __notify_rc(client_uid, node_in):
        pass

    @staticmethod
    def __req_rc(client_uid,node_in,node_out):
        pass
    def __init__(self):
        self.ser=Kubi_Ser_Py_Imp()
        self.data={}#your data
        pass
    def SetHeartTime(self,ht):
        self.ser.SetHeartTime(ht)
        pass

    def SetNotEnterStageTime(self,ht):
        self.ser.SetNotEnterStageTime(ht)
        pass
    def Start(self,port,if_inl):
        self.ser.Start(port,if_inl)
        pass

    def Ping(self,str_uid):
        self.ser.Ping(str_uid)
        pass    

    def Notify(self, uid, route, data_str, data_byte):
        self.ser.Notify(uid, route, data_str, data_byte)
        pass

    def Response(self,route,uid,pkg_id,data_str,data_byte):
        self.ser.Response(route,uid,pkg_id,data_str,data_byte)
        pass

    def SendCtrlAsync(self,signal,msg):
        self.ser.SendCtrlAsync(signal,msg)
        pass

    def RegClose(self):
        self.ser.RegClose()
        pass

    def RegCloseClient(self,uid):
        self.ser.RegCloseClient(uid)
        pass

    def OnClientEnter(self,rc_client_enter):# def rc_client_enter(str_uid,str_ip,int_port)
        self.ser.OnClientEnter(rc_client_enter)
        pass

    def OnClientLeave(self,rc_client_left):# def rc_client_left(str_uid)
        self.ser.OnClientLeave(rc_client_left)
        pass
    def RegistRequestRoute(self,str_route,rc_req_route):# def rc_req_route(str_uid,int_pkg_id,str_data，bytes_data)
        self.ser.RegistRequestRoute(str_route,rc_req_route)
        pass
    
    def RegistNotifyRoute(self,str_route,rc_notify_route):# def rc_req_route(str_uid,str_data，bytes_data)
        self.ser.RegistNotifyRoute(str_route,rc_notify_route)
        pass

    def RegistInlineCtrlRoute(self,str_signal,rc_inlinectrl_route):# def rc_inlinectrl_route(str_msg)
        self.ser.RegistInlineCtrlRoute(str_signal,rc_inlinectrl_route)
        pass
    
    def OnServerClose(self,rc_ser_close):
        self.ser.OnServerClose(rc_ser_close)
        pass

    def RunInYourThread(self):
        self.ser.RunInYourThread()
        pass