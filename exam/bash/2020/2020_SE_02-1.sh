# Даден е текстовият файл spacex.txt, който съдържа информация за полети на
# ракети на SpaceX. На всеки ред има информация за един такъв полет в следните колони, разделени
# с вертикална черта ’|’:
# • дата на полета в UNIX формат
# • космодрум
# • успешност на полета (две възможни стойности)
# – Success - успешен полет
# – Failure - неуспешен полет
# • полезен товар
# Първият ред във файла e header, който описва имената на колоните. Данните във файла не са
# сортирани.
# Намерете и изведете разделени с двоеточие (’:’) успешността и информацията за полезния товар
# на най-скорощния полет, който е изстрелян от космодрума с най-много неуспешни полети.
# Примерен входен файл:
# date|launch site|outcome|payload
# 1291759200|CCAFS|Success|Dragon demo flight and cheese
# 1435438800|CCAFS|Failure|SpaceX CRS-7
# 1275666300|CCAFS|Success|Dragon Spacecraft Qualification Unit
# 1452981600|VAFB|Success|Jason-3
# 1498165200|KSC|Success|BulgariaSat-1
# 1473454800|CCAFS|Failure|Amos-6
# 1517868000|KSC|Success|Elon Musk’s Tesla
# 1405285200|CCAFS|Success|Orbcomm
# 
#!/bin/bash

if [ $# -ne 1 ]; then
	echo "provide 1 arg"
	exit 99
fi

if [ ! -f $1 ]; then
	echo "the arg should be a file"
	exit 98
fi

data=$(tail -n +2 "$1" | sort -t '|' -nr | tr "|" ":")
cos=$(echo -e "$data" | cut -d ':' -f 2,3 | sort | uniq -c | sort -nr | head -n 1 | sed 's/\t/ /g; s/ \+/ /g' | cut -d ' ' -f 3 | cut -d ':' -f 1)
while read -r w; do
	curr=$(echo $w | cut -d ':' -f 2)
	if [[ "$curr" == "$cos" ]]; then
		echo "$w"
		exit 0
	fi
done < <(echo "$data")
