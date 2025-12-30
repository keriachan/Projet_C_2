#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

// InfoMem

typedef struct {
    size_t cumul_alloc; // champ obligatoire : cumul de l’espace mémoire alloué
    size_t cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire désalloué
    size_t max_alloc; // pic d'allocation mémoire utilisée durant l’exécution
} InfoMem;

void* myMalloc(size_t size, InfoMem* infoMem){
    void* ptr = (void*)malloc(size);
    if (ptr != NULL){
        infoMem->cumul_alloc+=size;
        if (infoMem->cumul_alloc - infoMem->cumul_desalloc>infoMem->max_alloc){
            infoMem->max_alloc = infoMem->cumul_alloc - infoMem->cumul_desalloc;
        }
    }
    return ptr;
}

void* myRealloc(void* ptr, size_t new_size, InfoMem* infoMem, size_t old_size) {
    void* ptr2 = realloc(ptr, new_size);
    if (ptr2 != NULL) {
        if (ptr2 != ptr && ptr != NULL) {
            infoMem->cumul_alloc += new_size;
            infoMem->cumul_desalloc += old_size;
        }
        else {
            if (new_size > old_size) {
                infoMem->cumul_alloc += new_size - old_size;
            } else if (new_size < old_size) {
                infoMem->cumul_desalloc += old_size - new_size;
            }
        }
        if (infoMem->cumul_alloc - infoMem->cumul_desalloc > infoMem->max_alloc) {
            infoMem->max_alloc = infoMem->cumul_alloc - infoMem->cumul_desalloc;
        }
    }
    return ptr2;
}

void myFree(void* ptr, InfoMem* infoMem, size_t old_size){
    if (ptr != NULL){
        infoMem->cumul_desalloc+=old_size;
        free(ptr);
    }
}

// Algo 2

#define MAX_MOT 100

typedef struct{
    int taille;
    char mot[MAX_MOT];
} Mot;

typedef struct mot {
    Mot m;
    struct mot *suite;
} CelluleMot, *Texte;

CelluleMot* allouerCellule(Mot _mot, InfoMem * info){
    CelluleMot * cm = myMalloc(sizeof(CelluleMot), info);
    if (!cm) return NULL;
    cm->m = _mot;
    cm->suite = NULL;
    return cm;
}

int insererEnTete(Texte *texte, Mot _mot, InfoMem * info){
    CelluleMot * cm = myMalloc(sizeof(CelluleMot), info);
    if (!cm) return 0;
    cm->m = _mot;
    cm->suite = *texte;
    *texte = cm;
    return 1;
}

Mot initMot(FILE * f){
    int c = fgetc(f);
    Mot _mot;
    int c_count = 0;
    while ((c != EOF) && (isspace(c) || ispunct(c))){
        c = fgetc(f);
    }
    if (c == EOF) {
        _mot.mot[0] = '\0';
        _mot.taille = 0;
        return _mot;
    }
    while ((c != EOF) && !isspace(c) && !ispunct(c) && c_count < MAX_MOT - 1){
        _mot.mot[c_count] = c;
        c_count++;
        c = fgetc(f);
    }
    _mot.mot[c_count] = '\0';
    _mot.taille = c_count;
    return _mot;
};

Texte initTexte(FILE * f, InfoMem * info){
    Texte _texte = NULL;
    while (1){
        Mot _mot = initMot(f);
        if (_mot.taille == 0) {
            break;
        }
        if (!insererEnTete(&_texte, _mot, info)) {
            break; 
        }
    }
    return _texte;
};

void printTexte(Texte _texte) {
    while (_texte) {
        printf("%s ", _texte->m.mot);
        _texte = _texte->suite;
    }
}

int main(void){
    InfoMem info = {0, 0, 0};
    FILE * f = fopen("./texts/text1.txt", "r");
    Texte texte = initTexte(f, &info);
    printTexte(texte);
    fclose(f);
}
