CXXFLAGS=--std=c++11

src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
dep = $(obj:.o=.d)

-include $(dep)

all: hlx-p2-b

hlx-p2-b: $(obj)
	g++ -o $@ $^

%.d: %.cpp
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

clean:
	rm -f $(obj) $(dep) hlx-p2-b