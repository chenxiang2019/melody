CC = g++ -g
CFLAGS = -std=c++11
GUROBI = /opt/gurobi1101/linux64
INC = -I$(GUROBI)/include  -I/usr/include
LDFLAGS = -L$(GUROBI)/lib -lgurobi_c++ -lgurobi110 -lcgraph -L/usr/lib
# LDFLAGS = -L$(GUROBI)/lib -lgurobi_c++ -lpthread -lcgraph -L/usr/lib
SRCS = analyzer/parser.cpp analyzer/analyzer.cpp optimizer/optimizer.cpp optimizer/heuristic.cpp melody.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = melody
$(TARGET): $(OBJS)
	@$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) 
%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

# CC = g++ -g
# CFLAGS = -std=c++11
# GUROBI = /opt/gurobi1101/linux64
# INC = -I$(GUROBI)/include  -I/usr/include
# LDFLAGS = -L$(GUROBI)/lib -lgurobi_c++ -lgurobi110 -lcgraph -L/usr/lib
# # LDFLAGS = -L$(GUROBI)/lib -lgurobi_c++ -lpthread -lcgraph -L/usr/lib
# SRCS =  a.cpp
# OBJS = $(SRCS:.cpp=.o)
# TARGET = a
# $(TARGET): $(OBJS)
# 	@$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) 
# %.o: %.cpp
# 	$(CC) $(CFLAGS) $(INC) -c $< -o $@
# .PHONY: clean
# clean:
# 	rm -f $(OBJS) $(TARGET)