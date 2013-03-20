INCLUDE = -I /home/yangyan/boost_1_50_0/
LIBS	= -lboost_serialization -lboost_system -lboost_thread
GPP	= g++
GCC	= gcc
DEBUG	= -g

EVENT_SOURCE = Argument.hpp ArgumentList.hpp Event.hpp EventManager.hpp EventPathHistory.hpp EventPath.hpp EventQueue.hpp EventTime.hpp MatchedEventPaths.hpp

API_SOURCE = event_api.cpp

OBJECTS = $(EVENT_SOURCE:.hpp=.o) $(API_SOURCE:.cpp=.o)

libevent.a : $(OBJECTS) ../communication/Sender.hpp
	ar csr $@ $^

%.o : %.hpp
	$(GCC) -c $(DEBUG) $(INCLUDE) -o $@ $^ 

%.o : %.cpp
	$(GCC) -c $(DEBUG) $(INCLUDE) -o $@ $^ 

test: test.cpp libevent.a
	$(GPP) $(DEBUG) $(LIBS) -o $@ $< -Levent

clean:
	rm *.o
	rm *.a
	rm test
