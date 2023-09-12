using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SimpleJSON;
using System;
using System.IO;
using System.Text;
using pb = global::Google.Protobuf;


public class how_to_use : MonoBehaviour
{
    class TaskNode{
        public string opt = "null";
        public string task = "";
        public int stage = 0;//0 1 2 0:未开始 1成功 2失败
    }
    class Task{
        public string uid = "";
        public TaskNode task = null;
        public byte[] img_datas = null;
        public List<int> img_datas_des = null;
        public List<byte[]> img_datas_div = null;
        public Task(){
            img_datas_des = new List<int>();
            img_datas_div = new List<byte[]>();
        }
        public void push_bytes(byte[] data){
            img_datas_div.Add(data);
            img_datas_des.Add(data.Length);
            if (img_datas != null){
                byte[] tmp = new byte[img_datas.Length + data.Length];
                Array.Copy(img_datas, tmp, img_datas.Length);
                Array.Copy(data, 0, tmp, img_datas.Length, data.Length);
                img_datas = tmp;
            }
            else{
                img_datas = data;
            }
        }
        public void set_task_stage(int stage){
            if (task != null){
                task.stage = stage;
            }
        }
        public void empty_data(){
            img_datas_des = new List<int>();
            img_datas_div = new List<byte[]>();
            img_datas = null;
        }
        //{"uid":"XXX",'tasks':[{'task':"what1",'stage':0},...{'task':"whatn",'stage':0}],'img_des':[256*256,167]}
        public string to_json_str(){
            JSONObject msg = new JSONObject();
            msg["uid"] = uid;
            msg["task"] = new JSONObject();
            JSONObject tasknode = new JSONObject();
            tasknode["opt"] = task.opt;
            tasknode["task"] = task.task;
            tasknode["stage"] = task.stage;
            msg["task"] = tasknode;
            msg["img_des"] = new JSONArray();
            for (int i = 0; i < img_datas_des.Count; i++){
                msg["img_des"].Add(img_datas_des[i]);
            }
            return msg.ToString();
        }
        public void parse_from_json_str(string json_str, byte[] datas)
        {
            img_datas_div.Clear();
            img_datas_des.Clear();
            JSONNode msg = JSON.Parse(json_str);
            uid = msg["uid"];
            JSONObject jtask = msg["task"].AsObject;
            task = new TaskNode();
            task.stage = jtask["stage"].AsInt;
            task.task = jtask["task"];
            task.opt = jtask["opt"];
            JSONArray j_img_des = msg["img_des"].AsArray;
            for (int i = 0; i < j_img_des.Count; i++)
            {
                img_datas_des.Add(j_img_des[i].AsInt);
            }
            img_datas = datas;
            int ind = 0;
            for (int i = 0; i < img_datas_des.Count; i++)
            {
                int len = img_datas_des[i];
                byte[] dst = new byte[len];
                Array.Copy(img_datas, ind, dst, 0, len);
                img_datas_div.Add(dst);
                ind += len;
            }
        }
    }
    class User{
        public string uid = "";
        public string name = "";
        public string role = "";
        public string to_json_str(){
            JSONObject msg = new JSONObject();
            msg["uid"] = uid;
            msg["name"] = name;
            msg["role"] = role;
            return msg.ToString();
        }
        public void parse_from_json_str(string json_str){
            JSONNode msg = JSON.Parse(json_str);
            uid = msg["uid"];
            name = msg["name"];
            role = msg["role"];
        }

    }


    private bool if_has_connect = false;
    private  Kubi_Client client;


    //获得的数据将保留在这里
    private Texture2D recv_texture;
    public RawImage RawImage;
    //---------------------------------

    void Start()
    {
        client = gameObject.AddComponent<Kubi_Client>();
        client.ClientStart("id_from_unity", "ws://127.0.0.1:8080");
        client.OnEnter(() => {
            Debug.Log("OnEnter");
            User user = new User();
            user.uid = "id_from_unity";
            user.name = "from_unity";
            user.role = "shower";
            client.Request("on_login", user.to_json_str(), null, (rc) =>
            {
                //登录成功
                JSONNode msg = JSON.Parse(rc.DataStr);
                string code = msg["code"];
                string msgstr = msg["msg"];
                Debug.Log(code + ":" + msgstr);
                if_has_connect = true;
            

            });
            client.OnNotice("on_caculater_result", (rc) =>{ 
                Task task = new Task();
                task.parse_from_json_str(rc.DataStr, rc.DataBytes.ToByteArray());
                if (recv_texture == null){
                    recv_texture = new Texture2D(1, 1);
                    RawImage.texture = recv_texture;
                }
                recv_texture.LoadImage(task.img_datas_div[0]);
                recv_texture.Apply();
            });
        });
        client.OnEnd(() => {
            //端口了连接，尝试自动重连
            if_has_connect = false;
            Debug.Log("TryToConnect...");
            client.TryToConnect();
        });
    }
    void Update()
    {
        if (client != null)
        {
            client.Loop();
            if (if_has_connect)
            {
             
            }
        }
    }
}
