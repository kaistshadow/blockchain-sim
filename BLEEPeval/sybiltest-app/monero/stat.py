from collections import defaultdict
import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import math

f = open("shadow.data/hosts/tester/stdout-tester.MONEROP2P_TESTER.1000.log", "r")

ipcount = defaultdict(int)

closecount = defaultdict(int)

for line in f:
    if "NodeIP:" in line:
        ipcount[line.split("NodeIP:")[1].strip()] += 1
    if "closed while recv, myIP=" in line:
        closecount[line.split("myIP=")[1].strip()] += 1

print("ipcount's element num = %d" % len(ipcount))
print("closecount's element num = %d" % len(closecount))
print(ipcount)

print(closecount)

f = open("shadow.data/hosts/tester/stdout-tester.MONEROP2P_TESTER.1000.log", "r")

timedata = []
benignNodeCountdata = []
shadowNodeCountdata = []
for line in f:
    if "(socket) connected" in line or "(socket) disconnected" in line:
        print(line)
        benignNodeCount = int(line.split(",")[2].split("=")[1])
        shadowNodeCount = int(line.split(",")[3].split("=")[1])
        time = datetime.datetime.strptime(line.split(",")[0], '%Y-%m-%d %H:%M:%S')
        timedata.append(time)
        benignNodeCountdata.append(benignNodeCount)
        shadowNodeCountdata.append(shadowNodeCount)

ax = plt.gca()
# x = list(xy.values()[:20])
# yint = range(min(x), max(x) + 1)
# plt.yticks(yint)
plt.xticks(rotation=40)
formatter = mdates.DateFormatter("%H:%M:%S")
ax.xaxis.set_major_formatter(formatter)
ax.xaxis.set_major_locator(mdates.MinuteLocator(interval=30))
plt.plot(timedata, benignNodeCountdata, label='BenignNode connection')
plt.plot(timedata, shadowNodeCountdata, label='ShadowNode connection')
plt.legend()
plt.show()