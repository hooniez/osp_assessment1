for file in output*.txt;
do cut -c 3- $file > final$file;
done

#diffuse finaloutput.txt finaloutput_task2.txt finaloutput_task3.txt finaloutput_task4.txt finaloutput_task5.txt