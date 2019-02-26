

/**
 * Malloc Lab
 * CS 241 - Spring 2019
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//double linked list
typedef struct dll {
    struct dll* next;
    struct dll* prev;
    size_t max;
    size_t lsize;
} dll;

static dll* head = NULL;
static dll* tail = NULL;


/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    // implement calloc!
    size_t sumsize = (num * size);
    void* out = malloc(sumsize);

    if (out) {
	memset(out, 0, sumsize);
    }
    else {
	return NULL;
    }
    
    return out;

//    return NULL;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
	
// implement malloc!
    dll* metadata = NULL;
    //start of memory block
    if (head == NULL) {
	metadata = sbrk(sizeof(dll)+size);
	metadata->next = NULL;
	metadata->prev = NULL;
	metadata->max = size;
	metadata->lsize = size;

	head = metadata;
	tail = metadata;
    }
    //allocate after tail
    else if (size <= 64) {
	metadata = sbrk(sizeof(dll)+size);
	metadata->prev = tail;
	metadata->max = size;
	metadata->lsize = size;

	tail->next = metadata;
	tail = tail->next;
	tail->next = NULL;
    }
    else {
	dll* temp = tail;
	//find correct mem block location
        for(; temp!= NULL; temp = temp->prev) {
	    size_t sizelim = size + temp->lsize + sizeof(dll);
	    if ((temp->max >= sizelim) || (temp->max >= size && temp->lsize == 0)) {
		break;
	    }
	}
	if (temp != NULL) {
	    //free block is found and is larger than space needed, so create allocated & remain entries
	    if (temp->lsize != 0) {
		//createtwo
		dll* otherb = (dll*) ((void*)temp + sizeof(dll) + temp->lsize);
		otherb->lsize = size;
		otherb->max = temp->max - temp->lsize - sizeof(dll);

		temp->max = temp->lsize;

		otherb->next = temp->next;
		otherb->prev = temp;

		temp->next = otherb;

		if (otherb->next == NULL) {
		    tail = otherb;
		}
		else {
		    otherb->next->prev = otherb;
		}
		
		metadata = temp->next;
	    }
	    else {
		metadata = temp;
		temp->lsize = size;
	    }
	}
	else {
	    metadata = sbrk(sizeof(dll)+size);
	    metadata->prev = tail;
	    metadata->max = size;
	    metadata->lsize = size;

	    tail->next = metadata;
	    tail = tail->next;
	    tail->next = NULL;
	}
    }
    void* out = (void*) (metadata + 1);
    return out;  
 // return NULL;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
//helper function to join to mem blocks
void helper(dll* t1, dll* t2) {
    t1->next = t2->next;
    t1->max = t1->max + t2->max + sizeof(dll);

    if (t1->next == NULL) {
	tail = t1;
    }
    else {
	t1->next->prev = t1;
    }
}

void free(void *ptr) {
    // implement free!
    dll* metadata = (dll*)ptr - 1;
    metadata->lsize = 0;

    if (metadata == head) {
	if (metadata->next != NULL && metadata->next->lsize == 0) {
	    helper(metadata, metadata->next);
	}
    }
    else if (metadata == tail) {
	if (metadata->prev->lsize == 0) {
	    helper(metadata->prev, metadata);
	}
    }
    else {
	if (metadata->next->lsize == 0) {
	    helper(metadata, metadata->next);
	}
	if (metadata->prev->lsize == 0) {
	    helper(metadata->prev, metadata);
	}
    }
}


/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    // implement realloc!
	if (ptr == NULL) {
	void* out = malloc(size);
	return out;
    }

    if (size == 0 && ptr != NULL) {
	free(ptr);
	return NULL;
    }

    dll* metadata = (dll*)ptr - 1;

    if (metadata->lsize >= size) {
	metadata->lsize = size;
	return ptr;
    }
    else {
	void* out = malloc(size);
	if (out == NULL) {
	    return NULL;
	}
	else {
	    memcpy(out, ptr, metadata->lsize);
	    free(ptr);
	    return out;
	}
    } 

 //  return NULL;
}


