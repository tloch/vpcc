#ifndef _SCANNER_H
#define _SCANNER_H 1

int init_scanner();
int getSymbol();

void print_token(int token);


extern int* identifier; // stores scanned identifier
extern int integer; // stores scanned integer

extern int maxIdentifierLength;


// other tokens
extern int IDENTIFIER;
extern int INTEGER;

// keyword tokens
extern int VOID;
extern int IF;
extern int INT;
extern int WHILE;
extern int ELSE;
extern int RETURN;

// special character and operator tokens
extern int SEMICOLON;
extern int ASTERISK;
extern int LPARENS;
extern int RPARENS;
extern int LBRACE;
extern int RBRACE;
extern int EQUAL;
extern int PLUS;
extern int MINUS;
extern int ASSIGN;
extern int GT;
extern int GTEQ;
extern int LT;
extern int LTEQ;
extern int COMMA;
extern int NEQUAL;
extern int NEGATE;
//extern int ;
//extern int ;
//extern int ;
//extern int ;

#define LEFTPARENTHESIS		LPARENS
#define RIGHTPARENTHESIS	RPARENS

#define LEFTBRACES		LBRACE
#define RIGHTBRACES		RBRACE

// FIXME: add new tokens here!


#endif

