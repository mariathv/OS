# ! / bin / bash
echo "> enter the number of files:"
read numf
if [ $numf -eq 0 ]
then 
	echo "> atleast one file is required!"
	exit 0
fi
if [ ! -d "TestData" ]
then
mkdir TestData
fi
cd TestData
count=1
while [ $count -le $numf ]
do
	fName="F$count.txt"
	echo "Input Sentence [$fName] : "
	read input
	echo "$input" > "$fName"
	((count++))
	
	echo "> successfully created $numf file(s)"
done
