src=$(wildcard *.cpp)
objs=$(patsubst %.cpp,%.o,$(src))
target=SerMain
LIBS=-lstdc++ -lssl -lcrypto -ljson
$(target):$(objs)
	g++ -o $@ $^ $(LIBS) 

%.o:%.cpp
	g++ -std=c++11 -c $<

.PHONY:clean
clean:
	rm -f $(objs)
