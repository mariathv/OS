# ! / bin / bash
choice=-1
while [ $choice -ne 0 ]
do
clear
echo "=================================================="
echo "1. Create New File"
echo "2. Edit File"
echo "0. Exit"
echo "=================================================="
echo ""
echo "> Enter Your Choice: "; read gett

if [ $gett -eq 1 ]
then
	clear
	echo "> Enter File Name: "
	read anyf
	if [ -f $anyf.c ]
	then
		echo "> file already exists"
		echo "> press enter to continue..."
		read getch
	else
		touch $anyf.c
		echo "> file created successfully"
		echo "> press enter to continue..."
		read getch
	fi
elif [ $gett -eq 2 ]
then 
	clear
	if ! ls *.c >/dev/null 2>&1; then
    	echo "No shell script files (*.c) found in the current directory, create one."
	fi
	echo "> Enter File Name: "
	read anyf
	if [ ! -f $anyf.c ] 
	then
		echo "> file not found, creating one..."
		touch $anyf.c
	fi
	clear
	echo "Shortcuts (esc+) :s (save) :x (exit) :!c (compile) :e (execute)"
	echo "Source File::"
	
	
	cinput=$(<"$anyf.c")
	echo ""
	echo "=========================================="
	echo""
	echo -n "$cinput"
	while true; do
    	IFS= read -rsn 1 key
    	if [ "$key" == $'\x7f' ] #backspace check 
    	then
		if [ "${#cinput}" -gt 0 ]; then
		
		    cinput="${cinput%?}" #echo back (removes prev char)
		    echo -ne "\b \b"
		
			
		fi
	elif [ "$key" == $"" ] #enter key check
	then  
		echo ""
		cinput+=$'\n' 
	elif [ "$key" == $'\x1b' ]
	then  #ascii for esc key 21 == 1b in hexa
		IFS= read -rsn 1 key1
		IFS= read -rsn 1 key2
		if [ "$key1" == ":" ] && [ "$key2" == "s" ]; then
			echo "$cinput" > $anyf.c
			echo ":: saved!"
		elif [ "$key1" == ":" ] && [ "$key2" == "x" ]; then
			echo "> Exit? [y/n]"
			read -rsn 1 confirm
			if [ "$confirm" == "y" ]
			then
		    	break
		    	fi
		elif [ "$key1" == ":" ] && [ "$key2" == "!" ]; then
			IFS= read -rsn 1 key3
			if [ "$key3" == "c" ]
			then
				if ! command -v gcc &> /dev/null; then
				    echo "> gcc is not installed. Installing..."
				    sudo apt update
				    sudo apt install gcc
				    if [ $? -eq 0 ]; then
					echo "gcc installed successfully"
				    else
					echo "Failed to install gcc"
				    fi
				elif  g++ -o $anyf $anyf.c 
				then
					echo ":: code compiled"
				else
					echo ":: errors"
				fi
			fi
		elif [ "$key1" == ":" ] && [ "$key2" == "e" ]; then
			echo ""
			echo ""
			if [ ! -f $anyf ]
			then
				echo "> No Executable file found"
			else
			./$anyf
			fi
		fi
		
	else
	cinput+="$key"
	echo -n "$key"
	fi

       done
       echo "$cinput" > $anyf.c
       echo "> press enter to continue..."
       read getch
	
elif [ $gett -eq 0 ]
then
	exit 0
fi
done
