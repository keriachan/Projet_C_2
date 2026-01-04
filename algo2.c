#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

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

// Algo 2

typedef struct{
    int taille;
    char *mot;
} Mot;

typedef struct mot {
    Mot m;
    int occurrences;
    struct mot *suite;
} CelluleMot, *Texte;

CelluleMot* allouerCellule(Mot _mot, InfoMem * info){
    CelluleMot * cm = myMalloc(sizeof(CelluleMot), info);
    if (!cm) return NULL;
    cm->m = _mot;
    cm->occurrences = 1;
    cm->suite = NULL;
    return cm;
}

int insererEnTete(Texte *texte, Mot _mot, InfoMem * info){
    CelluleMot * cm = allouerCellule(_mot, info);
    if (!cm) return 0;
    cm->suite = *texte;
    *texte = cm;
    return 1;
}

Mot initMot(FILE * f, InfoMem * info){
    int c = fgetc(f);
    Mot _mot;
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

void compterOuInserer(Texte *texte, Mot _mot, InfoMem * info) {
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
        Mot _mot = initMot(f, info);
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

void algo2(FILE * f, InfoMem * info){
    Texte texte = initTexte(f, info);
    printTexte(texte);
    libererTexte(texte, info);
}

int main(void){
    SetConsoleOutputCP(65001);
    FILE * f_log = fopen("./logs/mem_log_algo2.csv", "w");
    FILE * f_stats = fopen("./logs/mem_stats_algo2.csv", "w");
    InfoMem  info = {0, 0, 0, 0, f_log};
    FILE * f = fopen("./texts/text1.txt", "r");
    fprintf(f_log, "operation,memory\n");
    fprintf(f_log, "0,0\n"); // Point de départ
    clock_t debut = clock();
    algo2(f, &info);
    clock_t fin = clock();
    double temps_ecoule = (double)(fin - debut) / CLOCKS_PER_SEC;
    printf("\ntemps : %f sec\n", temps_ecoule);
    fprintf(f_stats, "metric,value\n");
    fprintf(f_stats, "max_alloc,%zu\n", info.max_alloc);
    fprintf(f_stats, "total_alloc,%zu\n", info.cumul_alloc);
    fprintf(f_stats, "total_free,%zu\n", info.cumul_desalloc);
    fprintf(f_stats, "total_time,%f\n", temps_ecoule);
    fclose(f_log);
    fclose(f_stats);
    fclose(f);
    return 0;
}
