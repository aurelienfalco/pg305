set terminal png
set title "Temps d'execution en fonction du nombre de processus"
set ylabel "Temps d'execution (s)"	    
set xlabel "Nombre de processus (Plafrim)"
set output 'graph_procs.png'
plot 'graph_procs.dat' using 1:((\$2+\$3+\$4+\$5+\$6)/5.0) with lines title 'Mot de passe'
