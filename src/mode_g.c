#include "mode_g.h"

// https://linux.die.net/man/3/getopt_long

// Fonction interne optimisée pour calculer le hash MD5
static void compute_md5(const unsigned char *data, size_t len, char output[33]) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5(data, len, digest);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&output[i * 2], "%02x", (unsigned int)digest[i]);
    }
    output[32] = '\0';
}

// Fonction pour écrire la ligne de checksum à la fin du fichier table.t3c
static void write_checksum_footer(FILE *outfile, MD5_CTX *ctx) {
    unsigned char global_hash[MD5_DIGEST_LENGTH];
    MD5_Final(global_hash, ctx);

    // Conversion du hash binaire en chaîne hexadécimale de 32 caractères
    char global_hash_str[33];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&global_hash_str[i * 2], "%02x", global_hash[i]);
    }
    global_hash_str[32] = '\0';

    // Écriture de la signature à la fin du fichier
    fprintf(outfile, "### CHECKSUM %s\n", global_hash_str);
}

int run_mode_g(int argc, char *argv[]) {
    char *input_path = NULL;
    char *output_path = NULL;
    char *algo = "md5"; // Valeur par défaut imposée

    // Analyse des options avec getopt_long
    int opt;
    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"algo", required_argument, 0, 'a'},
        {0, 0, 0, 0}
    };

    optind = 2; // Ignore "./t3c" et "G"
    while ((opt = getopt_long(argc, argv, "i:o:a:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'i': input_path = optarg; break;
            case 'o': output_path = optarg; break;
            case 'a': algo = optarg; break;
            default:
                fprintf(stderr, "Usage : ./t3c G -i <dictionnaire> -o <t3c_output> [-a <algo>]\n");
                return 1;
        }
    }

    // Vérification des paramètres obligatoires
    if (!input_path || !output_path) {
        fprintf(stderr, "Erreur : Les options -i et -o sont obligatoires en mode G.\n");
        return 1;
    }

    // Vérification de l'algorithme demandé
    if (strcasecmp(algo, "md5") != 0) {
        fprintf(stderr, "Erreur : Algorithme '%s' non supporté (seul 'md5' est requis).\n", algo);
        return 1;
    }

    // VÉRIFICATIONS DU FICHIER DICTIONNAIRE

    // Vérification de l'extension .txt
    char *ext = strrchr(input_path, '.');
    if (!ext || strcasecmp(ext, ".txt") != 0) {
        fprintf(stderr, "Erreur : Le dictionnaire '%s' doit être un fichier texte (.txt).\n", input_path);
        return 1;
    }

    // Ouverture du dictionnaire en lecture
    FILE *infile = fopen(input_path, "r");
    if (!infile) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier d'entrée '%s' (introuvable ou permissions insuffisantes).\n", input_path);
        return 1;
    }

    // Vérification que le fichier n'est pas vide
    fseek(infile, 0, SEEK_END);
    long size = ftell(infile);
    if (size == 0) {
        fprintf(stderr, "Erreur : Le fichier '%s' est vide.\n", input_path);
        fclose(infile);
        return 1;
    }
    rewind(infile); // Remet le curseur au début du fichier pour la lecture


    // Ouverture de la T3C en écriture
    FILE *outfile = fopen(output_path, "w");
    if (!outfile) {
        fprintf(stderr, "Erreur : Impossible de créer le fichier de sortie '%s'\n", output_path);
        fclose(infile);
        return 1;
    }

    // Initialisation du contexte de checsum global
    MD5_CTX global_ctx;
    MD5_Init(&global_ctx);

    char *line = NULL; // Pointeur stockant la ligne lue par getline
    size_t len = 0; // Taille du buffer alloué pour la ligne
    ssize_t read_bytes; // Stocke le nombre de caractères effectivement lus par getline
    char hash_str[33]; // Tableau de caractères pour stocker le hash MD5 au format hexadécimal avec caractère /0 à la fin

    // Lecture ligne par ligne en byte
    while ((read_bytes = getline(&line, &len, infile)) != -1) {
        /*// Nettoyage des retours à la ligne (\n ou \r\n)
        if (read_bytes > 0 && (line[read_bytes - 1] == '\n' || line[read_bytes - 1] == '\r')) {
            line[read_bytes - 1] = '\0';
            read_bytes--;
            if (read_bytes > 0 && line[read_bytes - 1] == '\r') {
                line[read_bytes - 1] = '\0';
                read_bytes--;
            }
        }*/
        // Ignorer les lignes vides
        if (read_bytes <= 1) continue;

        // Nettoyage robuste des retours à la ligne de fin (\n et \r)
        while (read_bytes > 0 && (line[read_bytes - 1] == '\n' || line[read_bytes - 1] == '\r')) {
            line[read_bytes - 1] = '\0';
            read_bytes--;
        }

        // Si la ligne est vide après nettoyage, on passe à la suivante
        if (read_bytes == 0) continue;

        // Calcul du hash MD5 de la ligne brute
        compute_md5((unsigned char *)line, read_bytes, hash_str);

        // Préparation de la ligne formatée exacte qui sera écrite dans le fichier
        char record[1024];
        int record_len = snprintf(record, sizeof(record), "%s %s\n", hash_str, line);

        // Mise à jours du checksum global
        MD5_Update(&global_ctx, record, record_len);

        // Écriture du résultat dans la T3C au format : <hash> <mot>
        fputs(record, outfile);
    }

    // Ecriture du footer de checksum à la fin du fichier
    write_checksum_footer(outfile, &global_ctx);

    // Libération des ressources
    free(line);
    fclose(infile);
    fclose(outfile);

    printf("Table T3C générée avec succès : %s\n", output_path);

    return 0;
}