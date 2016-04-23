//good lord I don't know if this is correct
/*c.c
 *
 * malloc.h implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>
#include "malloc.h"
#include "malloc_r.c"
#include "dataStructure.h" 
#include "mark_and_sweep.h"

#undef malloc
#undef calloc
#undef realloc
#undef free

pthread_t pid = 0; 
void* clean_helper()
{
//    while(_running)
//    {

//        find_stack_bottom();
        mark_on_stack(_metaData);
		mark_on_heap(_metaData);
        //stop the all other threads except this one
        sweep(_metaData);
        //resume the all other threads
//        sleep(5);
//    }
    return NULL;
}

/*
 * should be called by one thread
 */
void gc_init()
{
    size_t i;
    char num[100];
    char again[100];
    register size_t j asm("r11");
    asm("movq %rbp, %r12\n\t"
        "popq %rbp\n\t"
        "movq %rbp, %r11\n\t"
        "movq %r12, %rbp");
    sprintf(num, "%zx", j);
    size_t k = 0;
    for (k = 0; k < strlen(num); k++){
        again[k] = num[k];
    }
    again[strlen(num)] = '\0';
    sscanf(again, "%zx", i);
    set_stack_bottom(i);
    _metaData = DataStructure_init();
    //user calls this at the beginning of the program
    //we need to create a gc_thread to clean data
}

/*
 * should be called by one thread
 * initiate a thread-safe gc
 */
void gc_init_r(){
    _gcMultiThreaded = 1;
    gc_init();
}

/*
 * should be called by one thread
 */
void gc_destroy()
{
     _running = 0;
//     pthread_join(pid, NULL);
//     find_stack_bottom();
     mark_all_on_stack(_metaData);
     sweep(_metaData);
     DataStructure_destroy(_metaData);
    //user calls this at the end of program
    //we need to stop the gc_thread that we have created in the gc_init()
}

void* gc_malloc(size_t size){
    void* userData = malloc(size);
    Node_insert(_metaData, userData, size);
	clean_helper();
    return userData;
}

void* gc_realloc(void* ptr, size_t size){
    if(ptr == NULL)
        return NULL;
    if(size == 0)
    {
        gc_free(ptr);
        return ptr;
    }
    Node* node = DataStructure_findNode(_metaData, ptr);
    size_t oldsize = node->size;
    size_t newsize = oldsize > size ? size : oldsize; 

    Node_remove(_metaData, ptr);

    void* newptr = realloc(ptr, size);
    if(newptr == NULL)
        return NULL;

    Node_insert(_metaData, newptr, newsize);

    return newptr;
}

void gc_free(void* ptr){
	if (ptr == NULL)
		return;
	else {
    	Node* node = (Node*)DataStructure_findNode(_metaData, ptr);
    	node->mark = 0;
	}
    return;
}

void* gc_calloc(size_t nmemb, size_t size){
    void* ptr = calloc(nmemb, size);
    if(ptr != NULL)
        Node_insert(_metaData, ptr, size);
    return ptr;
}
