# Описание на формат на CSV (текстови) файл:
# • CSV файлът представлява таблица, като всеки ред на таблицата е записан на отделен ред;
# • на даден ред всяко поле (колона) е разделено от останалите със запетая;
# • в полетата не може да присъства запетая, т.е. запетаята винаги е разделител между полета;
# • броят на полетата на всеки ред е константа;
# • първият ред във файла e header, който описва имената на колоните.
# Разполагате с два CSV файла със следното примерно съдържание:
# • base.csv:
# unit name,unit symbol,measure
# second,s,time
# metre,m,length
# kilogram,kg,mass
# ampere,A,electric current
# kelvin,K,thermodynamic temperature
# mole,mol,amount of substance
# candela,cd,luminous intensity
# • prefix.csv:
# prefix name,prefix symbol,decimal
# tera,T,1000000000000
# giga,G,1000000000
# mega,M,1000000
# mili,m,0.001
# nano,n,0.000000001
# Където смисълът на колоните е:
# • за base.csv
# – unit name – име на мерна единица
# – unit symbol – съкратено означение на мерната единица
# – measure – величина
# • за prefix.csv
# – prefix name – име на представка
# – prefix symbol – означение на представка
# – decimal – стойност
# Забележка: Във файловете може да има и други редове, освен показаните в примера. Приемаме,
# че файловете спазват описания по-горе формат, т.е. не е необходимо да проверявате формата.
# Напишете shell скрипт, който приема три задължителни параметъра: число, prefix symbol и unit
# symbol.
# Скриптът, ако може, трябва да извежда числото в основната мерна единица без представка, доба-
# вяйки в скоби величината и името на мерната единица.
# Примерен вход и изход:
# $ ./foo.sh 2.1 M s
# 2100000.0 s (time, second)
# Забележка: За изчисления може да ползвате bc

#!/bin/bash

if [ $# -ne 3 ]; then
    echo "provide 3 args"
    exit 99
fi

if [[ ! $1 =~ ^-?[0-9]*\.?[0-9]+$ ]]; then
    echo "provide as first arg number"
    exit 98
fi

base_file=$(cat "base.csv")
prefix_file=$(cat "prefix.csv")

num="$1"
prefix="$2"
unit="$3"

prefix_num=$(echo "$prefix_file" | grep ",$prefix," | awk -F ',' '{print $3}' )
base_metadata=$(echo "$base_file" | grep ",$unit," | awk -F ',' 'BEGIN{OFS=", "}{print "("$3,$1")"}')

f_num=$(echo "scale=1; $num*$prefix_num" | bc)
echo "$f_num $unit $base_metadata"

