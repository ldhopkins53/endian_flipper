all:
	gcc main.c -o main.bin
	gcc test.c -o test.bin
	readelf -h ./test.bin
	./main.bin ./test.bin
	readelf -h ./test.bin
clean:
	rm -vf *.bin
