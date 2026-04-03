CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra

proj: pipeline.cpp pipeline.h
	$(CXX) $(CXXFLAGS) -o proj pipeline.cpp


clean:
	rm -f proj
