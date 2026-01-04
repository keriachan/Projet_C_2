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
    int op_count; // nombre d'opérations (myMalloc, myRealloc, myFree)
    FILE * f_log; // fichier de logs
} InfoMem;

void logMem(InfoMem * info) {
    if (info->f_log != NULL) {
        size_t current = info->cumul_alloc - info->cumul_desalloc;
        info->op_count++;
        fprintf(info->f_log, "%d,%zu\n", info->op_count, current);
    }
}

void* myMalloc(size_t size, InfoMem* infoMem){
    void* ptr = (void*)malloc(size);
    if (ptr != NULL){
        infoMem->cumul_alloc+=size;
        if (infoMem->cumul_alloc - infoMem->cumul_desalloc>infoMem->max_alloc){
            infoMem->max_alloc = infoMem->cumul_alloc - infoMem->cumul_desalloc;
        }
        logMem(infoMem);
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
        logMem(infoMem);
    }
    return ptr2;
}

void myFree(void* ptr, InfoMem* infoMem, size_t old_size){
    if (ptr != NULL){
        infoMem->cumul_desalloc+=old_size;
        free(ptr);
        logMem(infoMem);
    }
}

// ALGO 1

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

void algo1(FILE *fichier, InfoMem * info, int n, char *sortie) {
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
    FILE *fsortie = NULL;
    if (sortie)
        fsortie = fopen(sortie, "a");
    for (int i = 0; i < monDict.taille; i++) {
        if (i < max)
            printf("%s (%d)\n", monDict.elem[i].mot, monDict.elem[i].occurrences);
        if (fsortie)
            fprintf(fsortie, "%s %d\n", monDict.elem[i].mot, monDict.elem[i].occurrences);  
        myFree(monDict.elem[i].mot, info, strlen(monDict.elem[i].mot) + 1);
    }
    if (fsortie)
        fclose(fsortie);
    myFree(monDict.elem, info, monDict.capacite * sizeof(Element));
    myFree(courant.mot, info, courant.capacite * sizeof(char));
}

// ALGO 2

typedef struct{
    int taille;
    char *mot;
} Mot_;

typedef struct mot {
    Mot_ m;
    int occurrences;
    struct mot *suite;
} CelluleMot, *Texte;

int comparer_cellules(const void *a, const void *b) {
    CelluleMot *elemA = *(CelluleMot **)a;
    CelluleMot *elemB = *(CelluleMot **)b;
    return (elemB->occurrences - elemA->occurrences);
}

CelluleMot* allouerCellule(Mot_ _mot, InfoMem * info){
    CelluleMot * cm = myMalloc(sizeof(CelluleMot), info);
    if (!cm) return NULL;
    cm->m = _mot;
    cm->occurrences = 1;
    cm->suite = NULL;
    return cm;
}

int insererEnTete(Texte *texte, Mot_ _mot, InfoMem * info){
    CelluleMot * cm = allouerCellule(_mot, info);
    if (!cm) return 0;
    cm->suite = *texte;
    *texte = cm;
    return 1;
}

Mot_ initMot(FILE * f, InfoMem * info){
    int c = fgetc(f);
    Mot_ _mot;
    int c_count = 0;
    int max_mot = 100;
    _mot.mot = (char*)myMalloc(sizeof(char)*max_mot, info);
    if (_mot.mot == NULL) {
        _mot.taille = 0;
        return _mot; 
    }
    while ((c != EOF) && (isspace(c) || ispunct(c))){
        c = fgetc(f);
    }
    if (c == EOF) {
        _mot.mot[0] = '\0';
        _mot.taille = 0;
        return _mot;
    }
    while ((c != EOF) && !isspace(c) && !ispunct(c) && c_count < max_mot - 1){
        _mot.mot[c_count] = c;
        c_count++;
        c = fgetc(f);
    }
    _mot.mot[c_count] = '\0';
    _mot.taille = c_count;
    char * mot_ajuste = (char*)myRealloc(_mot.mot, sizeof(char)*(c_count + 1), info, sizeof(char)*max_mot); // +1 pour l'espace
    if (mot_ajuste != NULL) {
        _mot.mot = mot_ajuste;
    }
    return _mot;
};

void compterOuInserer(Texte *texte, Mot_ _mot, InfoMem * info) {
    CelluleMot *cm = *texte;
    while (cm != NULL) {
        if (strcmp(cm->m.mot, _mot.mot) == 0) {
            cm->occurrences++; 
            return;
        }
        cm = cm->suite;
    }
    insererEnTete(texte, _mot, info);
}

Texte initTexte(FILE * f, InfoMem * info){
    Texte _texte = NULL;
    while (1){
        Mot_ _mot = initMot(f, info);
        if (_mot.taille == 0) break;
        compterOuInserer(&_texte, _mot, info);
    }
    return _texte;
};

void printTexte(Texte _texte) {
    while (_texte) {
        printf("%s(%d) ", _texte->m.mot, _texte->occurrences);
        _texte = _texte->suite;
    }
}

void libererTexte(Texte _texte, InfoMem * info){
    CelluleMot * cm = _texte;
    while (cm != NULL){
        CelluleMot * scm = cm->suite;
        myFree(cm->m.mot, info, sizeof(char)*(cm->m.taille + 1)); // +1 pour l'espace
        myFree(cm, info, sizeof(CelluleMot));
        cm = scm;
    }
}

void algo2(FILE * f, InfoMem * info, int n, char *sortie){
    Texte texte = initTexte(f, info);
    int count = 0;
    CelluleMot *curr = texte;
    while (curr) {
        count++;
        curr = curr->suite;
    }
    if (count > 0) {
        CelluleMot **array = (CelluleMot**)myMalloc(count * sizeof(CelluleMot*), info);
        curr = texte;
        int i = 0;
        while (curr) {
            array[i++] = curr;
            curr = curr->suite;
        }
        qsort(array, count, sizeof(CelluleMot*), comparer_cellules);
        int max = (n > 0 && n < count) ? n : count;
        FILE *fsortie = NULL;
        if (sortie) fsortie = fopen(sortie, "w");
        for (int j = 0; j < count; j++) {
            if (j < max) {
                printf("%s (%d)\n", array[j]->m.mot, array[j]->occurrences);
            }
            if (fsortie) {
                fprintf(fsortie, "%s %d\n", array[j]->m.mot, array[j]->occurrences);
            }
        }
        if (fsortie) fclose(fsortie);
        myFree(array, info, count * sizeof(CelluleMot*));
    }
    libererTexte(texte, info);
}

// ALGO 3



// MAIN

int main(int argc, char *argv[]){
    int n=-1, a1=0, a2=0, a3=0;
    char *sortie = NULL;
    char *perf = NULL;
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
                else if (strcmp(token, "algo2") == 0) 
                    a3 = 1;
            }
            free(copie);
            i++;
        } else if (strcmp(argv[i], "-s") == 0){
            sortie = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "-l") == 0){
           perf = argv[i+1];
           i++;
        }
    }
    for (int i=1; i<argc; i++){
        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "-s") == 0) {
            i++;
            continue;
        }
        if (strcmp(argv[i], "-help") == 0) {
            continue;
        }
        FILE *fichier = fopen(argv[i], "r");
        if(fichier != 0){
            if (a1){
                FILE * f_log = fopen("./logs/mem_log_algo1.csv", "w");
                fprintf(f_log, "operation,memory\n");
                fprintf(f_log, "0,0\n");
                InfoMem info = {0, 0, 0, 0, f_log};
                printf("Algorithme 1 sur le fichier %s\n", argv[i]);
                algo1(fichier, &info, n, sortie);
                printf("Cumul Alloc: %zu\n", info.cumul_alloc);
                printf("Cumul Desalloc: %zu\n", info.cumul_desalloc);
                printf("Pic (Max Alloc): %zu\n", info.max_alloc);
                if (perf){
                    FILE *fperf = fopen(perf, "a");
                    if (fperf)
                        fprintf(fperf, "Fichier %s\nAlgo algo1\ncumul_alloc %zu\ncumul_desalloc %zu\nmax_alloc %zu\n",argv[i], info.cumul_alloc, info.cumul_desalloc, info.max_alloc);
                    fclose(fperf);
                }
                rewind(fichier);
                fclose(f_log);
            }
            if (a2){
                FILE * f_log = fopen("./logs/mem_log_algo1.csv", "w");
                fprintf(f_log, "operation,memory\n");
                fprintf(f_log, "0,0\n");
                InfoMem info = {0, 0, 0, 0, f_log};
                printf("Algorithme 2 sur le fichier %s\n", argv[i]);
                algo2(fichier, &info, n, sortie);
                printf("Cumul Alloc: %zu\n", info.cumul_alloc);
                printf("Cumul Desalloc: %zu\n", info.cumul_desalloc);
                printf("Pic (Max Alloc): %zu\n", info.max_alloc);
                if (perf){
                    FILE *fperf = fopen(perf, "a");
                    if (fperf)
                        fprintf(fperf, "Fichier %s\nAlgo algo1\ncumul_alloc %zu\ncumul_desalloc %zu\nmax_alloc %zu\n",argv[i], info.cumul_alloc, info.cumul_desalloc, info.max_alloc);
                    fclose(fperf);
                }
                rewind(fichier);
                fclose(f_log);
            }
            if (a3)
                printf("Pas encore de deuxieme algo.\n");
        } else {
            printf("Le fichier %s ne peut pas etre lu.\n", argv[i]);
        }
        fclose(fichier);
    }
    return 0;
}