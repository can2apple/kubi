from kubi_client import Kubi_Client,KNodeImp
import numpy as np
import json
import time

uid="py_0001"
client=Kubi_Client()

def OnEnd():
    print("OnEnd,Try To connect...")
    client.TryToConnect()
    pass
#-----
def OnEnter():
    print("OnEnter")
    client.Request("on_login","can",b'wen',Login_rc)
    pass
def Login_rc(knode):
    kni = KNodeImp()
    kni.FromkNode(knode)
    print(kni.data_str)
    pass

if __name__ == '__main__':
    client.OnEnd(OnEnd)
    client.OnEnter(OnEnter)
    client.Start(uid,"ws://127.0.0.1:8080")
    while (True):
        time.sleep(0.01)
        client.Loop()
        pass
    pass
