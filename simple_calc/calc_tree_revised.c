#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 100
#define MAXWORD 100
#define UNINITIALISED 0

enum {NUMBER, TASK_END, EXIT};

typedef struct tnode {
    char operator;
    double operand;
    struct tnode *top;
    struct tnode *left;
    struct tnode *right;
} node;

int getword(char *, int);
char getch_local(void);
void ungetch_local(char);
void ungets_local(char s[]);

node *add_node(node *p, char *w);
node *add_node_above(node *p, char *w);
node *add_node_below(node *p, char *w);
node *talloc(void);
node *parse_parensis(void);
node *parse_expression(void);
double calculate(node *part);

node *g_current_node = NULL;
node *g_prev_current_node = NULL;
int g_prev_priority = UNINITIALISED;

/* externals for getch_local and ungetch_local functions */
char buf[BUFSIZE];
int bp = 0;
char buf_char;

char getch_local(void) {
    char c;
    if (bp > 0) {
        c = buf[--bp];
    } else {
        c = getchar();
    }
    return c;
}

void ungetch_local(char c) {
    if (bp > BUFSIZE) {
        printf("error: ungetch_local - buffer is full\n");
    } else {
        buf[bp++] = c;
    }
}

void ungets_local(char s[]) {
    int s_length;
    int i;
    s_length = strlen(s);
    for (i = s_length; i > 0; i--) {
        ungetch_local(s[i - 1]);
    }
}

int getword(char *word, int lim) {
    char c;
    char *w = word;

    while ((c = getch_local()) == '\t' || c == ' ') {
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
            if (!isnumber(*w = getch_local())) {
                ungetch_local(*w);
                break;
            }
            w++;
        }
    } else {
        *w++ = c;
    }
    *w = '\0';
    return word[0];
}

int is_operation(int o) {
    if ((o == '+') || (o == '-') ||
        (o == '*') || (o == '/')) {
        return 1;
    } else {
        return 0;
    }
}

int is_expression(int p) {
    if ((isnumber(p)) || is_operation(p)) {
        return 1;
    } else {
        return 0;
    }
}

/* add_node empty */
node *add_node(node *p, char *w) {
    if (p == NULL) {
        if (is_operation(w[0])) {
            p = talloc();
            p->operand = 0;
            p->operator = w[0];
            p->left = p->right = p->top = NULL;
        } else if (isnumber(w[0])) {
            p = talloc();
            p->operator = 0;
            p->operand = atof(w);
            p->left = p->right = p->top = NULL;
        }
    }
    return p;
}


/* add node above */
node *add_node_above(node *p, char *w) {
    node *local_node;
    local_node = add_node(NULL, w);
    local_node->left = p;
    local_node->top = p->top;
    if (p->top) {
        p->top->right = local_node;
    }
    return local_node;
}

/* add node below */
node *add_node_below(node *p, char *w) {
    node *local_node;
    local_node = add_node(NULL, w);
    local_node->left = p->right;
    local_node->top = p;
    p->right = local_node;
    return local_node;
}

/* talloc: make a tnode */
node *talloc(void) {
    return (node *) malloc(sizeof(node));
}

void tfree(node *part) {
    if (part->left) {
        tfree(part->left);
    } else {
        free(part);
        part = NULL;
    }
    if (part) {
        if (part->right) {
            tfree(part->right);
        } else {
            free(part);
            part = NULL;
        }
    }
    if (part) {
        free(part);
        part = NULL;
    }
}

double calculate(node *part) {
    double op1, op2;
    double result;
    if (part->left) {
        op1 = calculate(part->left);
    } else {
        return part->operand;
    }
    if (part->right) {
        op2 = calculate(part->right);
    } else {
        return part->operand;
    }
    switch (part->operator) {
        case '+':
            result = op1 + op2;
            break;
        case '-':
            result = op1 - op2;
            break;
        case '*':
            result = op1 * op2;
            break;
        case '/':
            result = op1 / op2;
            break;
    }
    return result;
}

node *parse_expression(void) {
    int word_type;
    char current_word[MAXWORD];
    node l_node;
    int priority = 0;

    while ((word_type = getword(current_word, MAXWORD)) != EXIT) {
        if (word_type == '(') {
            parse_parensis();
        } else if (is_expression(word_type)) {
            // write number because of first part of expression
            // which always a number
            if (!g_current_node) {
                g_current_node = add_node(NULL, current_word);
            } else if (is_operation(word_type)) {
                // working with central node and left thread
                if (word_type == '+' || word_type == '-') {
                    priority = 1;
                    if (g_prev_priority == UNINITIALISED) {
                        g_prev_priority = 1;
                    }
                } else if (word_type == '*' || word_type == '/') {
                    priority = 2;
                    if (g_prev_priority == UNINITIALISED) {
                        g_prev_priority = 2;
                    }
                }
                if (g_prev_priority == priority) {
                    g_current_node = add_node_above(
                                        g_current_node,
                                        current_word
                                     );
                    g_prev_priority = priority;
                } else if (g_prev_priority < priority) {
                    g_prev_current_node = g_current_node;
                    g_current_node = add_node_below(
                                        g_current_node,
                                        current_word
                                     );
                    g_prev_priority = priority;
                } else if (g_prev_priority > priority) {
                    g_current_node = add_node_above(
                                        g_prev_current_node,
                                        current_word
                                     );
                    g_prev_priority = priority;
                    g_prev_current_node = NULL;
                }
            } else if (isnumber(current_word[0])) {
                // working with right thread
                g_current_node->right = add_node(NULL, current_word);
                g_current_node->right->top = g_current_node;
            }
 

        } else if (word_type == TASK_END) {
            printf("-----------\n");
            printf("calculation\n");
            if (g_prev_current_node != NULL) {
                g_current_node = g_prev_current_node;
                g_prev_current_node = NULL;
            }
            printf("result: %f\n", calculate(g_current_node));
            tfree(g_current_node);
            g_current_node = NULL;
            g_prev_priority = UNINITIALISED;
        } else {
            printf("error: %s is wrong expression\n", current_word);
        }
    }
    return NULL;
}

node *parse_parensis(void) {
    int word_type;
    char current_word[MAXWORD];

    word_type = getword(current_word, MAXWORD);
    if (word_type == '(') {
        printf("parse_parensis()\n");
        parse_parensis();
    } else {
        // write number
        printf("parse_expression()\n");
        parse_expression();
    }

    return NULL;
}

int main(int argc, const char *argv[]) {
    parse_expression();
    return 0;
}
