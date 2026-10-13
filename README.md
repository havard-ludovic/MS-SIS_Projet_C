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

lancez le conteneur
sur linux
```
docker run --rm -it -v $(pwd):/app c-cracker-env
```

sur windows
```
docker run --rm -it -v ${PWD}:/app c-cracker-env
```

Dans la machine docker
Compiler le code
```
make
```

Lancer le Mode G
```
./t3c G -i /opt/rockyou.txt -o table.t3c
```

Lancer le Mode L
```

```