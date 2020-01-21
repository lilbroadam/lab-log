lablog: lablog.c lablog.h commandhandler.c commandhandler.h
	gcc lablog.c commandhandler.c -o lablog

clean:
	rm lablog
