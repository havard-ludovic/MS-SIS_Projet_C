# MS-SIS_Projet_C

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
echo "f25a2fc72690b780b2a14e140ef6a9e0" | ./t3c L -t table.t3c
```

Lancer le Mode L avec plusieurs hash
```
/t3c L -t table.t3c <<EOF
5f4dcc3b5aa765d61d8327deb882cf99
e10adc3949ba59abbe56e057f20f883e
hashinconnuinexistant123456789
EOF
```

Lancer le Mode L avec un fichier
```
cat test.txt | ./t3c L -t table.t3c
```

