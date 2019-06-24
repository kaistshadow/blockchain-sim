#!/usr/bin/python
import os
import sys
import xml.etree.ElementTree as ET

namespace = "http://graphml.graphdrawing.org/xmlns"
namespace_prefix = "{" + namespace + "}"
ET.register_namespace('', namespace)

def genNewXml (orgfile, newfile, maxNum) :
    s = set()
    of = open(orgfile, 'r')
    nf = open(newfile, 'w')
    lineNum = 0
    nodeNum = 0
    selectedNodeNum = 0
    while True:
        line = of.readline()
        if lineNum < 12 :
            nf.write(line)
	    lineNum += 1
        elif '<node' in line:
            if selectedNodeNum == maxNum : continue
            if nodeNum % 3 == 0 :
                str = line.split('\"')
                id = str[1]
                s.add(id)
                nf.write(line)
                nf.write(of.readline())
                nf.write(of.readline())
                nf.write(of.readline())
                nf.write(of.readline())
                nf.write(of.readline())
                nf.write(of.readline())
                nf.write(of.readline())
                nf.write(of.readline())
                selectedNodeNum += 1
            nodeNum += 1
        elif '<edge' in line :
            str = line.split('\"')
            if str[1] in s and str[3] in s :
                nf.write(line)
                nf.write(of.readline())
                nf.write(of.readline())
                nf.write(of.readline())
                nf.write(of.readline())
        elif '</graph' in line :
            nf.write(line)
            nf.write(of.readline())
	    break

    of.close()
    nf.close()

if __name__ == '__main__':
    if len(sys.argv) < 4:
        sys.exit(0)
    orgfile = sys.argv[1]
    newfile = sys.argv[2]
    maxNum = int (sys.argv[3])
    genNewXml(orgfile, newfile, maxNum)

