CCFLAGS= -Wall -pedantic
CC= g++

all: algochain 
	
algochain: main.o cmdline.o sha256.o
	$(CC) $(CCFLAGS) -o algochain main.o cmdline.o sha256.o

main.o: ../src/main.cc ../inc/cmdline.h ../inc/sha256.h ../inc/Array.h ../inc/block.h ../inc/main.h
	$(CC) $(CCFLAGS) -c ../src/main.cc -o main.o 

cmdline.o: ../src/cmdline.cc ../inc/cmdline.h
	$(CC) $(CCFLAGS) -c ../src/cmdline.cc -o cmdline.o 

sha256.o: ../src/sha256.cpp ../inc/sha256.h
	$(CC) $(CCFLAGS) -c ../src/sha256.cpp -o sha256.o

clean:
	$(RM) *.o algochain