#!/usr/bin/python
import os
import sys

if __name__ == '__main__':
    if len(sys.argv) < 3:
        sys.exit(0)

    topofile = sys.argv[1]
    conffile = sys.argv[2]

    tp = open(topofile, 'r')
    cp = open(conffile, 'r')
    np = open("new" + conffile, 'w')

    lst = list()
    lines = tp.readlines()

    for line in lines :
        if '<node' in line :
            lst.append(line.split('\"')[1])
        if '<edge' in line :
            break

    lines = cp.readlines()

    i = 0
    for line in lines :
        if '<node' in line :
            if len(lst) <= i :
                np.write(line)
                continue
            id = line.split('\"')[1]
            str =  "  <node id=\"%s\" iphint=\"%s\">\n" % (id, lst[i].split('-')[1])
            np.write(str)
            i += 1
        else :
            np.write(line)


    tp.close()
    cp.close()
    np.close()


