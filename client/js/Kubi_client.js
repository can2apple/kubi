function Help_ArrayBufferToBase64(buffer){
    var binary = '';
    var bytes = new Uint8Array(buffer);
    var len = bytes.byteLength;
    for (var i = 0; i < len; i++) {
       binary += String.fromCharCode(bytes[i]);
    }
    return window.btoa(binary);
}

function uint8arrayToBase64(u8Arr) {
    var CHUNK_SIZE = 0x8000; //arbitrary number
    var index = 0;
    var length = u8Arr.length;
    var result = '';
    var slice;
    while (index < length) {
        slice = u8Arr.subarray(index, Math.min(index + CHUNK_SIZE, length));
        result += String.fromCharCode.apply(null, slice);
        index += CHUNK_SIZE;
    }
    return btoa(result);
}

function Help_Base64ToUint8Array(base64String) {
    var padding = '='.repeat((4 - base64String.length % 4) % 4);
    var base64 = (base64String + padding)
        .replace(/\-/g, '+')
        .replace(/_/g, '/');

        var rawData = window.atob(base64);
        var outputArray = new Uint8Array(rawData.length);

    for (var i = 0; i < rawData.length; ++i) {
        outputArray[i] = rawData.charCodeAt(i);
    }
    return outputArray;
}

function Help_Uint8ArrayToBase64(u8arr){
    var binary = '';
    var bytes = u8arr;
    var len = bytes.byteLength;
    for (var i = 0; i < len; i++) {
       binary += String.fromCharCode(bytes[i]);
    }
    return window.btoa(binary);
}

function Help_U8ToStr(data){
   return String.fromCharCode(new Uint8Array(data));
}
function Help_StrToU8(data){
   return String.fromCharCode.apply(null, new Uint8Array(data));
}
/**
 * byte[] 格式转字符串
 * @param {byte[]} arr
 */
 function Help_ByteToString(arr) {
    if(typeof arr === 'string') {
        return arr;
    }
    var str = '',
        _arr = arr;
    for(var i = 0; i < _arr.length; i++) {
        var one = _arr[i].toString(2),
            v = one.match(/^1+?(?=0)/);
        if(v && one.length == 8) {
            var bytesLength = v[0].length;
            var store = _arr[i].toString(2).slice(7 - bytesLength);
            for(var st = 1; st < bytesLength; st++) {
                store += _arr[st + i].toString(2).slice(2);
            }
            str += String.fromCharCode(parseInt(store, 2));
            i += bytesLength - 1;
        } else {
            str += String.fromCharCode(_arr[i]);
        }
    }
    return str;
}
   /**
 * stringToByte  字符串格式转byte[] 
 * @param {String} str
 */
function Help_StringToByte(str) {
    var bytes = new Array();
    var len, c;
    len = str.length;
    for (var i = 0; i < len; i++) {
        c = str.charCodeAt(i);
        if (c >= 0x010000 && c <= 0x10FFFF) {
            bytes.push(((c >> 18) & 0x07) | 0xF0);
            bytes.push(((c >> 12) & 0x3F) | 0x80);
            bytes.push(((c >> 6) & 0x3F) | 0x80);
            bytes.push((c & 0x3F) | 0x80);
        } else if (c >= 0x000800 && c <= 0x00FFFF) {
            bytes.push(((c >> 12) & 0x0F) | 0xE0);
            bytes.push(((c >> 6) & 0x3F) | 0x80);
            bytes.push((c & 0x3F) | 0x80);
        } else if (c >= 0x000080 && c <= 0x0007FF) {
            bytes.push(((c >> 6) & 0x1F) | 0xC0);
            bytes.push((c & 0x3F) | 0x80);
        } else {
            bytes.push(c & 0xFF);
        }
    }
    return bytes;
}
//-----------------------------------------------------------------------------------

    var PACK_TYPE = {
        PT_HEART : 0,//心跳
        PT_ENTER :1,//登陆
        PT_REQUEST :2,//请求
        PT_NOTICE :3,//正常通知
        PT_EXIT:4,//退出
    };

    function Kubi_Client(){
       
        var uid="";
        var address="";
        var pack_id=0;
        var heart_caculate_ind = 0;
        var heart_time_max = 4;
        var client=null;
        var notice_pools=new Array()//<string,call_back(KNode node)>
        var req_pools=new Array()//<string,call_back(KNode node)>
        var heart_inter=null;
        var on_enter=null;
        var on_close=null;
        var on_end=null;
        var on_err=null;//_onerr(type:int)
        var on_on_end_been_call=false;

        this.OnEnter=function(rc/*void rc(void)*/){
            on_enter=rc
        }
        this.OnEnd=function(rc/*void rc(void)*/){
            on_end=rc;
        }
        this.ClientStart=function(_uid,_address){
            uid=_uid;
            address=_address;
            this.TryToConnect();
        }
        this.Notice=function(route,data_str,data_arr_buf/*Uint8Array or string*/){
            var dpro = new proto.kubi.KNode()
            dpro.setUid(uid);
            dpro.setPkgType(PACK_TYPE.PT_NOTICE);
            dpro.setPkgId(0);
            dpro.setDataStr(data_str);
            dpro.setRoute(route);
            dpro.setDataBytes(data_arr_buf);
            _send(dpro)
        }
        this.Request=function(route,data_str,data_arr_buf/*Uint8Array or string*/,rc/*void rc(proto.kubi.KNode)*/){
            var dpro = new proto.kubi.KNode()
            dpro.setUid(uid);
            dpro.setPkgType(PACK_TYPE.PT_REQUEST);
            dpro.setPkgId(_get_pkg_id_add());
            dpro.setDataStr(data_str);
            dpro.setRoute(route);
            dpro.setDataBytes(data_arr_buf);
            req_pools[dpro.getPkgId()]= rc;
            _send(dpro)

        }
        this.OnNotice=function(route,rc/*void rc(proto.kubi.KNode)*/){
            notice_pools[route]=rc
        }
        this.TryToConnect=function(){
            heart_caculate_ind = 0;
            pack_id = 0;
            on_on_end_been_call=false;
            // //因爲on 在enter之後
            //try
            {
                console.log(address)
                if (notice_pools.length>0){
                    notice_pools=new Array();
                }
                if (req_pools.length>0){
                    req_pools=new Array();
                }
    
                client=new WebSocket(address);
                client.binaryType = "arraybuffer";
            }
            //catch(err)
            {
                //  console.log(" WebSocket connection err")
                //  if(on_end!=null){
                //      on_end();
                     
                //  }
            }
            //if(client!=null)
            {
                client.onopen=function(){
                    _send_enter();
                };
                client.onerror=function(event){
                   
                    client=null;
                    if(on_end!=null){
                        if(on_on_end_been_call==false){
                            on_on_end_been_call=true;
                            console.log("  onerror")
                            _heart_reg_stop();
                            on_end();
                            
                        }
                      
                       
                    }
                };
                client.onclose = function()
                {
                  client=null;
                  if(on_end!=null){
                    if(on_on_end_been_call==false){
                        console.log("  onclose")  
                        _heart_reg_stop();
                        on_on_end_been_call=true;
                        on_end();
                       
                    }
                  }
                };
                client.onmessage = function (evt){
                    _service(evt);
                };
            }
          
        }
        var _get_pkg_id_add=function() {
            pack_id++;
            if (pack_id > 65535)
            {
                pack_id = 0;
            }
            return pack_id;
        }
        var _send_enter=function()
        {
            var node = new proto.kubi.KNode()
            node.setUid(uid)
            node.setPkgType(PACK_TYPE.PT_ENTER);
            node.setPkgId(0);
            node.setRoute("");
            node.setDataStr("");
            node.setDataBytes("")
            _send(node);
        }

        var _send=function(node){
           if(client!=null){
            client.send(node.serializeBinary())
           }
        }
        var _service=function(evt){
         
            var buffer = evt.data;
            if(buffer.byteLength>2){
                _heart_reset_stage();
                var node= proto.kubi.KNode.deserializeBinary(buffer);
                if(node.getPkgType()==PACK_TYPE.PT_HEART){
                    _send_heart()
                }
                if(node.getPkgType()==PACK_TYPE.PT_ENTER){
                    heart_time_max= parseInt(node.getDataStr());
                    if (heart_time_max < 0){
                        heart_time_max= 0;
                    }
                    if (on_enter != null){
                        on_enter();
                    }
                    if (heart_time_max > 0){
                        _heart_init_timer();
                    }
                }
                if(node.getPkgType()==PACK_TYPE.PT_NOTICE){
                    var rc=notice_pools[node.getRoute()];
                    if(rc!=undefined){
                        rc(node)
                    }   
                }
                if(node.getPkgType()==PACK_TYPE.PT_REQUEST){
                    var rc=req_pools[node.getPkgId()];
                    delete req_pools[node.getPkgId()];
                    if(rc!=undefined){
                        rc(node)
                    }   
                }
            }else{
                if (client != null){
                    client.close();
                }
            }
        }
     
        var _heart_init_timer=function(){
            heart_inter= setInterval(_on_time_heart_start,1000)
        }
        var _on_time_heart_start=function(){
            if (heart_time_max > 0){
                if (_heart_check_stage()){
                    if (client != null){
                        client.close();
                    }
                    heart_caculate_ind = 0;
                    _heart_reg_stop();
                }
            }
        }
         var _heart_reset_stage=function(){
            if (heart_time_max > 0){
              heart_caculate_ind = 0;
            }
        }
        var _heart_check_stage=function(){
            if (heart_time_max > 0){
                if (heart_caculate_ind == heart_time_max) {
                    heart_caculate_ind = 0;
                    return true;
                }
                else{
                    heart_caculate_ind++;
                }
            }
            return false;
        }

        var _heart_reg_stop=function(){
            if (heart_time_max > 0){
                if(heart_inter!=null){
                    clearInterval(heart_inter);
                    heart_inter=null;
                }
            }
       }

        var _send_heart=function(){
            if(heart_time_max>0){
                var node = new proto.kubi.KNode()
                node.setUid(uid)
                node.setPkgType(PACK_TYPE.PACK_TYPE_ENTER);
                node.setPkgId(0);
                node.setRoute("");
                node.setDataStr("");
                node.setDataBytes("")
                _send(node);
            }
        }
    }
