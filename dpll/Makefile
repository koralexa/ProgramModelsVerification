SOURCE=dpll.cpp test.cpp
TARGET=test

CFLAGS = -I. -Wall -std=c++17 -O3

all: $(TARGET)

$(TARGET): $(SOURCE)
	g++ $(CFLAGS) -o $@ -I. $^

run:
	./$(TARGET)

clean:
	rm -rf $(TARGET)

