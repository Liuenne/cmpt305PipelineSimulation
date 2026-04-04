CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

proj: main.cpp stat.h instruction.h depth_config.h
	$(CXX) $(CXXFLAGS) -o proj main.cpp 


clean:
	rm -f proj
