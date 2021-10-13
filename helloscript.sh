#! /bin/bash
echo $#
if [ $# -ne 4 ]
then
exit -1
fi
virus_file=$1
countries_file=$2
dups_allowed=$4
numlines=$3
variable_name=$(tr -dc A-Za-z </dev/urandom | head -c $(($RANDOM% 9 + 3)) ;)
echo $virus_file
echo $countries_file
echo $numlines
echo $dups_allowed
i=0
#countries()
while IFS= read -r line
    do
    countries[i]="$line"
    i=$(( i+1 ))   
done < $countries_file
echo  "${countries[@]}"
#echo ${#countries[@]}
i=0
while IFS= read -r line
    do
    viruses[i]="$line"
    i=$(( i+1 ))   
done < $virus_file
echo  "${viruses[@]}"
#echo ${#viruses[@]}
inputfile="inputFile"
if [ -f "$inputfile" ]
then
rm $inputfile
fi
touch $inputfile
for (( i=0; i<$numlines; i++ ))
do
    temp=$(($RANDOM%10000))
    if [ $(($dups_allowed)) -eq 0 ]
    then
    if [[ " ${citizenIds[@]} " =~ " ${temp} " ]]; 
    then
    i=$(( i-1 ))
    continue   
    fi
    fi
    citizenIds[i]=$temp
    firstN[i]=$(tr -dc A-Za-z </dev/urandom | head -c $(($RANDOM% 9 + 3)) ;)
    lastN[i]=$(tr -dc A-Za-z </dev/urandom | head -c $(($RANDOM% 9 + 3)) ;)
    country[i]=${countries[$(($RANDOM% ${#countries[@]} ))]}
    ages[i]=$(($RANDOM%120))
    virus[i]=${viruses[$(($RANDOM% ${#viruses[@]} ))]}
    if [ $(($RANDOM%2)) -eq 0 ]
    then 
    yesno[i]="NO"
    date[i]=""
    else
    yesno[i]="YES"
    date[i]=" $(($RANDOM% 30 + 1 ))-$(($RANDOM% 12 +1 ))-$(($RANDOM%22 + 2000))"
    fi
done
if [ $(($dups_allowed)) -eq 1 ]
then
for (( i=1; i<$(($numlines/10)); i++ ))
do
citizenIds[$(($numlines - $i ))]=${citizenIds[$i]}
firstN[$(($numlines - $i ))]=${firstN[$i]}
lastN[$(($numlines - $i ))]=${lastN[$i]}
country[$(($numlines - $i ))]=${country[$i]}
ages[$(($numlines - $i ))]=${ages[$i]}
done
fi

for (( i=0; i<$numlines; i++ ))
do
    printf "${citizenIds[$i]} ${firstN[$i]} ${lastN[$i]} ${country[$i]} ${ages[$i]} ${virus[$i]} ${yesno[$i]}${date[$i]}" >> $inputfile
    if [ $(($i)) -ne $(($numlines-1)) ]
    then 
    echo "" >>inputFile
    fi
done

#echo "${citizenIds[@]}"
# echo "${firstN[@]}"
# echo "${lastN[@]}"
# echo "${country[@]}"
# echo "${ages[@]}"
# echo "${virus[@]}"
# echo "${virus[5000]}"
# echo "${country[5000]}"
# echo "${date[5000]}"
# echo "${yesno[@]}"
# echo "${date[@]}"


# if [[ " ${viruses[@]} " =~ " ${f} " ]]; then
#     # whatever you want to do when array contains value
#     echo fuck
# fi
