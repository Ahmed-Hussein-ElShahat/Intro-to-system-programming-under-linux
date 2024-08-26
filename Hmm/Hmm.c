#include "Hmm.h"
#include <math.h>

extern char edata;
char myheap[heapSize];
node *firstFree = NULL; // ptr to to the first free memory block in the heap
void *program_end;
#if 1
/* Enable if you want to replace libc malloc/free */
void * malloc(size_t size)
{
	return HmmAlloc(size);
}

void free(void *ptr)
{
	HmmFree(ptr);
}
#endif

void *HmmAlloc(size_t size){
	
	if(size ==0){
	       	size = sizeof(node);
	}

	// adjust the size to the node size
	size_t *sizelocation; // variable to save the size of the block
	
	// align memory if not alligned
	if(size%8 != 0)
		size = ((size + 7)/8)*8;
	
	size_t total_size = size>sizeof(node) ? size + sizeof(size_t) : sizeof(node) + sizeof(size_t);// size of data and metadata
	size = size > sizeof(node) ? size : sizeof(node); // the least size of a node must be equal to size
	
	
	// each time no free blocks
	if((char *)firstFree == NULL){
		sizelocation = (size_t *)sbrk((myceil((double)total_size/(blockSize)))*(blockSize));
		 
		if(sizelocation==(void *)-1){
			write(2, "couldn't allocate new memory\nmaximum memory reached\n", strlen("couldn't allocate new memory\nmaximum memory reached\n"));
			return NULL;
		}

		*sizelocation = size;

		// initialize first Free
		firstFree = (node *)((char *)sizelocation + total_size);

		void *program_end = sbrk(0);
		// complete the fields of firstFree if it is a free place (not program end)
		if((void *)firstFree != program_end){
			// try to put another node if there is a free segment
			if((char *)program_end - (char *)firstFree > sizeof(node))
				firstFree->size =  (size_t)((char *)program_end - (char *)firstFree) - sizeof(size_t);
			else{
				firstFree=NULL;
				*sizelocation += (char *)program_end - (char *)firstFree;
				return (void *)(sizelocation + 1);
			}
			
			firstFree->next = firstFree;
			firstFree->prev = firstFree;
		}
		else{
			firstFree=NULL;
		}

		return (void *)(sizelocation + 1);
	}

	//if there is free block
	else{

		// initialize a link to loop upon free blocks (which are doubly linked)
		node *dlk = firstFree;
		while(1){
			if(dlk->size == size || (dlk->size > size && dlk->size < size + sizeof(node))){
				deleteNode(dlk);	
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
		if((char *)dlk + dlk->size + sizeof(size_t) == sbrk(0)){
			size_t added_size = size > dlk->size ? size - dlk->size : 0;
			size_t new_block_size = (myceil((double)added_size/(blockSize)))*(blockSize);

			// if the new size will become more than neded then we will need to add a node
			if(dlk->size + new_block_size > size)added_size += sizeof(node);

			added_size = (myceil((double)added_size/(blockSize)))*(blockSize);
			/*if((char *)sbrk(0) - (char *)myheap + added_size >= 300*1024*1024){
			  write(2, "heap size exceeded\n", strlen("heap size exceeded\n"));
			  return NULL;
			  }*/

			//move the program end and calculate the new size
			void *ret = sbrk(added_size);
			if(ret == (void *)-1){
				write(2, "couldn't allocate new memory\nmaximum memory reached\n", strlen("couldn't allocate new memory\nmaximum memory reached\n"));
				return NULL;
			}
			dlk->size += added_size;	
		}

		// if not then add the program end only
		else{
			size_t added_size = total_size;
			size_t new_block_size = (myceil((double)added_size/(blockSize)))*(blockSize);

			// if the new size will become more than neded then we will need to add a node
			if(new_block_size > total_size)added_size += sizeof(node);

			//move the program end
			node * newNode = (node *)sbrk(0);

			added_size = (myceil((double)added_size/(blockSize)))*(blockSize);
			/*if((char *)program_end - (char *)program_end + added_size >= 300*1024*1024){
			  write(2, "heap size exceeded\n", strlen("heap size exceeded\n"));
			  return NULL;
			  }*/

			void *ret = sbrk(added_size);
			if(ret == (void *)-1){
				write(2, "couldn't allocate new memory\nmaximum memory reached\n", strlen("couldn't allocate new memory\nmaximum memory reached\n"));
				return NULL;
			}

			//initialize the new node
			newNode->size = (size_t)((char *)sbrk(0) - (char *)newNode) - sizeof(size_t);
			insertafternode(dlk, newNode);
		}

		/*if((char *)program_end - (char *)myheap >= 300*1024*1024){
		  write(2, "heap size exceeded\n", strlen("heap size exceeded\n"));
		  return NULL;
		  }*/
		return HmmAlloc(size);
	}
}


void HmmFree(void *ptr){
	if(ptr==NULL) return;

	node *newNode = (node *)((size_t *)ptr - 1);

	//if no free blocks add this one to the free list
	if((char *)firstFree == NULL){
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
	if((char *)newNode + sizeof(size_t) + newNode->size == (char *)sbrk(0)){
		// there are two cases
		// case 1: free the whole block
		if((newNode->size + sizeof(size_t))%(blockSize) == 0){
			deleteNode(newNode);
			sbrk(-((newNode->size + sizeof(node))/blockSize)*blockSize);
			if(newNode == firstFree)firstFree = NULL;
		}

		// case 2: free the whole block except the data of the node and the size shall be zero
		// 	   to not waste the 20 bytes carrying metadata
		else{
			sbrk(- ((newNode->size + sizeof(size_t) - sizeof(node))/(blockSize))*(blockSize));
			newNode->size -= ((newNode->size + sizeof(size_t) - sizeof(node))/(blockSize))*(blockSize);
		}
	}

	return;
}

void *calloc(size_t nmemb, size_t size){
	int req_size = nmemb * size;
	if(req_size < 0){
		write(2, "size is too large\n", strlen("size is too large\n"));
		return NULL;
	}
	void *ptr = HmmAlloc(req_size);
	memset(ptr, req_size, 0);
}

void *realloc(void *pointer, size_t size){
	if(pointer == NULL) return HmmAlloc(size);
//	else if(pointer == NULL && size ==0)return HmmAlloc(0);
	else if(size == 0){
		HmmFree(pointer);
		return NULL;
	}

	node *ptr = (node *)((size_t *)pointer-1);
	size_t added_size = size - ptr->size;

	//added_size is 0 or unwanted size is less than the size of a node
	if(added_size == 0 || -added_size < sizeof(node)) return pointer;

	node *ptr_next = (node *)( (char *)(&(ptr->size) + 1) + ptr->size );
	if(ptr_next == sbrk(0) && added_size>0){
		added_size = (myceil((double)added_size/(blockSize)))*(blockSize);
		
		void *ret = sbrk(added_size);
		if(ret == (void *)-1){
			write(2, "couldn't allocate new memory\nmaximum memory reached\n", strlen("couldn't allocate new memory\nmaximum memory reached\n"));
			return NULL;
		}

		if(added_size > size + sizeof(node)){
			node *newNode = (node *)((char *)(&(ptr->size)) + size);
			newNode->size = added_size - (size - ptr->size) - sizeof(size_t);
			ptr->size = size;
			insertafternode(firstFree->prev, newNode);
			
		}
		else{
			ptr->size = added_size;
		}
		return pointer;
	}

	node *dlk = firstFree;
	node *bestfit = firstFree;

	while(1){
		
		if(dlk == ptr_next){
			if(added_size>0){
				if(dlk->size == added_size){
					ptr->size += added_size;
					deleteNode(dlk);
					return pointer;
				}
				else if (dlk->size > added_size && dlk->size <= added_size + sizeof(node)){
                           ptr->size += added_size + sizeof(size_t);
                           deleteNode(dlk);
                           return pointer;
                       }	
				else if(dlk->size > added_size + sizeof(node)){
					ptr->size += added_size;
					node * newNode = (node *)((char *)dlk + added_size + sizeof(size_t));
					newNode->size = dlk->size - added_size;
					insertafternode(dlk, newNode);
					deleteNode(dlk);
					return pointer;
				}
			}
			else{
				node *newNode = (node *)((char *)(&(ptr->size) + 1) + ptr->size  + added_size);
                                ptr->size += added_size;
                                newNode->size = dlk->size -added_size;
                                insertafternode(dlk->prev, newNode);
                                deleteNode(dlk);
                                return pointer;
			}
			break;
		}

		if(dlk->next == firstFree || dlk > ptr_next)break;
		dlk = dlk->next;
	}

	free(pointer);
	void *new_ptr = (size_t *)HmmAlloc(size);
	new_ptr = memcpy(new_ptr, &(ptr->size) + 1, ptr->size);

	//void * ret_ptr =  (void *) (&((size_t *)new_ptr) - 1);
	return (void *)((size_t *)new_ptr - 1);
}
