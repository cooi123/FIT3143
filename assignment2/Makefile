CC = mpicc
CFLAGS = -Wall
LIBS = -lm
SIM_CODE = a2.c
CHARGING_NODE_CODE = charging_nodes.c
all: sim

sim: $(SIM_CODE) $(CHARGING_NODE_CODE)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@


clean:
	rm -f sim

.PHONY: all clean