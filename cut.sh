for file in output*.txt;
do cut -c 3- $file > final$file;
done

diffuse inaloutput.txt finaloutput_task2.txt finaloutput_task3.txt finaloutput_task4.txt