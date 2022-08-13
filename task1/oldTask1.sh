#!/bin/bash
# LC_ALL=C forces applications such as awk to use the default language for output and sorting to be byte-wise

# Select words of 3 to 15 characters that are all from the English alphabet.
string=`LC_ALL=C awk 'length($0) >= 3 && length($0) <= 15' ../input.txt | awk '/^[a-zA-Z]+$/'` 

# Split a string into an array delimited by whitespace
arr=($string)

numRowsAndCols=sqrt(${#arr[@]})

# Create a 2D array
declare -A matrix

# Store the initical character of the first element in the array (each row will contain words that start with the same character)
initialWord=${arr[0]};
initialChar=${initialWord:0:1};

# Indices for the 2d array
i=0;
j=0;

maxCols=0;
numRows=1;

for k in "${arr[@]}"
do
	firstWord="$k"
	firstChar=${firstWord:0:1}
	if [ $firstChar == $initialChar ]
	then
		matrix[$i,$j]=$firstWord;
		((j++))
	else
		if ((maxCols < j))
		then
			maxCols=$((j-1));
		fi
		((numRows++));
		initialChar=$firstChar;
		j=0;
		((i++));
		matrix[$i,$j]=$firstWord;
		((j++))

	fi	
done

echo "${#matrix[@]}"

#if ((maxCols < j))
#then
#	maxCols=$((j-1));
#fi
#declare -a finalArr;
#index=0;
#
#for (( i = 0; i <= maxCols; i++)) 
#do
#	for (( j = 0; j < numRows; j++))
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
