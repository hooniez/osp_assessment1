# Combine all the files from original_data into input.txt
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
