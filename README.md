# 🐍 Snake Automatique – Jeu Snake en C (Terminal)

**Snake Automatique** est une version revisitée du célèbre jeu **Snake**, développée en **langage C** et jouable dans le **terminal**.  
Le serpent se déplace **automatiquement** en suivant le **chemin le plus optimisé** pour atteindre les pommes.  
Le projet est structuré en **quatre versions**, chacune introduisant de nouvelles mécaniques de jeu.

---

# Détails des Versions

> ### Version 1 – Plateau avec bordures
> <details> <summary>Détails</summary>
>
>> - Le serpent évolue dans un plateau délimité par des **bordures fixes**.
>> - Il se déplace automatiquement vers les pommes sans interaction clavier.
>>
>> 🔗 [Voir le code - Version 1](https://github.com/yannislechevere/SAE-1.01/tree/master/Version1)
>
> </details>

---

### Version 2 – Téléportation par les murs
- Ajout de **trous dans les bordures** permettant au serpent de **se téléporter** de l'autre côté du plateau.
- Optimisation du chemin prise en compte avec la nouvelle mécanique de téléportation.

🔗 [Voir le code - Version 2](https://github.com/yannislechevere/SAE-1.01/tree/master/Version2)

---

### 🔹 Version 3 – Obstacles (pavés)
- Introduction de **pavés**, agissant comme des **obstacles fixes** que le serpent doit éviter.
- L'algorithme de déplacement automatique prend en compte ces nouvelles contraintes.

🔗 [Voir le code - Version 3](https://github.com/yannislechevere/SAE-1.01/tree/master/Version3)

---

### 🔹 Version 4 – Deux serpents compétitifs
- Deux serpents se déplacent automatiquement et **compétitionnent** pour manger les pommes.
- Chaque serpent suit son propre algorithme d'optimisation pour atteindre les pommes le plus rapidement possible.
- Le score final détermine le gagnant.

🔗 [Voir le code - Version 4](https://github.com/yannislechevere/SAE-1.01/tree/master/Version4)

---

## 📚 Documentation

La **documentation Doxygen** est disponible pour la dernière version du projet :  
🔗 [Accéder à la documentation](https://github.com/yannislechevere/SAE-1.01/tree/master/Doxygen)

---

## ⚙️ Technologies

- Langage : **C**
- Interface : **Terminal (ligne de commande)**
- Générateur de documentation : **Doxygen**

---

## 👨‍💻 Auteur

Projet réalisé par [Yannis Lechevere](https://github.com/yannislechevere) dans le cadre de la SAE 1.01.

---

## 🕹️ Objectif du Projet

Ce projet met en œuvre :
- Des **algorithmes de pathfinding** simples.
- Une approche **itérative** du développement logiciel avec ajout progressif de fonctionnalités.
- Des techniques de **gestion de collisions**, **téléportation**, **obstacles** et **intelligence rudimentaire** pour les déplacements automatisés.

---

## ✅ Pour exécuter le jeu

```bash
# Aller dans le dossier de la version souhaitée
cd Version4

# Compiler
make

# Lancer le jeu
./snake
