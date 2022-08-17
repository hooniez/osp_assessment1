#!/bin/bash
# LC_ALL=C forces applications such as awk to use the default language for output and sorting to be byte-wise

# Select words of 3 to 15 characters that are all from the English alphabet.
string=`LC_ALL=C awk 'length($0) >= 3 && length($0) <= 15' ../input.txt | awk '/^[a-zA-Z]+$/'` 

# Split a string into an array delimited by whitespace
arr=($string)

numRowsAndCols=$(echo "${#arr[@]}" | awk '{print int(sqrt($1))}');

# Create a 2D array
declare -A matrix

# Store the words in a 2d array and transpose it to shuffle the words

index=0;
for (( row = 0; row <= numRowsAndCols; row++))
do
	for(( col = 0; col <= numRowsAndCols; col++)) 
	do	
		value=${arr[$index]};
		# If value has no value
		if test -z "$value"
		then
			:
		else	
			matrix[$row,$col]=$value;
			((index++));
		fi
	done
done	

declare -a finalArr;
index=0;

for (( i = 0; i <= numRowsAndCols; i++)) 
do
	for (( j = 0; j <= numRowsAndCols; j++))
	do
		value=${matrix[$j,$i]};
		if test -z "$value"
		then
			:
		else
			finalArr[$index]=$value;
			((index++));
			echo $value;
		fi
	done
done;
