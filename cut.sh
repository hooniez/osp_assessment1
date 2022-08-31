for file in output*.txt;
do cut -c 3- $file > final$file;
done

diff finaloutput_task3.txt finaloutput_task4.txt