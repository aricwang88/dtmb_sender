
CC  = g++
CXX = g++
STRIP = strip


INCLUDE = -I./  -I./include
CFLAGS = -Wall -Werror -fno-strict-aliasing   $(INCLUDE) -g 
CXXFLAGS = -Wall -Werror -fno-strict-aliasing   $(INCLUDE) -g 
LIB = -L./lib -lm -lz -lpthread -lrt -ldl -ljsoncpp -lcyassl
 
PROGOBJS = main.o Socket.o Udp.o configSet.o cmmbFile.o reed_solom.o CRC_32.o bsDataProc.o cmdDataParse.o \
		   ver.o threadFunc.o

PGM = dtmbSender

#.SILENT:

all: $(PGM)

$(PGM): $(PROGOBJS)
		$(CC) -o $(PGM) $(PROGOBJS) $(LIB)
	
#$(STRIP) $(PGM)


distclean: clean

clean:
	rm -f *.o  *.out *.a *.so *.exe core $(PGM)



