# SPDX-License-Identifier: MIT WITH bison-exception WITH swig-exception
# Copyright © 2020 Matthew Stern, Benjamin Michalowicz

CC=gcc
CXX=g++
MCS=$(MCS)

LDFLAGS= -lm -lpthread -L. -ltopologic -pthread #-lfl
CFLAGS=-Wall	-Werror	-g	-fPIC -O2 #-fsanitize=thread 
OBJ=$(SRC:.c=.o)
AR=ar

BIN=topologic
SRC=$(wildcard src/*.c)
INCLUDES= $(wildcard include/*.h)

FLEX=parse/topologic_parser.lex
BISON=parse/topologic_parser.y
FLEX_C=$(FLEX:.lex=.yy.c)
BISON_C=$(BISON:.y=.tab.c)
BISON_H=$(BISON:.y=.tab.h)
FLEX_OBJ=$(FLEX_C:.c=.o)
BISON_OBJ=$(BISON_C:.c=.o)

FLEXPP=parse/topologic_parser_cpp.lex
BISONPP=parse/topologic_parser_cpp.ypp
FLEX_CPP=$(FLEXPP:.lex=.yy.cpp)
BISON_CPP=$(BISONPP:.ypp=.tab.cpp)
BISON_HPP=$(BISONPP:.ypp=.tab.hpp)
FLEX_OBJ_PP=$(FLEX_CPP:.cpp=.o)
BISON_OBJ_PP=$(BISON_CPP:.cpp=.o)

TESTS=$(TEST_SRC:.c=)#ADD MORE AS THEY GO
TEST_SRC=$(wildcard testing/*.c)  #ADD MORE IF NEED BE
TEST_OBJ=$(TEST_SRC:.c=.o)
TEST_DIR=testing

all: $(BISON) $(BISON_C) $(BISON_H) $(FLEX) $(FLEX_C) $(BIN) $(TESTS) 

$(FLEX_C):
	flex $(FLEX)
	mv lex.yy.c $(FLEX_C)
	$(CC) -fPIC -g -c $(FLEX_C) -o $(FLEX_OBJ) -lfl
$(BISON_C): $(BISON)
	bison -d $(BISON) -o $(BISON_C) #-Wcounterexample
	$(CC) -fPIC -g -c $(BISON_C) -o $(BISON_OBJ)

$(BIN): $(OBJ) $(INCLUDES) $(BISON_OBJ) $(FLEX_OBJ)
	$(AR) rcs libtopologic.a $(OBJ) $(BISON_OBJ) $(FLEX_OBJ)

$(TESTS): $(BIN) $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ libtopologic.a $(TEST_DIR)/$(@F).o $(LDFLAGS)

cpp: $(BISON_CPP) $(BISON_OBJ_PP) $(BISON_HPP) $(FLEX_CPP) $(FLEX_OBJ_PP) $(OBJ) $(INCLUDES) 
	$(AR) rcs libtopologic.a $(OBJ) $(BISON_OBJ_PP) $(FLEX_OBJ_PP)

$(FLEX_CPP):
	flex $(FLEXPP)
	mv lex.yy.cc $(FLEX_CPP)
	$(CXX) -fPIC -g -c $(FLEX_CPP) -o $(FLEX_OBJ_PP) -lfl
$(BISON_CPP): $(BISONPP)
	bison -d $(BISONPP) -o $(BISON_CPP)
	$(CXX) -fPIC -g -c $(BISON_CPP) -o $(BISON_OBJ_PP)

all:$(BIN)
.PHONY : clean cpp

clean:
	rm -f libtopologic.a
	rm -f $(TEST_OBJ) $(TESTS)
	rm -f $(FLEX_C) $(FLEX_OBJ)
	rm -f $(BISON_C) $(BISON_OBJ) $(BISON_H)
	rm -f $(FLEX_CPP) $(FLEX_OBJ_PP)
	rm -f $(BISON_CPP) $(BISON_OBJ_PP) $(BISON_HPP)
	rm -f $(OBJ) $(BIN)
