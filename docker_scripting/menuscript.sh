#!/bin/bash

gett=-1
while [ $gett -ne 0 ]
do
clear
echo "MENU"
echo "----------------------------------------------------"
echo "1. Get Processor Information"
echo "2. Create a Testing Directory"
echo "3. Safe Delete"
echo "4. MinEdit : A Minimalist CLI-Based C Program Editor"
echo "0. Exit"
echo ""
echo "> Enter Your Choice: "
read gett
if [ $gett -eq 1 ]
then
	clear
	./script1.sh  
	echo "press any key to continue..."
	read any
elif [ $gett -eq 2 ]
then
	clear
	./script2.sh 
	echo "press any key to continue..."
	read any
elif [ $gett -eq 3 ]
then
	clear
	./script3.sh 
	echo "press any key to continue..."
	read any
elif [ $gett -eq 4 ]
then
# question 4 with scripting & c both 
	clear
	./script4.sh
	#./4th
	echo "press any key to continue..."
	read any 
elif [ $gett -eq 0 ]
then
	exit 0
fi
	
done

