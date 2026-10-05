TARGET = cipher_prog
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17
LDFLAGS = -framework CoreFoundation

all: $(TARGET)

$(TARGET): cipher.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) cipher.cpp $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET).dSYM
	rm -f test.enc test.dec test_wrong.dec