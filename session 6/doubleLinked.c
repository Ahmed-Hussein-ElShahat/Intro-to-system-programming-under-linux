#include "doubleLinked.h"

extern node *firstFree;
node **head = &firstFree;

void insteratbegin(node *newNode){
        //initialize the new node fields
	newNode->next = *head;
        newNode->prev = (*head)->prev;

	//change the fields of prev and next nodes
        (*head)->prev->next = newNode;
        (*head)->prev = newNode;

        *head = newNode;
}

void insertafternode(node *mynode, node *newNode){
	//initialize the new node fields
        newNode->next = mynode->next;
        newNode->prev = mynode;
        
	//change the fields of prev and next nodes
        mynode->next->prev = newNode;
        mynode->next = newNode;
}

void deleteNode(node *mynode){
	mynode->prev->next = mynode->next;
	mynode->next->prev = mynode->prev;
}

