RV_ASPECTS = rv_main.ah 
RV_SOLUTIONS = rv_solutions.cpp rv_globals.cpp

AG = ag++
SOURCES = main.cpp
TARGETS = main
LIBS = -L../../modules/event/ -levent -lboost_serialization -lboost_system -lboost_thread

$(TARGETS): $(SOURCES) $(RV_SOLUTIONS) $(RV_ASPECTS)
	make -C ../../modules/event
	$(AG) -g -o $(TARGETS) $(SOURCES) $(RV_SOLUTIONS) -a $(RV_ASPECTS) $(LIBS)

