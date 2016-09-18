#!/bin/bash -f

#Preinstall reqs for Ubuntu
for n in x-dev xlibs-static-dev kommander gawk xpdf mailx sendmail g++ gcj flex bison konsole
	do 
		apt-get install $n
	done
ln /usr/bin/gawk /bin/gawk

