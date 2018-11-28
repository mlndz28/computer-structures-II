CC=g++
CFLAGS=-lpthread -o
TARGET=bin

build:
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) $(TARGET)/pi main.cpp
clean:
	@rm $(TARGET) -r
package: build
	@tar -czf $(TARGET)/pi-calculator.tar.gz $(TARGET)/pi && zip $(TARGET)/pi-calculator.zip $(TARGET)/pi
test: build
	@$(TARGET)/pi
