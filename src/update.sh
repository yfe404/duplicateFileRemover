#!/bin/bash

DIR=$*
FILES=`find $DIR -type f -user $USER 2> /dev/null | tr "\n" " "`

rm  pathnames.db 2> /dev/null
rm md5.db 2> /dev/null



for i in $FILES 
do  
	echo  $i >> pathnames.db 
	echo "`md5sum $i | tr -s " " | cut -d " " -f 1`" >> md5.db 
done
