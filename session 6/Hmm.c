#include "Hmm.h"

char myheap[heapSize];
char *program_end = myheap;
node *firstFree = (node *)myheap; // ptr to to the first free memory block in the heap


void *HmmAlloc(size_t size){
    if(size ==0) return NULL;

	// adjust the size to the node size
	size_t *sizelocation; // variable to save the size of the block
	size_t total_size = size>sizeof(node) ? size + sizeof(size_t) : sizeof(node);// size of data and metadata
	size = size > sizeof(node) ? size : sizeof(node); // the least size of a node must be equal to size

	// each time no free blocks
	if((char *)firstFree == program_end){
		sizelocation = (size_t *)(program_end);
		program_end += (myceil((double)total_size/blockSize))*blockSize;

		*sizelocation = size;

		// initialize first Free
		firstFree = (node *)((char *)sizelocation + total_size);

		// complete the fields of firstFree if it is a free place (not program end)
		if((char *)firstFree != program_end){
			firstFree->size =  (size_t)((char *)program_end - (char *)firstFree) - sizeof(size_t);
			firstFree->next = firstFree;
			firstFree->prev = firstFree;
		}
		return (void *)(sizelocation + 1);
	}

	//if there is free block
	else{
		
		// initialize a link to loop upon free blocks (which are doubly linked)
		node *dlk = firstFree;
		while(1){
			if(dlk->size == size){
				// if there is only one free block
				if(dlk == firstFree && dlk->next == firstFree){
					firstFree = (node *)program_end;
				}
				else{
					if(dlk==firstFree)
						firstFree = dlk->next;
					deleteNode(dlk);
				}
				
				// return a void ptr to the place after the size and adjust the nodes
				return (void *) ( &(dlk->size) + 1);
			}

			// the free size must be equal to the required size and enough for a new node
			else if(dlk->size > size + sizeof(node)){
				
				node *newaddress = (node *)((char *)((size_t *)dlk + 1) + size); 

				// if there is only one free block
				if(dlk == firstFree && firstFree-> next == firstFree){
					firstFree = newaddress;
					newaddress->next = firstFree;
					newaddress->prev = firstFree;
				}

				else{
					insertafternode(dlk, newaddress);
					deleteNode(dlk);

					//change firstFree
					if(dlk==firstFree){
						firstFree = newaddress;
					}
				}

				newaddress->size = dlk->size - size - sizeof(size_t);
				//return a void ptr to the place after the size and change the size of the allocated block
				dlk->size = size;
				return (void *) ( &(dlk->size) + 1);
			}

			//break when reaching the end of the free list
			if(dlk->next == firstFree)break;
			dlk = dlk->next;
		}

		//if no block have suitable size
		// check if we can add on the last free block
		if((char *)dlk + dlk->size + sizeof(size_t) == program_end){
			size_t added_size = size > dlk->size ? size - dlk->size : 0;
			size_t new_block_size = (myceil((double)added_size/blockSize))*blockSize;

			// if the new size will become more than neded then we will need to add a node
			if(dlk->size + new_block_size > size)added_size += sizeof(node);
			
     			added_size = (myceil((double)added_size/blockSize))*blockSize;
			if((char *)program_end - (char *)myheap + added_size >= 300*1024*1024){
				write(2, "heap size exceeded\n", strlen("heap size exceeded\n"));
                        	return NULL;
			}

			//move the program end and calculate the new size
			program_end +=added_size;
			dlk->size += added_size;	
		}

		// if not then add the program end only
		else{
			size_t added_size = total_size;
			size_t new_block_size = (myceil((double)added_size/blockSize))*blockSize;

			// if the new size will become more than neded then we will need to add a node
			if(new_block_size > total_size)added_size += sizeof(node);

			//move the program end
			node * newNode = (node *)program_end;
			
			added_size = (myceil((double)added_size/blockSize))*blockSize;
			if((char *)program_end - (char *)program_end + added_size >= 300*1024*1024){
                                write(2, "heap size exceeded\n", strlen("heap size exceeded\n"));
                                return NULL;
                        }

			program_end += added_size;

			//initialize the new node
			newNode->size = (size_t)((char *)program_end - (char *)newNode) - sizeof(size_t);
			insertafternode(dlk, newNode);
		}

		if((char *)program_end - (char *)myheap >= 300*1024*1024){
			write(2, "heap size exceeded\n", strlen("heap size exceeded\n"));
			return NULL;
		}
		return HmmAlloc(size);
	}
}


void HmmFree(void *ptr){
    if(ptr==NULL) return;
    
    node *newNode = (node *)((size_t *)ptr - 1);

	//if no free blocks add this one to the free list
	if((char *)firstFree == program_end){
		firstFree = newNode;
		firstFree ->next = firstFree;
		firstFree ->prev = firstFree;
		return;
	}
	node *dlk = firstFree;
	while(1){
		if(dlk==newNode){
			write(2, "can't free the same block more than once\n", strlen("can't free the same block more than once\n"));
			return;
		}
		else if(newNode < firstFree){
			insteratbegin(newNode);
			break;
		}

		//when link(dlk) go past the newNode then we add the newNode before the link
		else if(newNode < dlk){
			insertafternode(dlk->prev, newNode);
			break;
		}

		if(dlk->next == firstFree)break;
		dlk = dlk->next;
	}

	if(newNode > dlk){
		// the specified address will become the new last node
		insertafternode(dlk, newNode);
	}

	// add the newNode to the one next to it if they are contiguous
	if((node *)((char *)newNode + newNode->size + sizeof(size_t)) == newNode->next  && newNode->next != newNode){
		newNode->size += newNode->next->size + sizeof(size_t);
		deleteNode(newNode->next);
	}

	// add the newNode to the one before to it if they are contiguous
	if( (node *)((char *)(newNode->prev) + sizeof(size_t) + newNode->prev->size) == newNode &&  newNode->prev != newNode){
		newNode->prev->size += newNode->size + sizeof(size_t);
		deleteNode(newNode);

		newNode = newNode->prev;// for the next block of code
	}

	//if the newNode is the last node then try to decrease the program_end
	if((char *)newNode + sizeof(size_t) + newNode->size == program_end ){
		// there are two cases
		// case 1: free the whole block
		if((newNode->size + sizeof(size_t))%blockSize == 0){
			deleteNode(newNode);
			program_end -= ((newNode->size + sizeof(node))/blockSize)*blockSize;
			if(newNode == firstFree)firstFree = (node *)program_end;
		}

		// case 2: free the whole block except the data of the node and the size shall be zero
		// 	   to not waste the 20 bytes carrying metadata
		else{
			program_end -= ((newNode->size + sizeof(size_t) - sizeof(node))/blockSize)*blockSize;
			newNode->size -= ((newNode->size + sizeof(size_t) - sizeof(node))/blockSize)*blockSize;
		}
	}

	return;
}
