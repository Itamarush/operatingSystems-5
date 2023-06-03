st_pipeline.o: st_pipeline.c
	gcc -c st_pipeline.c

activeObject.o: activeObject.c
	gcc -c activeObject.c

queue.o: queue.c
	gcc -c queue.c

all: st_pipeline.o activeObject.o queue.o
	gcc -o st_pipeline st_pipeline.o activeObject.o queue.o -I. -pthread

clean:
	rm -f st_pipeline.o activeObject.o queue.o st_pipeline

.PHONY: clean
