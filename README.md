# ESIEA_Projet_C

```
ESIEA_projet_c/
├── Dockerfile
├── Makefile
├── README.md
├── src/
│   ├── main.c
│   ├── mode_g.c
│   ├── mode_g.h
│   ├── mode_l.c
│   ├── mode_l.h
│   ├── hash.c
│   └── hash.h
└── doc/
    └── sujet.md
```



## Utilisation
Construire l'image Docker
```
docker build -t c-cracker-env .
```

Lancer le conteneur en liant votre dossier courant
```
docker run --rm -it -v $(pwd):/app c-cracker-env
```

Compiler le code
```
make
```

lancer le programme
```
./cracker <hash_cible> <path/rockyou>
```
