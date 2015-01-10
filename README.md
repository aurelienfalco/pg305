###############
# Compilation #
###############
make

#############
# Execution #
#############
make exec p=proc seq=bool p=bool
Paramètres (optionnels):
- p : le nombre de processus (par défaut 4);
- t : le nombre de threads de travail (par défaut 5);
- a : l'alphabet (par défaut l'aphabet abcdefghijklmnopqrstuvwxyz);
- r : longueur maximum des mots à vérifier (par défaut 5) ;
- m : le mot de passe (par défaut passwd).


#########
# Stats #
#########
make stat
ou 
make qsub
Cette dernière option seulement si vous êtes sur plafrim, mais attention au dossier contenant les sources. Dans ce cas, il faudra peut-être modifier le batch.
La série de speedup ainsi affichée par ligne correspond au nombre de processeur suivi du speedup calculé. 

###########
# Courbes #
###########
make plot
Vous pouvez choisir le fichier contenant les données par la variable stat.
Par exemple:
make plot stat=data.txt
