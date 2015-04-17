//#include "glue.c"


static int heap[1 * 1024 * 1024]; // 1Mword
static int bptr = 0;

int *malloc(int size) {
	int wsize = (size + 3) / 4; // round up to full words
	int *pos = heap + bptr;
	bptr += wsize;
	return pos;
}

int getchar() {
	char buffer;
//  mipster_syscall(4004, 1, (unsigned int)str, 15, 0);

	// arguments: syscall number, ???, pointer to userspace buffer, buffer length, constant 0
	mipster_syscall(4003, 1, (unsigned int)&buffer, 1, 0); // __NR_read

	return (int)buffer;
}

