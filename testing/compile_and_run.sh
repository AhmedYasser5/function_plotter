#!/bin/bash
C=0; D=0; R=0; A=0
while [ $# -gt 0 -a $A -eq 0 -a "${1:0:1}" == "-" ]
do
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
The default build has debugging configurations\n
Note: In the case of using more than one parameter, you can concatenate them (i.e.\
 ./compile_and_run.sh -cra arg1 arg2)\n
./compile_and_run.sh [-[c|r|d|t]] [-a [arg1 arg2 ...]]\n
	-c\tcleans (deletes) already built files that have the configurations given (debug or release)
  -r\tbuilds using release configurations
  -d\tbuilds using debug configurations, and runs the debugger
	-a\tsends the following parameters as arguments to main function (this should be the last argument)
  -h\tshows this help message"
			exit 0
		fi
		i=$(($i + 1))
	done
	shift
done
if [ $C -eq 1 ]; then
	make RELEASE=$R clean
fi
if [ $D -eq 1 ]; then
	make debug
	exit 0
fi
if [ $A -eq 0 ]; then
	shift $#
fi
make RELEASE=$R all
FILENAME=$(make RELEASE=$R getTarget)
echo --------------------------------------------------
\time -f "\n--------------------------------------------------\n\
Elapsed Time: %e sec\nCPU Percentage: %P" $FILENAME "$@"
exit $?
