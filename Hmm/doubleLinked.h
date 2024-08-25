#include <inttypes.h>
#include <stddef.h>

typedef struct node node;

struct node{
        size_t size;
        node *next;
        node *prev;
};

void insteratbegin(node *newNode);
void insertafternode(node *mynode, node *newNode);
void deleteNode(node *mynode);
