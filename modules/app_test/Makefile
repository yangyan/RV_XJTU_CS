INCLUDE = -I /home/yangyan/boost_1_50_0/
EVENTLIBS = -L../event/ -levent
LIBS	= -lboost_serialization -lboost_system -lboost_thread 
CC	= ag++ -k 
DEBUG	= -g

SOURCE	= test.cpp 

test: $(SOURCE)
	$(CC) $(DEBUG) -o $@ $^ $(EVENTLIBS) $(LIBS) 
