EXE = SamsAnalysis

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cc)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

ROOTCFLAGS   := $(shell root-config --cflags) -Wl,--no-as-needed
ROOTLIBS     := $(shell root-config --glibs) -lTreePlayer 

CPPFLAGS += -Iinclude
CPPFLAGS +=  -Wall -O2 $(ROOTCFLAGS)

# un comment if Belle II style is desired
CPPFLAGS += -DBELLE     

# uncomment if pdf figures are desired instead of png
#CPPFLAGS += -DPDF     

# uncomment if you want the systematic plots to be automatically produced
#CPPFLAGS += -DPLOTSYSTEMATICS

CFLAGS += -Wall
LDFLAGS += -Llib
LDLIBS += -lm
LDLIBS += $(ROOTLIBS) 

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@ $(CPPFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ 

