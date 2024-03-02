# ! / bin / bash
if [ $# -eq 0 ]
then 
	echo "> One Argument is Required!"
elif [ $# -gt 1 ]
then
	echo "> Only one Argument is Required!"
else 
	if [ ! -f "$1" ]
	then
	echo "> file does not exist, Try Again"
	exit 0
	fi
	
	if [ ! -d "trash" ]
	then
		mkdir trash
		chmod +w trash
	fi
	fmove="$1"
	dest="trash"
	echo "> are you sure you want to remove the file? [y / n]"
	read ch
	if [ "$ch" == "y" ]
	then
	cp "$fmove" "$dest/"
	rm $1
	echo "> Done"
	fi
fi
	
