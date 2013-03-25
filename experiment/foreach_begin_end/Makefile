CC = ag++
CFLAGS = -g
SOURCE = main.cpp
OTHER_SOURCES = rv_globals.cpp rv_solutions.cpp
AC_HEAD = rv_main.ah
RV_HEADER = rv_flags.h
TARGET = fbe 
LIB = -L../../modules/event/ -levent -lboost_serialization -lboost_system -lboost_thread

$(TARGET): $(SOURCE) $(AC_HEAD) $(OTHER_SOURCES) $(RV_HEADER)
	$(CC) -o $(TARGET) $(SOURCE) $(OTHER_SOURCES) $(CFLAGS) -a $(AC_HEAD) $(LIB)
