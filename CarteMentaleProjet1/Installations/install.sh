#!/bin/bash

rep_courant=`pwd`

nbSpace=`pwd | grep -o " " | grep -c " "`

if [ "$nbSpace" == "0" ]
then
     touch "yatea.dtd"
     mkdir "build"

     #unitex
     cd unitex/Src/C++/build
     make install
     cd ../../../..


     #TagEN
     cd TagEN/src
     ./compile
     cd ../..

     #TreeTagger
     cd ./tree-tagger
     sh install-tagger.sh
     cd ..


     echo "Fin intallation"
     echo "pour executer il faut taper la commande suivant: tal"
else
   echo "Erreur: Le path du paquet contient de l'espace."
   echo "Merci d'installer  l'application dans un path qui ne contient pas d'espace."
fi

exit 0
