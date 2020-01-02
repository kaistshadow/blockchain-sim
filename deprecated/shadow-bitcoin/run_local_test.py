import os
import argparse
import time

def run_bitcoin_serv(bitcoin_path):
    bitcoind_path = bitcoin_path+"/bin/bitcoind"
    if not os.path.exists(bitcoind_path):
        print "No bitcoind is installed (%s Not Found)" % bitcoind_path
        exit(1)
    
    if not os.path.exists("bitcoin_datadir"):
        os.system("mkdir -p bitcoin_datadir")

    os.system("%s -regtest -daemon -datadir=./bitcoin_datadir" % bitcoind_path)
        
def run_bitcoin_cli(bitcoin_path):
    bitcoin_cli_path = bitcoin_path+"/bin/bitcoin-cli"
    if not os.path.exists(bitcoin_cli_path):
        print "No bitcoin-cli is installed (%s Not Found)" % bitcoin_cli_path
        exit(1)
    
    # generate 10 blocks
    os.system("%s -regtest -datadir=./bitcoin_datadir generate 10" % bitcoin_cli_path)        

def run_bitcoin_block_browser():
    bitcoin_abe_path = "./bitcoin-abe"
    if not os.path.exists(bitcoin_abe_path):
        print "No bitcoin-abe is installed (%s Not Found)" % bitcoin_abe_path
        os.system("git clone https://github.com/kaistshadow/bitcoin-abe")
        os.system("cd %s; sudo python setup.py install" % bitcoin_abe_path)

    os.system("echo 'dbtype psycopg2' > %s/abe-pg.conf" % bitcoin_abe_path)
    os.system("echo 'connect-args {\"database\":\"abe\"}' >> %s/abe-pg.conf" % bitcoin_abe_path)
    os.system("echo 'upgrade' >> %s/abe-pg.conf" % bitcoin_abe_path)
    os.system("echo 'port 2750' >> %s/abe-pg.conf" % bitcoin_abe_path)
    os.system("echo 'datadir = %s' >> %s/abe-pg.conf" % (os.path.abspath("./bitcoin_datadir/regtest"), bitcoin_abe_path) )

    os.system("sudo -u postgres psql -c 'drop database abe'")
    os.system("sudo -u postgres createdb abe")
    os.system("cd %s; python -m Abe.abe --config abe-pg.conf" % bitcoin_abe_path)

def stop_bitcoin_serv(bitcoin_path):
    bitcoin_cli_path = bitcoin_path+"/bin/bitcoin-cli"
    if not os.path.exists(bitcoin_cli_path):
        print "No bitcoin-cli is installed (%s Not Found)" % bitcoin_cli_path
        exit(1)
    
    # stop bitcoind
    os.system("%s -regtest -datadir=./bitcoin_datadir stop" % bitcoin_cli_path)        

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run local test using bitcoin regtest' )
    parser.add_argument("--bitcoin-path", action="store", dest="bitcoin_path", help="bitcoin installed path")
    args = parser.parse_args()

    if args.bitcoin_path == None:
        print "argument for bitcoin path is required."
        print "ex) python run_local_test.py --bitcoin-path ~/.bitcoin/"
        exit(1)

    run_bitcoin_serv(args.bitcoin_path)
    print "wait for warming up server..."
    time.sleep(3)
    run_bitcoin_cli(args.bitcoin_path)
    time.sleep(1)
    run_bitcoin_block_browser()
    stop_bitcoin_serv(args.bitcoin_path)
