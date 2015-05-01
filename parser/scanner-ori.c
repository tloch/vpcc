#include <stdio.h>
#include <stdlib.h>

int character; // most recently read character
int symbol; // most recently recognized symbol

// tokens
int IDENTIFIER;
int INTEGER;
int VOID;
int SEMICOLON;
int IF;
//...

int* identifier; // stores scanned identifier

// maximum number of characters in an identifier
// set as needed
int maxIdentifierLength;

int integer; // stores scanned integer

int getSymbol() {
    int* identifierCursor;
    int identifierLength;

    if (findNextCharacter() == -1)
        return -1;

    if (isCharacterLetter()) {
        identifier = malloc(maxIdentifierLength * 4);

        identifierCursor = identifier;
        identifierLength = 1;

        while (isCharacterLetterOrDigit()) {
            if (identifierLength >= maxIdentifierLength)
                exit(-1); // identifier too long

            *identifierCursor = character;
            identifierCursor = identifierCursor + 1;
            identifierLength = identifierLength + 1;

            character = getchar();
        }

        *identifierCursor = 0; // null terminated string

        return identifierOrKeyword();
    } else if (isCharacterDigit()) {
        integer = 0;

        while (isCharacterDigit()) {
            // caution: overflows remain undetected
            integer = integer * 10 + character - 48;

            character = getchar();
        }

        return INTEGER;
    } else if (character == 59) { // ASCII code 59 = ;
        character = getchar();

        return SEMICOLON;
    //} else if ( ... ) {

    ///...

    } else
        exit(-1); // unknown character
}

int findNextCharacter() {
    int inComment; // are we scanning a comment?

    inComment = 0;

    while (1) {
        if (inComment) {
            character = getchar();

            if (character == 10) // ASCII code 10 = linefeed
                inComment = 0;
            else if (character == 13) // ASCII code 13 = carriage return
                inComment = 0;
            else if (character == -1) // end of file is represented by -1
                return character;
        } else if (isCharacterWhitespace())
            character = getchar();
        else if (character == 35) { // ACCII code 35 = #
            character = getchar();

            inComment = 1;
        } else if (character == 47) { // ASCII code 47 = /
            character = getchar();

            if (character == 47) { // ASCII code 47 = /
                character = getchar();

                inComment = 1;
            } else
                return character;
        } else
            return character;
    }
}

int isCharacterWhitespace() {
    if (character == 32) // ASCII code 32 = space
        return 1;
    else if (character == 9) // ASCII code 9 = tab
        return 1;
    else if (character == 10) // ASCII code 10 = linefeed
        return 1;
    else if (character == 13) // ASCII code 13 = carriage return
        return 1;
    else
        return 0;
}

int isCharacterLetter() {
    if (character >= 97)      // ASCII code 97 = a
        if (character <= 122) // ASCII code 122 = z
            return 1;
        else
            return 0;
    else if (character >= 65) // ASCII code 65 = A
        if (character <= 90)  // ASCII code 90 = Z
            return 1;
        else
            return 0;
    else
        return 0;
}

int identifierOrKeyword() {
    int* identifierCursor;

    identifierCursor = identifier;

    if (*identifierCursor == 119) { // ASCII code 119 = w
        // looking for "while"
        identifierCursor = identifierCursor + 1;

        if (*identifierCursor == 104) { // ASCII code 104 = h
            identifierCursor = identifierCursor + 1;

            //...
        } else
            return IDENTIFIER;
    } else if (*identifierCursor == 105) { // ASCII code 105 = i
        // looking for "if" or "int"
        identifierCursor = identifierCursor + 1;

        if (*identifierCursor == 102) { // ASCII code 102 = f
            // looking for "if"
            identifierCursor = identifierCursor + 1;

            if (*identifierCursor == 0) // end of identifier?
                return IF;
            else
                return IDENTIFIER;
        } else if (*identifierCursor == 110) { // ASCII code 110 = n
            // looking for "int"
            identifierCursor = identifierCursor + 1;

            // ...
        } else
            return IDENTIFIER;
    } else if (*identifierCursor == 101) { // ASCII code 101 = e
        // looking for "else"
        identifierCursor = identifierCursor + 1;

        //...
    } else if (*identifierCursor == 114) { // ASCII code 114 = r
        // looking for "return"
        identifierCursor = identifierCursor + 1;

        //...
    } else
        // no keyword found
        return IDENTIFIER;
}

