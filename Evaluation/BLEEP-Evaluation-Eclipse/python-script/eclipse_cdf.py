import os

def getCDF(start_time, eclipse, ip_map):
    # Log information
    shadow_plugin = "PEER"
    datadir = "eclipse-datadir"

    # Create log file
    f = open("eclipse-data.txt",'w')

    # Crawling the data from target node, bleep0
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
            
                log = str(etime)+" "+str(float(tcnt)/float(35))+" "+str(float(ecnt)/float(35))
                #print log
                f.write(log+"\n")

    # Close log file
    f.close()

