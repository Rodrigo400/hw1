# cs335 lab1
# to compile your project, type make and press enter

all: hw1

hw1: hw1.cpp
	g++ hw1.cpp -Wall -olab1 -lX11 -lGL -lGLU -lm

clean:
	rm -f hw11
	rm -f *.o

