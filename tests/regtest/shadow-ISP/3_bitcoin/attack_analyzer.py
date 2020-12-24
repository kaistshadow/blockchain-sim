import datetime
import os
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import math

benign_node_logfile = "shadow.data/hosts/ISP-emulated_benigns/stdout-ISP-emulated_benigns.emulated_benigns.1000.log"
bitcoin_logfile = "shadow.data/hosts/bcdnode0/stdout-bcdnode0.bitcoind.1000.log"
xy = {}
cur_outgoing_peerid = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
churnout = []

if __name__ == '__main__':

    curtime = 0
    outgoing_count = -1
    if os.path.exists(benign_node_logfile):
        print("log file {} exists".format(benign_node_logfile))
    with open(bitcoin_logfile, "r") as fp:
        for line in fp:
            if "disconnecting" in line:
                peerid = int(line.split("=")[1])
                if peerid in cur_outgoing_peerid:
                    # print(line.split()[0])
                    churnout.append(datetime.datetime.strptime(line.split()[0], '%Y-%m-%dT%H:%M:%SZ'))
                    cur_outgoing_peerid.remove(peerid)
                    for connectline in fp:
                        if "Added connection" in connectline:
                            break
                    cur_outgoing_peerid.append(int(connectline.split("=")[1]))

    with open(benign_node_logfile, "r", encoding='unicode_escape') as fp:
        while True:
            try:
                line = fp.readline()
                if "cur_time" in line:
                    curtime = int(line.split(":")[1])
                    curdatetime = datetime.datetime.fromtimestamp(curtime) - datetime.timedelta(hours=9)
                elif "outgoing count" in line:
                    new_outgoing_count = int(line.split("=")[1])
                    if outgoing_count != new_outgoing_count:
                        outgoing_count = new_outgoing_count
                        print("%s,%d" % (curdatetime, outgoing_count))
                        xy[curdatetime] = outgoing_count
                elif "" == line:
                    break
            except UnicodeDecodeError:
                continue

    print(len(churnout))
    print(churnout)
    ax = plt.gca()
    ax.stem(churnout, [6 for x in churnout], markerfmt='x', basefmt=' ')
    # plt.plot(churnout, [10 for x in churnout], label="churnout")

    x = list(xy.values())
    yint = range(min(x), math.ceil(max(x)) + 1)
    plt.yticks(yint)
    plt.xticks(rotation=25)

    # ax = plt.gca()
    formatter = mdates.DateFormatter("%Y-%m-%d")
    ax.xaxis.set_major_formatter(formatter)

    locator = mdates.DayLocator()
    ax.xaxis.set_major_locator(locator)

    for dat in xy.keys():
        print(dat)
    plt.step(xy.keys(), xy.values(), label='Outgoing connection', where='post')
    plt.show()
