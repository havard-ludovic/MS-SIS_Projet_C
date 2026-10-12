PROJET
(termes en gras au sens RFC 2119)

A/ ATTENDUS DU PROGRAMME A LIVRER
--- Le programme **doit** être écrit en langage C ;
--- Le programme **doit** avoir au moins deux modes de fonctionnements : génération de condensats (G pour generate) et recherche parmi une liste de condensats (L pour lookup) ;

--- Le mode G **doit** avoir une fonctionnalité de lecture des chaînes de caractères d'entrée à partir d'un fichier (dit dictionnaire) ;
--- Le mode G **doit** accepter que le séparateur des chaînes de caractères d'entrées soit un caractère saut de ligne "\n" ;
--- Le mode G **doit** générer le condensat cryptographique de chacune des chaînes de caractères d'entrées ;
--- Le mode G **doit** avoir une fonctionnalité d'écriture de la table de correspondance chaine-condensat (T3C) sous forme de fichier ;
--- Le mode G doit supporter l'algorithme MD5 ; il peut proposer de générer les condensats avec des algorithmes alternatifs ; si le choix d'algorithme n'est pas précisé en ligne de commande, MD5 doit être utilisé par défaut ;

--- Le mode L **doit** avoir une fonctionnalité de chargement en mémoire une T3C à partir d'un fichier ;
--- Le mode L **doit** avoir une fonctionnalité de recherche rapide d'une chaîne de caractère à partir d'un condensat ;
--- Le mode L **doit** accepter de recevoir les condensats par le flux standard d'entrée ;
--- Le mode L **doit** accepter que les condensats fournis soient séparés par un caractère saut de ligne "\n" ;
--- Le mode L **doit** accepter d'afficher les chaînes de caractères trouvées sur le flux standard de sortie ;

--- Pour tous les modes, le programme **doit** accepter de recevoir les fichiers d'entrée (dictionnaire, T3C) sous la forme d'un chemin donné en argument de ligne de commande ;
--- Pour tous les modes, le programme **doit** accepter de recevoir tous les paramètres utiles à sa bonne exécution en arguments de ligne de commande ;

--- Le programme **peut** exiger des pré-requis pour la compilation ou le fonctionnement du programme ;
--- Le programme **ne doit pas** exiger d'autres pré-requis que : un OS linux, l'installation de bibliothèques via paquets des dépôts standards publics debian, Docker.


B/ PROTOTYPE D'APPEL (interface imposée)

Afin de permettre une correction automatisée identique pour tous les rendus, tous les programmes **doivent** respecter le contrat d'interface suivant. Le format interne du fichier T3C reste libre ; seule l'interface d'appel est imposée.

--- Le `makefile` **doit** produire, après exécution de la commande `make` à la racine du dépôt, un exécutable nommé `t3c`, situé à la racine du dépôt ;

--- Le mode **doit** être fourni en premier argument positionnel, sous la forme :
```
./t3c <G|L> [options]
```

--- En mode G, le programme **doit** accepter la syntaxe suivante :
```
./t3c G -i <dictionnaire> -o <t3c_output> [-a <algo>]
```
--- `-i` / `--input <path>` : chemin du fichier dictionnaire (obligatoire) ;
--- `-o` / `--output <path>` : chemin de sortie de la T3C (obligatoire) ;
--- `-a` / `--algo <nom>` : algorithme de condensat (optionnel) ; la valeur `md5` **doit** être supportée et utilisée par défaut si l'option est omise ;

--- En mode L, le programme **doit** accepter la syntaxe suivante :
```
./t3c L -t <t3c_path>
```
--- `-t` / `--table <path>` : chemin de la T3C à charger (obligatoire) ;
--- les condensats à rechercher sont fournis exclusivement via l'entrée standard (`stdin`), séparés par des sauts de ligne ;

--- Le flux standard de sortie (`stdout`) **doit** être réservé exclusivement au résultat utile du programme : en mode L, aucune autre information (log, message de progression, avertissement) ne doit y apparaître ;

--- Tous les messages d'erreur, avertissements ou informations de log **doivent** être écrits sur le flux d'erreur standard (`stderr`), jamais sur `stdout` ;

--- En mode L, le programme **doit** produire exactement une ligne de sortie par condensat lu en entrée, dans le même ordre que celui de l'entrée :
    - si le condensat est trouvé dans la T3C, la ligne correspondante contient la chaîne de caractères associée ;
    - si le condensat n'est pas trouvé, la ligne correspondante **doit** être vide ;

--- Le programme **doit** retourner un code de sortie égal à `0` lorsque l'exécution s'est déroulée normalement, y compris lorsque certains condensats fournis en mode L ne sont pas trouvés dans la T3C (ce n'est pas considéré comme une erreur) ;

--- Le programme **doit** retourner un code de sortie différent de `0` en cas d'erreur réelle (argument manquant ou invalide, fichier d'entrée inexistant ou illisible, etc.) ;

--- Le programme ne **doit** jamais se terminer par un arrêt anormal (crash, segmentation fault, core dump), y compris en cas d'entrée invalide, de fichier corrompu, de dictionnaire vide ou de paramètres manquants : dans tous ces cas, le programme **doit** se terminer proprement avec un message d'erreur sur `stderr` et un code de sortie non nul ;

--- Deux exécutions successives du mode G sur un même dictionnaire, avec le même algorithme, **doivent** produire des T3C fonctionnellement équivalentes (mêmes associations chaîne/condensat retrouvées en mode L), même si l'ordre de stockage interne diffère ;

--- Le programme **peut** proposer une option `--help` ou `-h` affichant un message d'usage minimal sur `stderr`.


C/ CONDITIONS DE LIVRAISON
--- Le dépot **doit** être livré par transmission d'une URL gitlab ou github accessible publiquement ;
--- Le dépot **doit** contenir un fichier README.MD à la racine qui documente la compilation et l'utilisation du programme, explicite les pré-requis s'il en existe ;
--- Le dépot **doit** contenir un fichier makefile qui permet de compiler le programme ;
--- Le dépôt **peut** contenir un fichier Dockerfile qui permet de construire une image docker pour compiler le programme ;
--- Le dépôt **peut** contenir un fichier Dockerfile qui permet de construire une image docker pour exécuter le programme ;
--- Si le dépôt contient un ou des fichiers Dockerfile, le fichier README.MD **doit** indiquer comment construire la ou les image(s) et son/leur utilisation ;
--- Le programme livré **doit** être sur une branche nommée "master" ;
--- Les fichiers du code source **doivent** être situés dans un dossier "src" à la racine du dépôt ;
--- Les documentations autre que le README.MD **doivent** être situées dans un dossier doc à la racine du dépôt ;
--- Le dépôt **peut** contenir des tests ou une documentation de test ;
--- Le dépôt **peut** contenir un système d'intégration continue.

D/ EVALUATION
Seront principalement pris en compte dans l'évaluation :
--- Le respect des attendus du programme à livrer exprimées supra ;
--- Le respect du prototype d'appel exprimé supra ;
--- Le respect des conditions de livraisons exprimées supra ;
--- La qualité de la documentation ;
--- Le bon fonctionnement des éléments livrés ;
--- La démonstration de la maîtrise des notions enseignées : lecture/écriture de fichiers, paramètres passés en ligne de commande, tableaux, pointeurs, allocation dynamique de mémoire, structures.

Seront également pris en compte dans l'évaluation :
--- La performance du programme (mesurée à titre indicatif, non éliminatoire) ;
--- L'absence d'erreurs mémoire (fuites, accès invalides), vérifiée à titre indicatif via l'outil valgrind ;
--- La pertinence du découpage du programme en différents fichiers ;
--- La lisibilité du code ;
--- La créativité par des fonctionnalités additionnelles ;
--- La pertinence des fonctions additionnelles ;
--- La facilité d'utilisation.

NB :
--- Un imposant malus sera appliqué si dépôt ne permet pas au correcteur de compiler et d'exécuter le programme en moins de 15 minutes sur un OS linux en suivant les instructions contenues dans le fichier README.MD ;
--- Un malus sera appliqué à tout arrêt anormal (crash) du programme constaté lors de la correction, quelle que soit l'entrée fournie ;
--- Le non-respect du prototype d'appel décrit en B/ (nom de l'exécutable, syntaxe des arguments, flux d'entrée/sortie, codes de retour) entrave la correction du rendu et sera pénalisé en conséquence.