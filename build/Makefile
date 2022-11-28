CCFLAGS= -Wall
CC= g++

all: algochain
		
algochain: main.o cmdline.o sha256.o
		$(CC) $(CCFLAGS) -o algochain main.o cmdline.o sha256.o

main.o: ../src/main.cc ../src/cmdline.cc ../src/sha256.cpp ../inc/Array.h ../inc/block.h ../inc/cmdline.h ../inc/dictionary.h ../inc/finders.h ../inc/sha256.h ../inc/tools.h ../inc/user.h ../inc/prototypeLista.h ../inc/Lista.h ../inc/main.h
		$(CC) $(CCFLAGS) -c ../src/main.cc -o main.o 

cmdline.o: ../src/cmdline.cc ../inc/cmdline.h
		$(CC) $(CCFLAGS) -c ../src/cmdline.cc -o cmdline.o 

sha256.o: ../src/sha256.cpp ../inc/sha256.h
		$(CC) $(CCFLAGS) -c ../src/sha256.cpp -o sha256.o

clean:
		$(RM) *.o algochain