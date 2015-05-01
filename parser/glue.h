#ifndef _GLUE_H
#define _GLUE_H 1

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


#define exit mipster_exit

extern int *malloc(int size);

extern int getchar();

extern int putchar(int c);
extern int puts(char *s);

extern int printf(char *fmt, ...);


#endif
