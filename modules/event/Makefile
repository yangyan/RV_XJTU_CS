INCLUDE = -I /home/yangyan/boost_1_50_0/
LIBS	= -lboost_serialization -lboost_system -lboost_thread
GPP	= g++
GCC	= gcc
DEBUG	= -g

EVENT_SOURCE = Argument.hpp ArgumentList.hpp Event.hpp EventManager.hpp EventPathHistory.hpp EventPath.hpp EventQueue.hpp EventTime.hpp 

API_SOURCE = event_api.cpp

OBJECTS = $(EVENT_SOURCE:.hpp=.o) $(API_SOURCE:.cpp=.o)

event.a : $(OBJECTS)
	ar -r $@ $^

%.o : %.hpp
	$(GCC) -c $(DEBUG) $(INCLUDE) -o $@ $^ 

%.o : %.cpp
	$(GCC) -c $(DEBUG) $(INCLUDE) -o $@ $^ 

test: test2.cpp event.a
	$(GPP) $(DEBUG) $(LIBS) -o $@ $^
