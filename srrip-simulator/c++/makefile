CC=g++
CFLAGS=-o
TARGET=bin

build:
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) $(TARGET)/cache main.cpp cache.cpp
clean:
	@rm $(TARGET) -r
package: build
	@tar -czf $(TARGET)/cache.tar.gz $(TARGET)/cache && zip $(TARGET)/cache.zip $(TARGET)/cache
