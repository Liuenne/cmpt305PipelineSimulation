CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

<<<<<<< Updated upstream
proj: main.cpp trace.cpp hazard.cpp instruction.h stat.h depth_config.h trace.h hazard.h
	$(CXX) $(CXXFLAGS) -o proj main.cpp trace.cpp hazard.cpp
=======
proj: main.cpp simulator.cpp trace.cpp hazard.cpp stat.h instruction.h depth_config.h trace.h hazard.h simulator.h
	$(CXX) $(CXXFLAGS) -o proj main.cpp simulator.cpp trace.cpp hazard.cpp
>>>>>>> Stashed changes


clean:
	rm -f proj *.o
