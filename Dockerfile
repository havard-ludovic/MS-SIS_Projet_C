# Utilisation d'une image Ubuntu stable
FROM ubuntu:22.04

# Évite les interactions bloquantes pendant l'installation
ENV DEBIAN_FRONTEND=noninteractive

# Installation des outils de développement, OpenSSL, curl et tar pour décompresser
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libc6-dev \
    curl \
    tar \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Définition du répertoire de travail dans le conteneur
WORKDIR /app

# Téléchargement et extraction directe de rockyou.txt depuis SecLists
RUN curl -L -o /app/rockyou.txt.tar.gz https://github.com/danielmiessler/SecLists/raw/master/Passwords/Leaked-Databases/rockyou.txt.tar.gz \
    && tar -xzf /app/rockyou.txt.tar.gz -C /opt/ \
    && rm /app/rockyou.txt.tar.gz

# Par défaut, lance un shell bash
CMD ["/bin/bash"]