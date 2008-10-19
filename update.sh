#!/bin/bash
logfile=logfile.log
echo "" > $logfile;
run () {

#	echo $1 >>  $logfile
#	$1 >> $logfile	
	
	$1

	if [ $? -eq 0 ]  ; then 
		echo -e "\033[1m$1 [\E[0;32mOK\033[0m\033[1m]\033[0m" 
	else 
		echo -e "\033[1m$1 [\E[0;31mFAILED\033[0m\033[1m]\033[0m" 
		exit 1;
	fi
}

autotools() {
	run "aclocal";

	run "libtoolize --copy --force";

	run "automake --add-missing --foreign" ;

	run "autoconf";
}

enter_folder() {
	if [ -d $1 ] ; then
		echo -e "\033[1m\E[0;34mActive folder $1 \E[0m[\E[0;32mOK\033[0m\033[1m]\033[0m"
		cd $1
	else 
		echo -e "\033[1m\E[44mActive folder  $1 \E[0m[\E[0;31mFAILED\033[0m\033[1m]\033[0m"
		exit 1;
	fi
}

enter_folder "spu"

autotools

cd ..

autotools

echo -e "Logfile" | cat $logfile





