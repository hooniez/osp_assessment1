# Combine all the files from original_data into combined.txt
startIndex=1;
endIndex=12;
fileName="combined.txt";
# First, remove the file by the same name
if test -f "$fileName"; then
  rm $fileName;
fi

for (( index=startIndex; index <= endIndex; index++ ))
do
  cat "wlist_match$index.txt" >> $fileName
done


# Shuffle combined.txt and output numWords words
#numWords=1000000
#shuf $fileName -n $numWords > "../input.txt"
shuf $fileName > "../input.txt"
