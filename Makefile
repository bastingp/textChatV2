#MakeFile to build and deploy the Sample US CENSUS Name Data using ajax
# For CSC3004 Software Development

# Put your user name below:
USER= schutzj

CC= g++ -std=c++11

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

RM= /bin/rm -f

all: textserver testclient textchat_ajax PutCGI PutHTML
#all: nameserver testclient 

testclient.o: testclient.cpp fifo.h
	$(CC) -c $(CFLAGS) testclient.cpp

textserver.o: textserver.cpp fifo.h USER_H.h
	$(CC) -c $(CFLAGS) textserver.cpp

textchat_ajax.o: textchat_ajax.cpp fifo.h
	$(CC) -c $(CFLAGS) textchat_ajax.cpp

testclient: testclient.o fifo.o
	$(CC) testclient.o  fifo.o -o testclient
	
textserver: textserver.o fifo.o USER.o
	$(CC) textserver.o  fifo.o USER.o -o textserver

USER.o: USER.cpp USER_H.h
	$(CC) -c $(CFLAGS) USER.cpp

fifo.o:		fifo.cpp fifo.h
		g++ -c fifo.cpp
		
textchat_ajax: textchat_ajax.o  fifo.h
	$(CC) textchat_ajax.o  fifo.o -o textchat_ajax -L/usr/local/lib -lcgicc

PutCGI: textchat_ajax
	chmod 757 textchat_ajax
	cp textchat_ajax /usr/lib/cgi-bin/$(USER)_textchat_ajax.cgi 
	cp textchat_ajax.cpp /var/www/html/class/softdev/$(USER)

	echo "Current contents of your cgi-bin directory: "
	ls -l /usr/lib/cgi-bin/

PutHTML:
	cp textchat.html /var/www/html/class/softdev/$(USER)
	cp textchat.js /var/www/html/class/softdev/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o textchat_ajax textserver
