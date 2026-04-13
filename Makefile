CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

proj: main.cpp simulator.cpp trace.cpp hazard.cpp stat.h instruction.h depth_config.h trace.h hazard.h simulator.h
	$(CXX) $(CXXFLAGS) -o proj main.cpp simulator.cpp trace.cpp hazard.cpp


clean:
	rm -f proj *.o
