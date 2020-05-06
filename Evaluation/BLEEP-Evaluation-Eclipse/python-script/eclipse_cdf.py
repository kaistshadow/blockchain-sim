import os

def getCDF(start_time, eclipse, ip_map):
    # Log information
    shadow_plugin = "PEER"
    datadir = "eclipse-datadir"

    # Create log file
    f = open("eclipse-data-entire-view.txt",'w')

    # Crawling the data from target node, bleep0
    # size of partial view = 4 + 24 (default)
    partialview = 28 
    outputfile = "./%s/hosts/bleep0/stdout-bleep0.%s.1000.log" % (datadir, shadow_plugin)
    if os.path.exists(outputfile):
        fileHandle = open(outputfile,"r")
        lineList   = fileHandle.readlines()
        fileHandle.close()
        for idx in range(0, len(lineList)):
            parsing = lineList[idx].split('\n')[0].split(' ')
            header  = parsing[0]
            if header == "[ECLIPSE]":
                etime = int(parsing[1])-start_time
                enode = parsing[2].split(',')[0:-1]

                tcnt = len(enode)
                ecnt = 0
                for node in enode:
                    if int(ip_map[node]) in eclipse:
                        ecnt = ecnt + 1
            
                log = str(etime).rjust(10)+"  "+str(float(etime)/1000.0).rjust(10) +"  "+str(float(tcnt)/float(partialview)).rjust(20)+"  "+str(float(ecnt)/float(partialview)).rjust(20)
                f.write(log+"\n")
    f.close()

