#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t cumul_alloc; // champ obligatoire : cumul de l’espace mémoire alloué
    size_t cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire désalloué
} InfoMem;

void* myMalloc(size_t size, InfoMem* infoMem){
    void* ptr = malloc(size);
    if (ptr != NULL){
        infoMem->cumul_alloc+=size;
    }
    return ptr;
}

void* myRealloc(void* ptr, size_t new_size, InfoMem* infoMem, size_t old_size){
    void* ptr2 = realloc(ptr, new_size);
    if (ptr2 != NULL){
        if (new_size > old_size)
            infoMem->cumul_alloc+=new_size - old_size;
        else if (new_size < old_size)
            infoMem->cumul_desalloc+=old_size - new_size;
    }
    return ptr2;
}

void myFree(void* ptr, InfoMem* infoMem, size_t old_size){
    if (ptr != NULL){
        infoMem->cumul_desalloc+=old_size;
        free(ptr);
    }
}

int main(void){
    InfoMem Memoire;
    Memoire.cumul_alloc = 0;
    Memoire.cumul_desalloc = 0;
    return 0;
}