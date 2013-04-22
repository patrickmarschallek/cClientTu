if [ $# == 2 ]
then
	./client $1 1234 4321 $2
else
	./client 127.0.0.1 1234 4321 123
fi
