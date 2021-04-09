if [ $# -ne 1 ]; then
  echo "script need 1 parameter about difficulty "
  exit 1
fi
if [ $1 -gt 3 ]; then
  echo "difficulty is smaller than 3"
  exit
fi

sh clean_data.sh $1
shadow test-BitcoinP2P.xml
