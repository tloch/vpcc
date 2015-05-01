//#include "glue.h"

#include <stdarg.h>

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

	// arguments: syscall number, file handle, pointer to userspace buffer, buffer length, constant 0
	mipster_syscall(4003, 0, (unsigned int)&buffer, 1, 0); // __NR_read

	return (int)buffer;
}

int putchar(int c) {
	// this relies on integers being stored little-endian, i.e. least significant byte first
	// somehow mipster is missing instructions that prevent assigning a char variable from an int value.

	// arguments: syscall number, file handle, pointer to userspace buffer, buffer length, constant 0
	mipster_syscall(4004, 1, (unsigned int)&c, 1, 0); // __NR_write

	return c;
}

int puts(char *str) {
	int len = 0;
	for(; str[len] != '\0'; len++);

	// arguments: syscall number, file handle, pointer to userspace buffer, buffer length, constant 0
	mipster_syscall(4004, 1, (unsigned int)str, len, 0);

	return 1;
}


int print_int(int i) {
	int sign;
	int pos = 0; // index of next free position in buffer
	int buffer[16];
	if(i < 0) {
		sign = -1; i *= -1;
		putchar(45); // '-'
	}

	if(i == 0) {
		// i = 0: special case because it wouldn't output any digits at all
		buffer[pos] = 48; // '0'
		pos++;
	} else {
		// i > 0: guaranteed to have at least one loop iteration
		while(i != 0) {
			buffer[pos] = 48 + (i % 10);
			pos++;
			i /= 10;
		}
	}

	while(pos > 0) {
		pos--;
		putchar(buffer[pos]);
	}

	return 0;
}

int printf(char *fmt, ...) {
	va_list ap;
	char *f;
	char c;
	int i;
	char *s;

	va_start(ap, fmt);

	for(f = fmt; *f != '\0'; f++) {
		if(*f != '%') {
			putchar(*f);
			continue;
		}

		f++;
		if(*f == '\0') continue;

		switch(*f) {
		case 'c':
			i = va_arg(ap, int);
			putchar(i);
			break;
		case 's':
			s = va_arg(ap, char*);
			puts(s);
			break;
		case 'd':
			i = va_arg(ap, int);
			print_int(i);
			break;
		default:
			putchar('%');
			putchar(*f);
		}
	}

	va_end(ap);

	return 0;
}

