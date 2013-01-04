main: main.cpp
	make -C ../../modules/event
	ag++ -k -g -o $@ $^ -L../../modules/event/ -levent -lboost_serialization -lboost_system -lboost_thread

