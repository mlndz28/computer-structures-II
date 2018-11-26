CC=mpic++
CFLAGS=-o
TARGET=bin

build:
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) $(TARGET)/mult util.cpp matrix.cpp main.cpp
clean:
	@rm $(TARGET) -r
package: build
	@tar -czf $(TARGET)/matrix-multiplier.tar.gz $(TARGET)/mult && zip $(TARGET)/matrix-multiplier.zip $(TARGET)/mult
test: build
	@mpirun -n 4 $(TARGET)/mult -s 10
