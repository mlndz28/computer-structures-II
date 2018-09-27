GC=go build
GFLAGS=-o
TARGET=bin

build:
	@$(GC) $(GFLAGS) $(TARGET)/cache *.go
clean:
	@rm $(TARGET) -r
package: build
	@tar -czf $(TARGET)/cache.tar.gz $(TARGET)/cache && zip $(TARGET)/cache.zip $(TARGET)/cache
