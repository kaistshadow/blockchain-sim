if [ $# -ne 2 ]; then
  echo "script need 2 parameter about difficulty and policy(TPS/Latency) "
  exit 1
fi
if [ $1 -gt 3 ]; then
  echo "difficulty is smaller than 3"
  exit
fi

path=../../../testlibs/dump/difficulty_$1
if [ ! -e $path/coinflip_hash.txt ]; then
  echo "start to dump!"
  cd ../../../testlibs/datadirDump/
  sh startdump.sh $1
fi

sh clean_data.sh $1

if [ $2 = "TPS" ]; then
  shadow test-BitcoinTPS.xml > data/output.txt
elif [ $2 = "Latency" ]; then
  shadow test-BitcoinLatency.xml > data/output.txt
else
  echo "policy is not exist"
  exit 1
fi

