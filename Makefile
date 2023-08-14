
CXXFLAGS=-g -Wall 
SRCS= eeprom.cc spidev_lib++.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: eeprom

eeprom: $(OBJS)
	g++ -g -o $@ $(OBJS)

%.o: %.cc
	$(CC) $(CXXFLAGS) -c $<
