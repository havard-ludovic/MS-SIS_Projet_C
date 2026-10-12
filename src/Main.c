#include "mode_g.h"
#include "mode_l.h"
#include <string.h>


// Affichage de l'aide sur le flux d'erreur standard
void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage :\n");
    fprintf(stderr, "  %s G -i <dictionnaire> -o <t3c_output> [-a <algo>]\n", prog_name);
    fprintf(stderr, "  %s L -t <t3c_path>\n", prog_name);
}

int main(int argc, char *argv[]) {
    // Vérification qu'un mode (G ou L) est bien fourni
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Aiguillage selon le premier argument
    if (strcmp(argv[1], "G") == 0) {
        return run_mode_g(argc, argv);
    } 
    else if (strcmp(argv[1], "L") == 0) {
        return run_mode_l(argc, argv);
    } 
    else {
        fprintf(stderr, "Erreur : Mode inconnu '%s'. Utilisez 'G' ou 'L'.\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }
}