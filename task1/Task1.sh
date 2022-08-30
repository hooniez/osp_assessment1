#!/bin/bash

# Select words whose letters all from the English alphabet.
awk '/^[a-zA-Z]+$/' ../original_data/combined.txt  | sort --unique > ../input_bash.txt

# I have spent extra hours to ensure the script's shuffled outcome is identical to that of its equivalent program in C++.
# It was only after my great effort that I found out the code below is useless because of the requirement to shuffle has turned out to be unnecessary at odds with what the specification initially required.

## Split a string into an array delimited by whitespace
#arr=($string)
#
#numRowsAndCols=$(echo "${#arr[@]}" | awk '{print int(sqrt($1))}');
#
## Create a 2D array
#declare -A matrix
#
## Store the words in a 2d array and transpose it to shuffle the words
#
#index=0;
#for (( row = 0; row <= numRowsAndCols; row++))
#do
#	for(( col = 0; col <= numRowsAndCols; col++))
#	do
#		value=${arr[$index]};
#		# If value has no value
#		if test -z "$value"
#		then
#			:
#		else
#			matrix[$row,$col]=$value;
#			((index++));
#		fi
#	done
#done
#
#declare -a finalArr;
#index=0;
#
#for (( i = 0; i <= numRowsAndCols; i++))
#do
#	for (( j = 0; j <= numRowsAndCols; j++))
#	do
#		value=${matrix[$j,$i]};
#		if test -z "$value"
#		then
#			:
#		else
#			finalArr[$index]=$value;
#			((index++));
#			echo $value;
#		fi
#	done
#done;
