TARGET = hash_prog
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17
LDFLAGS = -framework CoreFoundation

all: $(TARGET)

$(TARGET): hash.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) hash.cpp $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET).dSYM
