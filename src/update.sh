#!/bin/bash

DIR=$*
FILES=`find $DIR -type f -user $USER 2> /dev/null | tr "\n" " "` # recherche dans le dossier

# suppression des anciennes bases
rm  pathnames.db 2> /dev/null
rm md5.db 2> /dev/null



for i in $FILES # pour chaque fichier des dossiers...
do  
	echo  $i >> pathnames.db # ajout du path dans pathnames.db
	echo "`md5sum $i | tr -s " " | cut -d " " -f 1 2> /dev/null`" >> md5.db # et ajout de la clé md5 dans md5.db
done
