main: main.cpp
	make -C ../../modules/event
	ag++ -g -o $@ $^ -a main.ah -L../../modules/event/ -levent -lboost_serialization -lboost_system -lboost_thread

