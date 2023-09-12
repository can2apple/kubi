class Kubi_Ser_Py_Imp:
    def __init__(self):
        pass
    def SetHeartTime(self, heart_time):
        pass
    def SetNotEnterStageTime(self,heart_time):
        pass
    def Start(self,port,if_inl):#return int
        pass
    def Ping(self,port,if_inl):#(uid,str_data)
        pass
    def Notify(self,uid,route,data_str,data_byte):
        pass
    def Response(self,route,uid,pkg_id,data_str,data_byte):
        pass
    def RegClose(self):
        pass
    def SendCtrlAsync(self,str_signal,str_msg):
        pass
    def RegCloseClient(self,uid):
        pass
    def RunInYourThread(self):  # list
        pass
    def OnServerClose(self,_rc):#def rc():->void
        pass
    def OnClientEnter(self,_rc):#def rc(str_uid, str_ip,int_port):->void
        pass
    def OnClientLeave(self,_rc):  # def rc(str_uid):->void
        pass
    def RegistNotifyRoute(self,route,_rc):  # def rc(uid,pkg_id,data_str,data_bytes)
        pass
    def RegistRequestRoute(self, route, _rc):  #rc (uid,pkg_id,data_str,data_bytes)
        pass
    def RegistInlineCtrlRoute(self,str_signal,str_msg):
        pass