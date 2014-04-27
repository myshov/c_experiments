#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 100
#define MAXWORD 100

enum {TASK_END, EXIT};

typedef struct tnode {
    char operator;
    int operand;
    struct tnode *top;
    struct tnode *left;
    struct tnode *right;
} node;

int getword(char *, int);
char getch(void);
void ungetch(char);
//struct tnode *addtree(struct tnode *, char *);
//struct tnode *talloc(void);

/* externals for getch and ungetch functions */
char buf[BUFSIZE];
int bp = 0;
char buf_char;

char getch(void) {
    char c;
    if (bp > 0) {
        c = buf[--bp];
    } else {
        c = getchar();
    }
    return c;
}

void ungetch(char c) {
    if (bp > BUFSIZE) {
        printf("error: ungetch - buffer is full\n");
    } else {
        buf[bp++] = c;
    }
}

int getword(char *word, int lim) {
    char c;
    char *w = word;

    while ((c = getch()) == '\t' || c == ' ') {
        ;
    }

    if (c == EOF) {
        return EXIT;
    } 

    if (c == '\n') {
        return TASK_END;
    } 
    
    if (c == '+' || c == '-' || c == '*' || c == '/') {
        *w++ = c;
        *w = '\0';
        return c;
    } else if (c == '(') {
        *w++ = c;
        *w = '\0';
        return c;
    } else if (c == ')') {
        *w++ = c;
        *w = '\0';
        return c;
    } else if (isnumber(c)) {
        *w++ = c;
        while (--lim > 0) {
            if (!isnumber(*w = getch())) {
                ungetch(*w);
                break;
            }
            w++;
        }
    }
    *w = '\0';
    return word[0];
}

node *parse_expression() {
    /* code */
    return NULL;
}

node *parse_parensis() {
    /* code */
    return NULL;
}

int main(int argc, const char *argv[]) {
    printf("%d\n", TASK_END);
    printf("%d\n", EXIT);
    int word_type;
    char current_word[MAXWORD];

    while ((word_type = getword(current_word, MAXWORD)) != EXIT) {
        if (word_type == TASK_END) {
            printf("\n");
        } else {
            printf("%s ", current_word);
        }
    }
    return 0;
}
