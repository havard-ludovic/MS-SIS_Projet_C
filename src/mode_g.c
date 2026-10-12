#include "mode_g.h"

// Fonction interne optimisée pour calculer le hash MD5
static void compute_md5(const unsigned char *data, size_t len, char output[33]) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5(data, len, digest);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&output[i * 2], "%02x", (unsigned int)digest[i]);
    }
    output[32] = '\0';
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

    // Ouverture du dictionnaire en lecture
    FILE *infile = fopen(input_path, "r");
    if (!infile) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier d'entrée '%s'\n", input_path);
        return 1;
    }

    // Ouverture de la T3C en écriture
    FILE *outfile = fopen(output_path, "w");
    if (!outfile) {
        fprintf(stderr, "Erreur : Impossible de créer le fichier de sortie '%s'\n", output_path);
        fclose(infile);
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read_bytes;
    char hash_str[33];

    // Lecture performante ligne par ligne
    while ((read_bytes = getline(&line, &len, infile)) != -1) {
        // Nettoyage des retours à la ligne (\n ou \r\n)
        if (read_bytes > 0 && (line[read_bytes - 1] == '\n' || line[read_bytes - 1] == '\r')) {
            line[read_bytes - 1] = '\0';
            read_bytes--;
            if (read_bytes > 0 && line[read_bytes - 1] == '\r') {
                line[read_bytes - 1] = '\0';
                read_bytes--;
            }
        }

        // Calcul du hash MD5 de la ligne brute
        compute_md5((unsigned char *)line, read_bytes, hash_str);

        // Écriture du résultat dans la T3C au format : <hash> <mot>
        fprintf(outfile, "%s %s\n", hash_str, line);
    }

    // Libération des ressources
    free(line);
    fclose(infile);
    fclose(outfile);

    return 0;
}