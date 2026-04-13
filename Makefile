CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

proj: main.cpp trace.cpp hazard.cpp instruction.h stat.h depth_config.h trace.h hazard.h
	$(CXX) $(CXXFLAGS) -o proj main.cpp trace.cpp hazard.cpp


clean:
	rm -f proj
