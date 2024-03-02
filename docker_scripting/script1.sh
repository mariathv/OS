# ! / bin / bash
filePath="/proc/cpuinfo"

echo "================================================================"	
procsrs=$(grep -c '^processor' $filePath)
echo Total Number Of Processors : $procsrs
crs=$(grep '^core id' $filePath | sort -u | wc -l)
echo Total Number Of Physical Cores : $crs
if [ $procsrs -gt $crs ]
then
	echo "> Hyperthreading is [ENABLED]"
else
	echo "> Hyperthreading is [NOT ENABLED]"
fi
crswthyper=$(($procsrs / $crs)) 
echo Total Physical Cores with HyperThreading Enabled: $crswthyper
echo Model Name: $(grep -m 1 'model name' $filePath | awk -F ': ' '{print $2}')
echo "================================================================"	
exit 0
