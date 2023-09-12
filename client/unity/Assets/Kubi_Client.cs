using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Text;
using System.IO;
using System;
using pb = global::Google.Protobuf;
using BestHTTP;
using BestHTTP.WebSocket;

public class Kubi_Client : MonoBehaviour
{
    void Start()
    {

    }
    void Update()
    {

    }
    void OnDestroy()
    {
        if (client != null)
        {
            client.Close();
        }
        if (heart_time_max > 0)
        {
            heart_reg_stop();
        }
    }
    public enum PACK_TYPE
    {
        PT_HEART = 0,//心跳
        PT_ENTER,//登陆
        PT_REQUEST,//请求
        PT_NOTICE,//正常通知
        PT_EXIT,//退出
    };
    public void ClientStart(string _uid,string _address)
    {
        uid = _uid;
        address = _address;
        TryToConnect();
    }
    public void TryToConnect() {
        _OnEndStage = 0;
        heart_caculate_ind = 0;
        pack_id = 0;
        //因爲on 在enter之後
        _clear_not_pools();
        _clear_req_pools();
        client = new WebSocket(new Uri(address));
       
        client.OnOpen = _OnOpen;
        client.OnMessage = _OnMessageRecv;
        client.OnBinary = _OnBinaryRecv;
        client.OnClosed = _OnClosed;
        client.OnError = _OnError;
        client.Open();
      
    }
    public void Loop() {
        
        //u3d失去焦点fix:
        if (_OnEndStage == 1) {
            if (client == null)
            {
                _OnEndStage = 4;
            }
        }
        if (_onend != null)
        {
      
            if (_OnEndStage >= 2)
            {
                 _OnEndStage = 0;
                heart_reg_stop();
                client = null;
                _onend();
            }
        }
    }
    public void Request(string route,string data_str,byte[] data_bytes, Action<Kubi.KNode> rc) {
        Kubi.KNode node = new Kubi.KNode();
        node.PkgType = (int)PACK_TYPE.PT_REQUEST;
        node.PkgId = _get_pkg_id_add();
        node.Uid = uid;
        node.Route = route;
        node.DataStr = data_str;
        if (data_bytes != null)
        {
            node.DataBytes = pb.ByteString.CopyFrom(data_bytes, 0, data_bytes.Length);
        }
        else
        {
            byte[] arr = Encoding.Default.GetBytes("");
            node.DataBytes = pb.ByteString.CopyFrom(arr, 0, arr.Length);
        }
        _add_req_pools(node.PkgId, rc);
        _send(node);
    }
    public void Notice(string route, string data_str, byte[] data_bytes)
    {
        Kubi.KNode node = new Kubi.KNode();
        node.PkgType = (int)PACK_TYPE.PT_NOTICE;
        node.PkgId = 0;
        node.Uid = uid;
        node.Route = route;
        node.DataStr = data_str;
        if (data_bytes != null)
        {
            node.DataBytes = pb.ByteString.CopyFrom(data_bytes, 0, data_bytes.Length);
        }
        else {
            byte[] arr = Encoding.Default.GetBytes("");
            node.DataBytes = pb.ByteString.CopyFrom(arr, 0, arr.Length);
        }
        
        _send(node);
    }
    public void OnEnter(Action rc){
        _onenter = rc;
    }
    public void OnNotice(string route, Action<Kubi.KNode> rc)
    {
        _add_not_pools(route, rc);
    }
    //public void OnClose(Action rc)
    //{
    //    _onclose = rc;
    //}
    //public void OnErr(Action<int> rc)
    //{
    //    _onerr = rc;
    //}
    public void OnEnd(Action rc)
    {
        _onend = rc;
    }
    //-----------------------------------------
    Action _onenter = null;
    Action _onclose = null;
    Action<int> _onerr = null;
    Action _onend = null;
    int _OnEndStage = 0;
    WebSocket client;
    //UnityWebSocket.Uniform.WebSocket client;
    string uid = "";
    string address = "";
    int pack_id = 0;
    object _pack_id_lock=new object();
    int _get_pkg_id_add() {
        lock(_pack_id_lock) {
            pack_id++;
            if (pack_id > 65535)
            {
                pack_id = 0;
            }
        }
        return pack_id;
    }
    Dictionary<string, Action<Kubi.KNode>> _notice_pools = new Dictionary<string, Action<Kubi.KNode>>();
    Dictionary<int, Action<Kubi.KNode>> _req_pools = new Dictionary<int, Action<Kubi.KNode>>();
    void _add_not_pools(string route, Action<Kubi.KNode> rc) {
        lock (_notice_pools) {
            _notice_pools.Add(route, rc);
        }
    }
    Action<Kubi.KNode> _get_not_rc(string route) {
        Action<Kubi.KNode> re = null;
        lock (_notice_pools)
        {
            if (_notice_pools.ContainsKey(route))
            {
                re = _notice_pools[route];
            }
        }
        return re;
    }
    void _clear_not_pools() {
        lock (_notice_pools)
        {
            _notice_pools.Clear();
        }
    }
    void _add_req_pools(int pkg_id, Action<Kubi.KNode> rc)
    {
        lock (_req_pools)
        {
            _req_pools.Add(pkg_id, rc);
        }
    }
    Action<Kubi.KNode> _get_req_rc(int pkg_id)
    {
        Action<Kubi.KNode> re = null;
        lock (_req_pools)
        {
            if (_req_pools.ContainsKey(pkg_id))
            {
                re = _req_pools[pkg_id];
                _req_pools.Remove(pkg_id);
            }
        }
        return re;
    }
    void _clear_req_pools()
    {
        lock (_req_pools)
        {
            _req_pools.Clear();
        }
    }
    void _service(byte[] data){
        heart_reset_stage();
        if (data.Length > 2){
            Kubi.KNode np = Kubi.KNode.Parser.ParseFrom(data);
            if (np != null)
            {
                PACK_TYPE key = (PACK_TYPE)np.PkgType;
                if (key == PACK_TYPE.PT_HEART)
                {
                    heart_send();
                }
                else if (key == PACK_TYPE.PT_ENTER)
                {
                    int heart_max = int.Parse(np.DataStr);
                  
                    this.heart_time_max = heart_max;
                    if (this.heart_time_max < 0)
                    {
                        this.heart_time_max = 0;
                    }
                    if (_onenter != null)
                    {
                        _onenter();
                    }
                    if (this.heart_time_max > 0)
                    {
                        heart_init_timer();
                    }
                }
                else if (key == PACK_TYPE.PT_NOTICE)
                {
                    Action<Kubi.KNode> res = _get_not_rc(np.Route);
                    if (res != null)
                    {
                        res(np);
                    }
                }
                else if (key == PACK_TYPE.PT_REQUEST)
                { 
                    Action<Kubi.KNode> res = _get_req_rc(np.PkgId);
                    if (res != null)
                    {

                        res(np);
                    }
                }
                else if (key == PACK_TYPE.PT_EXIT)
                {}
            }
            else {
                Debug.Log(data.Length);
                if (client != null) {
                    client.Close();
                }
            }
        }
    }
    void _send(Kubi.KNode node)
    {
        if (client != null) {
            MemoryStream byte_out = new MemoryStream();
            pb::CodedOutputStream byte_out_br = new pb.CodedOutputStream(byte_out);
            node.WriteTo(byte_out_br);
            byte_out_br.Flush();
            client.Send(byte_out.ToArray());
        }
       
    }
    void _send_enter()
    {
        Kubi.KNode node = new Kubi.KNode();
        node.PkgType=(int)PACK_TYPE.PT_ENTER;
        node.PkgId = 0;
        node.Uid = uid;
        node.Route = "";
        node.DataStr = "";
        byte[] arr = Encoding.Default.GetBytes("");
        node.DataBytes = pb.ByteString.CopyFrom(arr, 0, arr.Length);
        _send(node);
    }
    //---------------------
    int heart_caculate_ind = 0;
    int heart_time_max = 4;
    void heart_init_timer() {
        if (this.heart_time_max > 0)
        {
            heart_caculate_ind = 0;
            InvokeRepeating("on_time_heart_start", 1, 1);
        }
    }
    void heart_reset_stage(){
        if (this.heart_time_max > 0)
        {
            this.heart_caculate_ind = 0;
        }
    }
    void on_time_heart_start(){
        if (this.heart_time_max > 0)
        {
            if (heart_check_stage())
            {
                if (client != null)
                {
                    client.Close();
                }
                this.heart_caculate_ind = 0;
                heart_reg_stop();
            }
        }
    }
    bool heart_check_stage(){
        if (this.heart_time_max > 0)
        {
            if (heart_caculate_ind == heart_time_max)
            {
                heart_caculate_ind = 0;
                return true;
            }
            else
            {
                heart_caculate_ind++;
            }
        }
        return false;
    }
    void heart_reg_stop(){
        if (this.heart_time_max > 0)
        {
            CancelInvoke("on_time_heart_start");
        }
    }
    void heart_send(){
        if (this.heart_time_max > 0)
        {
            Kubi.KNode node = new Kubi.KNode();
            node.PkgType = (int)PACK_TYPE.PT_HEART;
            node.PkgId = 0;
            node.Uid = uid;
            node.Route = "";
            node.DataStr = "";
            byte[] arr = Encoding.Default.GetBytes("");
            node.DataBytes = pb.ByteString.CopyFrom(arr, 0, arr.Length);
            _send(node);
        }
    }

    void _OnOpen(WebSocket ws)
    {
        _OnEndStage = 1;
        _send_enter();
    }
    void _OnMessageRecv(WebSocket ws, string message)
    {
       
    }
    void _OnBinaryRecv(WebSocket ws, byte[] data)
    {
        _service(data);
    }
    void _OnClosed(WebSocket ws, UInt16 code, string message)
    {
       // Debug.Log("_OnClosed");
        _OnEndStage = 3;
        if (_onclose != null)
        {
            _onclose();
        }
    }
    void _OnError(WebSocket ws, Exception ex)
    {
        //Debug.Log("_OnError");
        _OnEndStage = 2;
        if (_onerr != null)
        {
            _onerr(0);
        }
      
    }
}
