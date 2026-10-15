#include "mode_l.h"

// https://man7.org/linux/man-pages/man3/isatty.3.html

// Structure pour stocker une association hash -> mot en mémoire
typedef struct {
    char hash[MD5_HEX_STRING_LEN+1];
    char *word;
} Entry;

// Fonction de comparaison pour qsort et bsearch (tri par hash)
static int compare_entries(const void *a, const void *b) {
    return strcmp(((const Entry *)a)->hash, ((const Entry *)b)->hash);
}

// Fonction de comparaison spécifique pour bsearch (recherche d'une chaîne clé)
static int compare_search(const void *key, const void *element) {
    return strcmp((const char *)key, ((const Entry *)element)->hash);
}

// Fonction qui vérifie le checksum global du fichier T3C
// Retourne 1 si le fichier est intègre, 0 en cas de corruption ou d'altération.
int verify_and_rewind_table(FILE *file) {
    EVP_MD_CTX *global_ctx = EVP_MD_CTX_new();
    if (!global_ctx) {
        fprintf(stderr, "Erreur : Échec d'allocation du contexte EVP global.\n");
        return 0;
    }
    EVP_DigestInit_ex(global_ctx, EVP_md5(), NULL);

    char expected_checksum[MD5_HEX_STRING_LEN+1] = "";
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Première passe : lecture globale pour recalculer le hash et extraire le checksum attendu
    while ((read = getline(&line, &len, file)) != -1) {
        if (strncmp(line, "### CHECKSUM ", 13) == 0) {
            // Extraction du checksum inscrit à la fin du fichier
            memcpy(expected_checksum, line + 13, MD5_HEX_STRING_LEN);
            expected_checksum[MD5_HEX_STRING_LEN] = '\0';
            continue; // On ne l'inclut pas dans le calcul mathématique du contenu
        }
        // Mise à jour de l'empreinte avec la ligne de données lue
        EVP_DigestUpdate(global_ctx, line, read);
    }

    free(line);

    // Sécurité : si aucun checksum n'a été trouvé dans le fichier
    if (expected_checksum[0] == '\0') {
        fprintf(stderr, "Erreur : Fichier T3C invalide ou ancien format (aucun checksum trouvé).\n");
        EVP_MD_CTX_free(global_ctx);
        return 0;
    }

    // Calcul final du checksum réel
    unsigned char computed_hash[MD5_DIGEST_LENGTH];
    unsigned int computed_hash_len = 0;
    EVP_DigestFinal_ex(global_ctx, computed_hash, &computed_hash_len);
    EVP_MD_CTX_free(global_ctx);

    char computed_hash_str[MD5_HEX_STRING_LEN+1];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&computed_hash_str[i * 2], "%02x", computed_hash[i]);
    }
    computed_hash_str[MD5_HEX_STRING_LEN] = '\0';

    // Comparaison stricte entre le calcul et la valeur enregistrée
    if (strcmp(computed_hash_str, expected_checksum) != 0) {
        fprintf(stderr, "Erreur : Le fichier table.t3c a été modifié ou corrompu !\n");
        return 0; // Échec de l'intégrité
    }

    // Si tout est valide, on remet le curseur au début pour permettre le chargement normal
    rewind(file);
    return 1; // Succès
}

int run_mode_l(int argc, char *argv[]) {
    char *table_path = NULL;

    // Analyse des options avec getopt_long
    int opt;
    static struct option long_options[] = {
        {"table", required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    optind = 2; // Ignore "./t3c" et "L"
    while ((opt = getopt_long(argc, argv, "t:", long_options, NULL)) != -1) {
        switch (opt) {
            case 't': table_path = optarg; break;
            default:
                fprintf(stderr, "Usage : ./t3c L -t <t3c_file>\n");
                return 1;
        }
    }

    // Vérification de l'option obligatoire
    if (!table_path) {
        fprintf(stderr, "Erreur : L'option -t est obligatoire en mode L.\n");
        return 1;
    }
    
    // Si aucun flux n'est envoyé, on quitte
    if (isatty(STDIN_FILENO)) {
        fprintf(stderr, "Il n'y a pas de hash (utilisez un pipe)\n");
        return 1;
    }

    // Ouverture du fichier T3C
    FILE *file = fopen(table_path, "r");
    if (!file) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier T3C '%s'\n", table_path);
        return 1;
    }

    // Vérification de l'intégrité du fichier avant de charger en mémoire
    if (!verify_and_rewind_table(file)) {
        fclose(file);
        fprintf(stderr, "Erreur : le fichier table.t3c a été modifié\n");
        return 1; // Arrêt du programme si le fichier a été modifié
    }

    // Chargement dynamique des données en mémoire
    size_t capacity = 1000; // Capacité initiale du tableau alloué dynamiquement
    size_t count = 0; // Compteur pour suivre le nombre réel d'entrées actuellement stockées dans le tableau
    Entry *entries = malloc(capacity * sizeof(Entry)); // Allocation initiale en mémoire d'un tableau d'éléments de type "Entry"
    if (!entries) {
        fprintf(stderr, "Erreur : Allocation mémoire échouée.\n");
        fclose(file);
        return 1;
    }

    char *line = NULL; // Pointeur stockant la ligne lue par getline
    size_t len = 0; // Taille du buffer alloué pour la ligne
    ssize_t read; // Stocke le nombre de caractères lus par getline

    // Lecture ligne par ligne du fichier T3C
    while ((read = getline(&line, &len, file)) != -1) {
        // Nettoyage des retours à la ligne
        if (read > 0 && (line[read - 1] == '\n' || line[read - 1] == '\r')) {
            line[read - 1] = '\0';
            read--;
            if (read > 0 && line[read - 1] == '\r') {
                line[read - 1] = '\0';
                read--;
            }
        }

        // Vérification minimale du format (<hash> <mot>)
        if (read < 34 || line[MD5_HEX_STRING_LEN] != ' ') continue;

        // Agrandissement dynamique du tableau si nécessaire
        if (count >= capacity) {
            capacity *= 2;
            Entry *temp = realloc(entries, capacity * sizeof(Entry));
            if (!temp) {
                fprintf(stderr, "Erreur : Réallocation mémoire échouée.\n");
                break;
            }
            entries = temp;
        }

        // Copie du hash
        memcpy(entries[count].hash, line, MD5_HEX_STRING_LEN);
        entries[count].hash[MD5_HEX_STRING_LEN] = '\0';

        // Copie du mot associé (situé après l'espace)
        entries[count].word = strdup(line + MD5_HEX_STRING_LEN+1);

        count++;
    }

    free(line);
    fclose(file);

    // Tri impératif du tableau en mémoire pour que bsearch fonctionne
    qsort(entries, count, sizeof(Entry), compare_entries);

    // Boucle de recherche rapide depuis l'entrée standard (stdin)
    char *query = NULL;
    size_t q_len = 0;
    ssize_t q_read;

    while ((q_read = getline(&query, &q_len, stdin)) != -1) {
        // Nettoyage du hash recherché
        if (q_read > 0 && (query[q_read - 1] == '\n' || query[q_read - 1] == '\r')) {
            query[q_read - 1] = '\0';
            q_read--;
            if (q_read > 0 && query[q_read - 1] == '\r') {
                query[q_read - 1] = '\0';
                q_read--;
            }
        }

        if (q_read == 0) continue;

        // Recherche O(log n)
        Entry *found = bsearch(query, entries, count, sizeof(Entry), compare_search);

        if (found) {
            printf("%s\n", found->word);
        } else {
            printf("Le hash n'a pas été trouvé\n");
        }
    }

    // Nettoyage de la mémoire allouée
    free(query);

    for (size_t i = 0; i < count; i++) {
        free(entries[i].word);
    }
    free(entries);

    return 0;
}

