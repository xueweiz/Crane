#C compiler
CC = g++

SRC = main.cpp 
SRC += connections.cpp ChronoCpu.cpp Chrono.cpp
SRC += Membership.cpp FileSystem.cpp 
SRC += Tuple.cpp Bolt.cpp Crane.cpp Supervisor.cpp Spout.cpp
SRC += BoltGender.cpp SpoutTwits.cpp

CC_FLAGS = -pthread -std=c++11

EXE = crane

release:$(SRC)
	$(CC)    $(SRC) $(CC_FLAGS) -o $(EXE) 

clean: $(SRC)
	rm -f $(EXE) $(EXE_X) $(EXE).linkinfo 
