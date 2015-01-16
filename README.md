############################
# Compilation et exécution #
############################

Sur votre ordinateur :

make exec [p=<number>] [t=<number>] [r=<number>] [m=<word>] [a=<word>] [c=<path>]

Sur Plafrim :
le dossier nommé pg305-hf doit être à la racine de votre $HOME.

make qsub [p=<number>] [t=<number>] [r=<number>] [m=<word>] [a=<word>] [c=<path>]

Paramètres (optionnels):
- p : le nombre de processus (par défaut 4);
- t : le nombre de threads de travail (par défaut 5);
- a : l'alphabet (par défaut l'aphabet abcdefghijklmnopqrstuvwxyz);
- r : longueur maximum des mots à vérifier (par défaut 5) ;
- m : le mot de passe (par défaut passwd) ;
- c : le chemin de l'exécutable "slave" (par défaut ./slave).


###########
# Courbes #
###########

Sur Plafrim, il est possible de générer automatiquement les graphes
présents dans le rapport, pour cela exécutez :
./createGraph.sh