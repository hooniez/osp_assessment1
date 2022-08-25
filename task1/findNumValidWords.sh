#This script finds the number of words of length 3 to 15 letters in the dataset
fileName="../output.txt"
words=$(awk 'length($0) >= 3 && length($0) <= 15' $fileName)
arr=($words)
echo "${#arr[@]}";