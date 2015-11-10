CC = g++
ARMCC = arm-none-linux-gnueabi-g++
#SRC = main.cpp platcomm.cpp tcplistener.cpp EHSCmdParser.cpp rdbManager.cpp knxcomm.cpp ip_knxapp.cpp ip_knxPtl.cpp ptlManager.cpp objManager.cpp logsys.cpp devMonitor.cpp ehDataPushOutor.cpp
SRC = *.cpp *.c ../test/src/*.cpp
OBJ = servertest
Target = servertest

CFLAG = -I"../include/" -I"../test/include/"
#################################################
## $(Target) : $(OBJ)
##	$(CC) $(OBJ) -o $(Target)
#################################################

$(OBJ) : $(SRC)
	$(CC) $(SRC) $(CFLAG) -o $(OBJ) -lpthread

armcompile:
	$(ARMCC) $(SRC) $(CFLAG) -o $(OBJ) -lpthread
	
install:
	cp $(Target) ../../../bin/
	cp $(Target) ~/rootfs/home/bin/

clean:
	rm $(Target)
