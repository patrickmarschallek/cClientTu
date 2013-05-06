if [ $# == 3 ]
then
	./client $1 1234 4321 $2 $3
else
	./client 127.0.0.1 1234 4321 234
fi
