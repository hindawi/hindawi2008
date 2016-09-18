#!/bin/bash -f

#Preinstall reqs for Fedora
#for n in konsole
for n in kommander gawk xpdf mailx sendmail flex bison gcc gcc-c++ gcc-java libX11-devel kdebase
	do 
		yum install $n
	done
ln /usr/bin/bison /usr/bin/yacc

