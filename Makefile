CXX=g++
CXXFLAGS=-std=c++17 -fopenmp

TARGET=parallel-rand.out

$(TARGET):main.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)
  
clean:
	rm -f $(TARGET)
