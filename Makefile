## 其余变量
VER:=release ## 调试选项
OPTM:=elide ## 构造器优化选项
STD:=c++11
CPP:=g++

## 头文件
HEADER:=include
## 可执行程序
EXE_RECOMMENDER:=bin/offline1
EXE_SEARCHER:=bin/offline2
EXE_OFFLINE:= $(EXE_RECOMMENDER) $(EXE_SEARCHER)
EXE_SERVER:=bin/server
EXE_CLIENT:=bin/client
EXE_TEST:=bin/test
EXES:= $(EXE_OFFLINE) $(EXE_SERVER) $(EXE_CLIENT) $(EXE_TEST)
## 源程序
SRC_RECOMMENDER:=$(wildcard src/offline/module1/*.cpp)
SRC_SEARCHER:=$(wildcard src/offline/module2/*.cpp)
SRC_SERVER:=$(wildcard src/online/*.cpp) $(wildcard src/online/net/*.cpp)
SRC_CLIENT:=$(wildcard src/client/*.cpp)
SRC_TEST:=$(wildcard src/test/*.cpp)

## 测试变量
# all:
# 	echo $(SRC_SERVER) 

## Compilation parameters
ifeq ($(VER), debug) 
	OPT+=-g
	OPT+=-D __DEBUG__
endif

ifeq ($(OPTM), no-elide)
	OPT+=-fno-elide-constructors
endif



all:$(EXE_SERVER) $(EXE_CLIENT)

offline:$(EXE_OFFLINE)

recommender:$(EXE_RECOMMENDER)

searcher:$(EXE_SEARCHER)

server:$(EXE_SERVER)

client:$(EXE_CLIENT)

test:$(EXE_TEST)



$(EXE_RECOMMENDER):$(SRC_RECOMMENDER)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) $(OPT)

$(EXE_SEARCHER):$(SRC_SEARCHER)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) $(OPT)

$(EXE_SERVER):$(SRC_SERVER)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) $(OPT)

$(EXE_CLIENT):$(SRC_CLIENT)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) $(OPT)

$(EXE_TEST):$(SRC_TEST)
	$(CPP) $^ -o $@ -Wall -O0 -llog4cpp -lpthread -lredis++ -lhiredis -I $(HEADER) -std=$(STD) $(OPT)


## 伪目标
.PHONY:clean rebuild
clean:
	$(RM) $(EXES)
rebuild:clean $(EXES)