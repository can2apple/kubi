from kubi_client import Kubi_Client,KNodeImp
import numpy as np
import json
import time
import cv2
from PIL import Image, ImageFont, ImageDraw, ImageOps
#协助类：
class User:
    def __init__(self):
        self.uid =""
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
#协助类：
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


class AsciiArt:
    def __init__(self):
        self.char_list = "$@YYGY#*886886HELLO666?-_+~<>i!lI;:,\"^`'. "
        self.bg_code = 255
        self.font = ImageFont.truetype("demo/DejaVuSansMono-Bold.ttf", size=int(10 *2.0))
        self.num_chars = len(self.char_list)
        self.num_cols = 40
        self.overlay_ratio=0.2
        pass
    
    def transform(self,frame):

        image = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
   
        height, width = image.shape
        cell_width = width / self.num_cols
        cell_height = 2 * cell_width
        num_rows = int(height / cell_height)
        if  self.num_cols > width or num_rows > height:
            cell_width = 6
            cell_height = 12
            self.num_cols = int(width / cell_width)
            num_rows = int(height / cell_height)
        _,_,char_width, char_height = self.font.getbbox("A")
        out_width = char_width *  self.num_cols
        out_height = 2 * char_height * num_rows
        out_image = Image.new("L", (out_width, out_height), self.bg_code)
        draw = ImageDraw.Draw(out_image)
        for i in range(num_rows):
            line = "".join([ self.char_list[min(int(np.mean(
                image[int(i * cell_height):min(int((i + 1) * cell_height), height),
                int(j * cell_width):min(int((j + 1) * cell_width), width)]) * self.num_chars / 255), self.num_chars - 1)] for j in
                            range( self.num_cols)]) + "\n"
            draw.text((0, i * char_height), line, fill=255 - self.bg_code, font=self.font)

        cropped_image = ImageOps.invert(out_image).getbbox()
        out_image = out_image.crop(cropped_image)
        # out_image = cv2.cvtColor(np.array(out_image), cv2.COLOR_GRAY2BGR)
        out_image = np.array(out_image)
        out_image=cv2.resize(out_image,(width,height))
        return out_image
        pass

uid="from_py_0001"
client=Kubi_Client()
asci_art=AsciiArt() 


def on_cap(knode):
    kni = KNodeImp()
    kni.FromkNode(knode)
    tmp=Task()
    tmp.parse_from_json_str(kni.data_str,kni.data_bytes)
    data_c = np.frombuffer(tmp.img_datas_div[0], np.uint8)
    img=cv2.imdecode(data_c,cv2.IMREAD_COLOR)
    img= asci_art.transform(img)
    tsk_in=Task()
    tsk_in.push_bytes(cv2.imencode('.jpeg', img)[1].tobytes())
    client.Notice("on_caculater_result",tsk_in.to_json_str(),tsk_in.img_datas)
    # cv2.imshow("cap",img)
    # cv2.waitKey(1)
    
def on_login_rc(knode):
    kni = KNodeImp()
    kni.FromkNode(knode)
    print("on_login_rc")
 
    pass

def OnEnd():
    print("OnEnd,Try To connect...")
    client.TryToConnect()
    pass
#-----
def OnEnter():
    print("OnEnter")
    user=User()
    user.name="Python"
    user.uid=uid
    user.role="caculater"
    client.Request("on_login",user.to_json_str(),b'',on_login_rc)
    client.OnNotice("on_cap",on_cap)
    pass

# def test():
#     cap = cv2.VideoCapture("demo/jntm.mp4")
#     asci_art=AsciiArt() 
#     while True:
#         ret, frame_src1 = cap.read()
#         if ret:
#             img= asci_art.transform(frame_src1)
#             cv2.imshow("img",img)
#         key = cv2.waitKey(1)
#         if key == 27:
#             break

if __name__ == '__main__':
    # test()
    client.OnEnd(OnEnd)
    client.OnEnter(OnEnter)
    client.Start(uid,"ws://127.0.0.1:8080")
    while (True):
        time.sleep(0.01)
        client.Loop()
        pass
    pass