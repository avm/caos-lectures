ssefloat: float.c
	$(CC) $^ -m32 -o $@ -lm -msse4 -mfpmath=sse

float: float.c
	$(CC) $^ -m32 -o $@ -lm

clean:
	rm -f float ssefloat
