

## kubi_net 项目简介
**kubi_net 基于WebSocket协议的解决多语言多程序协调的微型网络库：**<br/>
### 服务器端：
    cmake构建 用c/c++编写 仅依赖libuv+protobuf库，并做了python绑定，可以直接基于cpp做业务编写，也可以使用python做业务编写
### 客户端：
    目前支持cpp 、Python 、js 、 c#(unity)
### 苦逼）kubi_net解决的问题：
    （苦逼）kubi_net 代码形式和Socket.IO类似，但有以下几个优点：
    1. 更轻量级，可以直接对代码修改用于自己的项目。
    2. 更彻底的断线重连机制，服务器断了，客户端重连请求依然存在，等服务器上线，客户端依然可以再次重连。
    3. 服务器使用了两种信息轮询方法。可以直接集成在程序里面使用。
    4. 直接支持 字符串、二进制数据传输，也支持字符串和二进制数据同时传输。
### 服务器代码：
cpp:<br/>
```
Kubi_Ser_Cpp11_Imp ser;
ser.OnServerClose([]()->void {
        LOG(WARNING) << "Server close!";
});
ser.OnClientEnter([](const std::string& uid, const std::string& ip, int port)->void {
    LOG(INFO) << "Kubi_Session has connect to server!";
    });
ser.OnClientLeave([](const std::string& uid)->void {
    LOG(WARNING) <<uid<< ",Kubi_Session has Leave ";
    });
ser.RegistRequestRoute("on_login", [](const std::string& uid, int pkg_id, const std::string& data_str, const std::string& data_byts)->void {
    //业务逻辑：
    Json::Value v_rsp;
    v_rsp["code"] = 123;
    v_rsp["msg"] = "welcome";
    ser.Response("on_login", uid, pkg_id, v_rsp.toStyledString().c_str(), NULL, 0);
    });
ser.RegistNotifyRoute("on_msg",  [](const std::string& uid, const std::string& data_str, const std::string& data_byts)->void {
    //业务逻辑：
    ser.Notify(uid,"on_msg", data_str.c_str(), (unsigned char*)data_byts.data(), data_byts.size());
    });
ser.SetNotEnterStageTime(-1);
ser.SetHeartTime(-1);
阻塞方式：
ser.Start(8080, true);
//回归到业务线程方式：
ser.Start(8080, false);
ser.RunInYourThread();
#在业务线程直接使用
ser.Notify(route, uid, str, bytes, len(bytes));
```
python:<br/>
```
g_ser=Kubi_ser()
def WhenServerClose():
    pass
def WhenClientEnter(uid:str,ip:str,port:int):
    print(uid+" enter,ip:"+ip+" ,"+str(port))
    pass
def WhenClientLeave(uid:str):
    print(uid + "leave")
    pass
def On_login(uid:str,pkg_id:int,data_str:str,data_bytes):
    g_ser.Response("login",uid,pkg_id,"welcome",b'')
    pass
if __name__ == '__main__':
    g_ser.SetNotEnterStageTime(6)
    g_ser.SetHeartTime(10)
    g_ser.OnServerClose(WhenServerClose)
    g_ser.OnClientEnter(WhenClientEnter)    
    g_ser.OnClientLeave(WhenClientLeave)
    g_ser.RegistRequestRoute("login",On_login)
    阻塞方式：
    g_ser.Start(8080,True)
    //回归到业务线程方式：
     g_ser.Start(8080,False)
     server.Loop()
```
---
### 客户端：
cpp(其他的 感兴趣的朋友可以自行查看源码demo)<br/>
```
Kubi_Client client;
client.Start(user.uid, "ws://127.0.0.1", 8080);
client.OnEnter([]()->void{
    client.Request("on_login", user.to_json_str().c_str(), NULL, 0, [](const kubi::KNode* in) ->void {
        #请求回应：
    });
    client.OnNotice("on_msg", [](const kubi::KNode* in) ->void {
        //do you want:
    });
	
});
client.OnEnd([]()->void {
    printf("OnEnd...\n");
    client.TryToConnect();
});

//业务线程：
client.Loop();
client.Notice(router, str, bytes, len(bytes));
```

### 关于demo:
demo 是视频Ascii艺术化处理的简单项目<br/>
有4个部分组成：<br/>
服务器端（cpp 、 python都实现了）<br/>
客户端：cpp 完成视频的解析和采集工作<br/>
客户端：python 完成Ascii艺术化的处理<br/>
客户端：unity js 做Ascii艺术化的展示工作<br/>

### 关于稳定性:
目前该代码在我的项目运行：<br/>
项目为<strong>展厅项目<br/> 
<strong>共31个<br/>
<strong>运行时3年
### 技术博客:
https://blog.csdn.net/u012357842