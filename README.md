# 3Dbird : Projet d'INF443

## Idée générale

Nous proposons de modéliser un oiseau se baladant au dessus d'une île. Nous proposons de changer le visuel de l'oiseau et le visuel de l'île, choix thématique des visuels. 

L'oiseau bougerait en fonction des actions de l'utilisateur (droite pour aller à droite, gauche pour aller à gauche, haut pour aller vers le haut et bas pour aller vers le bas).

Au fur et à mesure que l'oiseau vole, sa vitesse augmente. Il a la possibilité de passer dans des anneaux qui voleront, ce qui lui fera gagner des points. Cependant, cela pourrait augmenter aussi sa vitesse. 
Pour aller avec cela, nous maintiendrons un score correspondant au temps de vol et au nombre d'anneaux passés.

Enfin, lorsque l'oiseau rencontre un obstacle, il s'écrase et perd.

A noter : le terrain a une surface limitée, l'oiseau ne peut pas aller au delà. Les limites seront visuels (par exemple, la limite du terrain sur le jeu MineCraft). Ensuite, il y aura aussi une limite en hauteur, cependant elle ne sera pas matérialisé car on changera juste la direction de l'oiseau pour qu'il redescende.

## Organisation du projet

Nous vous proposons l'organisation du projet suivante :

### Phase 1 : modélisation du terrain

Modélisation d'un terrain de base avec les bordures sur les cotés.

### Phase 2 : Modélisation de l'oiseau

Modélisation de l'oiseau et interaction avec l'utilisateur via les touches du clavier, et accélération de l'oiseau avec le temps.

### Phase 3 : Résolution des chocs

Nous mettrons en place la possibilité de se prendre un arbre, un caillou, un bateau, le terrain (océan et île) ou les bords.
Ensuite, lorsque l'oiseau touche le plafond, nous le ferons redescendre.

### Phase 4 : Ajout des fruits et du scoring

Nous modéliserons les anneaux et feront en sorte que l'oiseau puisse passer à travers. Nous ajouterons aussi le score dans la fenètre graphique du joueur.

### Phase 5 : Personnalisation du thème

Enfin, nous ajouterons la possibilité de jouer avec plusieurs paramètres de jeux. 
Pour cela, nous mettrons en place une sorte de menu avec des paramètres. Vous pourrez choisir la couleur du corps de l'oiseau ainsi que les différents paramètres de génération du terrain.

## Comment jouer

Le jeu utilise la librairie VCL. Il faut donc l'avoir dans ses dossiers pour pouvoir y jouer (et changer le chemin vers la librairie dans le CMakeLists.txt).

Ensuite, il suffit de se placer dans le dossier racine et lancer la commande :
```
cmake ./
```

Puis de lancer la commande :
```
make
```

Enfin, vous pouvez lancer le jeu avec la commande :
```
./3Dbird
```

Vous pouvez aussi avoir accès à la documentation en lançant la commande :
```
make documentation
```