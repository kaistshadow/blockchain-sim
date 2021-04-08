from collections import defaultdict
import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import matplotlib.ticker


f = open("shadow.data/hosts/tester/stdout-tester.MONEROP2P_TESTER.1000.log", "r")

timedata = []
hijackedConnCountData = []
for line in f:
    if "outgoing connection hijacked" in line:
        print(line)
        hijackedOutgoingNum = int(line.split(",")[2].split("=")[1])
        time = datetime.datetime.strptime(line.split(",")[0], '%Y-%m-%d %H:%M:%S')
        timedata.append(time)
        hijackedConnCountData.append(hijackedOutgoingNum)

ax = plt.gca()
# x = list(xy.values()[:20])
# yint = range(min(x), max(x) + 1)
# plt.yticks(yint)
plt.xticks(rotation=40)
formatter = mdates.DateFormatter("%H:%M:%S")
ax.xaxis.set_major_formatter(formatter)
# ax.xaxis.set_major_locator(mdates.MinuteLocator(byminute=(0,30)))
ax.xaxis.set_major_locator(mdates.HourLocator(byhour=[0,12]))
ax.set_xlim(datetime.datetime(2020,1,1), timedata[-1])
ax.set_ylim([0,max(hijackedConnCountData)])

locs = [mdates.date2num(timedata[0])] + list(ax.get_xticks())
locator= matplotlib.ticker.FixedLocator(locs)
ax.xaxis.set_major_locator(locator)

plt.plot(timedata, hijackedConnCountData, label='Hijacked outgoing connection num')
plt.legend()
plt.show()