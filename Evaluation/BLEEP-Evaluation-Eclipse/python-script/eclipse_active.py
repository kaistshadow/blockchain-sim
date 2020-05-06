import os

def getCDF(start_time, eclipse, ip_map):
    # Log information
    shadow_plugin = "PEER"
    datadir = "eclipse-datadir"

    # Create log file
    f = open("eclipse-data-active-view.txt",'w')

    # Crawling the data from target node, bleep0
    # size of active view = 4 (default)
    activesize = 4 
    outputfile = "./%s/hosts/bleep0/stdout-bleep0.%s.1000.log" % (datadir, shadow_plugin)
    print "\n[Time vs. Active Veiw Ratio]"
    print "---------------------------------------------------------------------"
    print "time(ms)".rjust(10)+" |"+"time(s)".rjust(10) +" |"+"ratio(#node/view)".rjust(20)+" |"+"ratio(#malicious/view)".rjust(20)
    print "---------------------------------------------------------------------"
    if os.path.exists(outputfile):
        fileHandle = open(outputfile,"r")
        lineList   = fileHandle.readlines()
        fileHandle.close()
        for idx in range(0, len(lineList)):
            parsing = lineList[idx].split('\n')[0].split(' ')
            header  = parsing[0]
            if header == "[NetworkGraph]":
                etime = int(parsing[1])-start_time
                enode = parsing[2].split(',')

                tcnt = len(enode)
                ecnt = 0
                for node in enode:
                    if int(ip_map[node[:-3]]) in eclipse:
                        ecnt = ecnt + 1
            
                log = str(etime).rjust(10)+"  "+str(float(etime)/1000.0).rjust(10) +"  "+str(float(tcnt)/float(activesize)).rjust(20)+"  "+str(float(ecnt)/float(activesize)).rjust(20)
                print log
                f.write(log+"\n")
    print "---------------------------------------------------------------------"
    f.close()

