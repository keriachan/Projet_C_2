#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_MOT 100

typedef struct{
    int taille;
    char mot[MAX_MOT];
} Mot;

typedef struct mot {
    Mot m;
    struct mot *suite
} CelluleMot, *Texte;

CelluleMot* allouerCellule(Mot _mot){
    CelluleMot * cm = malloc(sizeof(CelluleMot));
    if (!cm) return NULL;
    cm->m = _mot;
    cm->suite = NULL;
    return cm;
}

int insererEnTete(Texte *texte, Mot _mot){
    CelluleMot * cm = malloc(sizeof(CelluleMot));
    if (!cm) return 0;
    cm->m = _mot;
    cm->suite = *texte;
    *texte = cm;
    return 1;
}

Mot initMot(FILE * f){
    char c = fgetc(f);
    Mot _mot = {
        .taille = MAX_MOT
    };
    int c_count = 0;
    while (!(c==' ' || c=='\n' || c=='\t' || c==EOF || c=='.' || c==',' || c=='?' || c=='!' || c==';' || c=='(')){
        _mot.mot[c_count] = c;
        c = fgetc(f);
        c_count++;
    }
    _mot.mot[c_count] = ' ';
    return _mot;
};

Texte initTexte(FILE * f){
    while ()
};

int main(void){
    FILE * f = fopen("/texts/text1.txt", "r");
}
