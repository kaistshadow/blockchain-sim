#python calcResult.py [nodenum]

import sys
import datetime
import json


def calc_Block_Num(num) :
	totalNum=0
	for i in range(0,num):
		path="./datadir/hosts/client" + str(i) + "/stdout-client"+str(i)+ ".client."+str(num+i)+".log"
		file=open(path,"r")
		if not file : 
			print("error! file is not open")
		data=file.readlines()
		print("data=",data[3])
		dict=json.loads(data[3])
		totalNum += dict["result"]["blocks"]
		file.close()

	blockNum=totalNum/num
	return blockNum

def calc_Tx_Num(num) :
	totalNum=0
	for i in range(0,num):
		path="./datadir/hosts/client" + str(i) + "/stdout-client"+str(i)+ ".client."+str(num+i)+".log"
		file=open(path,"r")
		if not file :
			print("error! file is not open")
		data=file.readlines()
		print(data[4])
		dict=json.loads(data[4])
		print("txcount = ",dict["txCount"])
		totalNum += dict["txCount"]
		file.close()

	txNum=totalNum/num
	return txNum


def writetToFile(nodenum,avgBlockNum,txNum,simultime):

	bps = float(avgBlockNum)/float(simultime)
	tps = float(txNum)/float(simultime)

	path = "./datadir/result_node"+str(nodenum)+"_"+str(simultime)+".log"
	file = open(path,"w")
	file.write("-------------------Test Result ---------------------\n")
	str_write = "report %s\n"% str(datetime.datetime.now()) 
	file.write(str_write)
	str_write = "BlockNum = %d\n"% avgBlockNum
	file.write(str_write)
	str_write = "TxNum = %d\n"% txNum
	file.write(str_write)
	str_write = "Simultime = %d\n" % simultime
	file.write(str_write)
	str_write = "TPS(Transaction per Second = %f\n" % tps
	file.write(str_write)
	str_write = "BPS(Block per Second) = %f/s \n" % bps
	file.write(str_write)
	file.close()


if __name__ == '__main__':
	nodenum = int(sys.argv[1])
	simultime = int(sys.argv[2])
	avgBlockNum = calc_Block_Num(nodenum)
	avgTxNum = calc_Tx_Num(nodenum)

	writetToFile(nodenum, avgBlockNum, avgTxNum, simultime)
