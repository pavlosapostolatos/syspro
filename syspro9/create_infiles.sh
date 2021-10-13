#! /bin/bash
if [ $# -ne 3 ]
then
exit -1
fi
inputfile=$1
input_dir=$2
numFilesPerDirectory=$3
if [ $# -ne 3 ]
then
exit -1
fi
if [[ -d "$input_dir" ]]
then
    echo "$input_dir exists on your filesystem.exiting"
    rm -r $input_dir
    #exit -1
fi
mkdir "$input_dir"
while IFS= read -r line || [ -n "$line" ];
    do
    read -ra parts <<<"$line"
    country="${parts[3]}"
    if [[ -d "$input_dir/$country" ]]
    then
        read -r counter < "$input_dir/$country/counter.txt"
        #unset 'parts[3]'# lathos ekfonisis. de diagrafo ti xora
        echo "${parts[@]}" >> "$input_dir/$country/$country-$counter.txt"
        #output entire array into the txt
        echo "$(( $counter %$numFilesPerDirectory +1))" > "$input_dir/$country/counter.txt"
        #update the counter
    else
        mkdir "$input_dir/$country"
        for (( i=1; i<=$numFilesPerDirectory; i++ ))
        do
            touch "$input_dir/$country/$country-$i.txt"
        done
        #unset 'parts[3]'
        echo "${parts[@]}" >> "$input_dir/$country/$country-1.txt"
        echo "$((2%$numFilesPerDirectory))" > "$input_dir/$country/counter.txt"
    fi  
done < $inputfile # store all country names in an array
read -ra parts <<< $(echo $input_dir/*)
echo ${parts[@]}
for i in "${parts[@]}" #accessing each element of array  
do  
    rm $i/counter.txt
done  