#!/bin/sh
C=0; D=0; R=0; A=0
if [ "${1:0:1}" == "-" ]; then
	i=1
	while [ "${1:${i}:1}" ]; do
		CHAR=${1:${i}:1}
		if [ "$CHAR" == "c" ]; then C=1
		elif [ "$CHAR" == "d" ]; then D=1
		elif [ "$CHAR" == "r" ]; then R=1
		elif [ "$CHAR" == "a" ]; then A=1
		else
			printf "This program compiles c/cpp files using make and runs the\
 executable file while calculating the elapsed time\n
It uses custom cflags from .my_flags and custom paths from .my_paths in the same directory as the Makefile\n
Note: In the case of using more than one parameter, concatenate them (i.e.\
 ./compile_and_run.sh -cra arg1 arg2)\n
./compile_and_run.sh [-[c|r|d|t]] [-a [arg1 arg2 ...]]\n
  -c\tcleans (deletes) already built files
  -r\tbuilds using release configurations
  -d\tbuilds using debug configurations, and runs the debugger
  -a\tsends the following parameters as arguments to main function
  -h\tshows this help message"
			exit 0
		fi
		i=$(($i + 1))
	done
	shift
fi
if [ $C -eq 1 ]; then
	make clean
fi
if [ $D -eq 1 ]; then
	make DEBUG=$D all
	if [ ! -e *.exe ]; then
		exit 127
	fi
	make debug
	exit 0
fi
make RELEASE=$R all
if [ ! -e *.exe ]; then
	exit 127
fi
if [ $A -eq 0 ]; then
	shift $#
fi
echo -------------------------------
START_TIME=$(date +%s.%6N)
./*.exe $@
TMP=$?
END_TIME=$(date +%s.%6N)
echo
echo -------------------------------
echo "Program finished with exit code" $TMP
echo "Elapsed time:" $(python -c "print(round(($END_TIME - $START_TIME) * 1000, 3))") "ms"
exit 0
