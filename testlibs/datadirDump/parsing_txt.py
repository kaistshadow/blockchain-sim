import json
import os

def writeToFile(rpccall, data):
    print("writeToFile",rpccall,data)
    filename=""
    if(rpccall== "dumpprivkey"):
        filename="key.txt"
    elif rpccall=="getrawtransaction":
        filename="state.txt"
    w=open("./data/"+filename,'w')
    w.write(data);
    w.close()


if (os.path.isfile("./data/coinflip_hash.txt")==False):
    print("error shadow is not successed")
    exit(0)

f = open("./shadow.data/hosts/client0/stdout-client0.client.1000.log")
isRequest = bool(False)
strcommand = ""

lines = f.readlines()
for line in lines:
    type = line.split(" ")
    if type[0] == "request":
        request= type[2];
        jsonObject = json.loads(request)
        method = jsonObject.get("method")
        if(method=="getrawtransaction" or method=="dumpprivkey" ):
            isRequest=True
            strcommand = method
    elif len(type)>1 and type[1]=="result" and isRequest==True and strcommand is not "" :
        response=line.find("{")
        response= line[response:]
        jsonObject = json.loads(response)
        data=""
        if(strcommand=="getrawtransaction"):
            data = jsonObject.get("result").get("hex")
        elif strcommand=="dumpprivkey":
            data=jsonObject.get("result")
        writeToFile(strcommand,data)
        isRequest=False
        strcommand=""

f.close()