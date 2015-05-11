# Apollo Pi Rover Makefile
# TODO: make this file more useful, ie. clean, use variables, etc.

AVRDUDE := avrdude
AVRCC   := avr-g++

INCLUDE := -I${QPCPP}/include -I${QPCPP}/ports/posix/gnu
LINK    := -lqp -lpthread -ljson -lssl -lcrypto -lwiringPi
LIBS    := -L${QPCPP}/ports/posix/gnu/dbg

all:
	arm-bcm2708hardfp-linux-gnueabi-g++ src/*.cpp ${INCLUDE} ${LIBS} ${LINK} -o bin/apollo-pi -std=c++11

#TODO compile objects, then link

#TODO clean rule
