#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    size_t cumul_alloc; // champ obligatoire : cumul de l’espace mémoire alloué
    size_t cumul_desalloc; // champ obligatoire : cumul de l’espace mémoire désalloué
    size_t max_alloc; // pic d'allocation mémoire utilisée durant l’exécution
} InfoMem;

typedef struct {
    char * mot; 
    int occurrences;
} Element;

typedef struct {
    Element * elem;
    int taille;
    int capacite;
} Dict;

typedef struct{
    char * mot;
    int taille;
    int capacite;
} Mot;

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

// ALGO 1

int comparer_occurrences(const void *a, const void *b) {
    const Element *elemA = (const Element *)a;
    const Element *elemB = (const Element *)b;
    return (elemB->occurrences - elemA->occurrences);
}

void ajouter_au_dict(Dict *d, const char *mot_lu, InfoMem *info) {
    for (int i = 0; i < d->taille; i++) {
        if (strcmp(d->elem[i].mot, mot_lu) == 0) {
            d->elem[i].occurrences++;
            return;
        }
    }
    if (d->taille >= d->capacite) {
        int ancienne_cap = d->capacite;
        if (d->capacite == 0)
            d->capacite = 10;
        else
            d->capacite = d->capacite * 2;
        d->elem = (Element*)myRealloc(d->elem, d->capacite * sizeof(Element), info, ancienne_cap * sizeof(Element));
    }
    d->elem[d->taille].mot = strdup(mot_lu);
    d->elem[d->taille].occurrences = 1;
    d->taille++;
}

void algo1(FILE *fichier, InfoMem * info, int n) {
    Dict monDict = {NULL, 0, 0};
    Mot courant = {NULL, 0, 0};
    int c;
    while ((c = fgetc(fichier)) != EOF) {
        if (c==' ' || c=='\n' || c=='\t') {
            if (courant.taille > 0) {
                courant.mot[courant.taille] = '\0';
                ajouter_au_dict(&monDict, courant.mot, info);
                courant.taille = 0;
            }
        } else {
            if (courant.taille + 1 >= courant.capacite) {
                int ancienne_cap = courant.capacite;
                if (courant.capacite == 0)
                    courant.capacite = 10;
                else
                    courant.capacite = courant.capacite * 2;
                courant.mot = (char*)myRealloc(courant.mot, courant.capacite * sizeof(char), info, ancienne_cap * sizeof(char));
            }
            courant.mot[courant.taille++] = (char)c;
        }
    }
    if (courant.taille > 0) {
        courant.mot[courant.taille] = '\0';
        ajouter_au_dict(&monDict, courant.mot, info);
    }
    qsort(monDict.elem, monDict.taille, sizeof(Element), comparer_occurrences);
    int max = (n>0 && n<monDict.taille) ? n : monDict.taille;
    for (int i = 0; i < max; i++) {
        printf("%s : %d\n", monDict.elem[i].mot, monDict.elem[i].occurrences);
        myFree(monDict.elem[i].mot, info, strlen(monDict.elem[i].mot) + 1);
    }
    myFree(monDict.elem, info, monDict.capacite * sizeof(Element));
    myFree(courant.mot, info, courant.capacite * sizeof(char));
}

int main(int argc, char *argv[]){
    int n=-1, a1=0, a2=0;
    for (int i=1; i<argc; i++){
        if (strcmp(argv[i], "-help") == 0){
            printf("Le programme pourra, par exemple, etre lance par la commande :\n./projet [-n int] [-a algo1|algo2|algo3|...] [-help] [-s fichierdesortie] [-l fichierdeperf] fichiers de donnees (plusieurs!)\n");
        } else if (strcmp(argv[i], "-n") == 0){
            int resultat = 0;
            for (int j = 0; argv[i+1][j]; j++) {
                resultat = (resultat * 10) + (argv[i+1][j] - '0');
            }
            n = resultat;
            i++;
        } else if (strcmp(argv[i], "-a") == 0){
            char *copie = strdup(argv[i+1]);
            char *token = strtok(copie, "|");
            for (; token; token = strtok(NULL, "|")) {
                if (strcmp(token, "algo1") == 0) 
                    a1 = 1;
                else if (strcmp(token, "algo2") == 0) 
                    a2 = 1;
            }
            free(copie);
            i++;
        }
    }
    for (int i=1; i<argc; i++){
        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "-a") == 0) {
            i++;
            continue;
        }
        if (strcmp(argv[i], "-help") == 0) {
            continue;
        }
        FILE *fichier = fopen(argv[i], "r");
        if(fichier != 0){
            InfoMem info = {0, 0, 0};
            if (a1){
                InfoMem info = {0, 0, 0};
                algo1(fichier, &info, n);
                printf("Algorithme 1 :\n");
                printf("Cumul Alloc: %zu\n", info.cumul_alloc);
                printf("Cumul Desalloc: %zu\n", info.cumul_desalloc);
                printf("Pic (Max Alloc): %zu\n", info.max_alloc);
            }
            if (a2)
                printf("Pas encore de deuxieme algo.\n");
        } else {
            printf("Le fichier %s ne peu pas etre lu.\n", argv[i]);
        }
        fclose(fichier);
    }
    return 0;
}