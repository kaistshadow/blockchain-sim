# 1.Getting -dbgsym.ddeb packages(for libevent)
echo "deb http://ddebs.ubuntu.com $(lsb_release -cs) main restricted universe multiverse
deb http://ddebs.ubuntu.com $(lsb_release -cs)-updates main restricted universe multiverse
deb http://ddebs.ubuntu.com $(lsb_release -cs)-proposed main restricted universe multiverse" | \
sudo tee -a /etc/apt/sources.list.d/ddebs.list

sudo apt install ubuntu-dbgsym-keyring
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys F2EDC64DC5AEE1F6B9C621F0C8CAB6595FDFF622
sudo apt-get update

# 2.you need to dowload source libevent for debugging bitcoind libevent
sudo apt-get source libevent-2.1-6

# if you get error mesage "'source' URIs in your sources.list", then you must modify about "vi /etc/apt/sources.list" 
# if you install was successful, add "directory path" to gdbinit 