CC = gcc
CROSS_CC = arm-linux-gcc

SRC = ehdaemon.c
TARGET = ehdaemon

$(TARGET) : $(SRC)
	$(CC) $(SRC) -o $(TARGET)

armcompile:
	$(CROSS_CC) $(SRC) -o $(TARGET)

clean:
	rm $(TARGET)
