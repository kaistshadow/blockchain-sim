if [ $# -ne 1 ]; then
  echo "script need 1 parameter about policy(TPS/Latency) "
  exit 1
fi
if [ ! -e ./data/key.txt ]
 then
  echo "make new key.txt"
  mkdir -p data/
  touch ./data/key.txt
fi
if [ ! -e ./data/state.txt ]
then
  echo "make new state.txt"
  touch ./data/state.txt
fi

if [ -e ./data/output.txt ]
then
  echo "delete output file"
  rm -rf ./data/output.txt
fi

if [ $1 = "TPS" ]; then
  shadow test-BLEEPLib-TPS.xml > data/output.txt
elif [ $1 = "Latency" ]; then
  shadow test-BLEEPLib-Latency.xml > data/output.txt
else
  echo "policy is not exist"
  exit 1
fi