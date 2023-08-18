# Под конфигурационен файл ще разбираме файл, в който има редове от вида key=value,
# където key и value могат да се състоят от букви, цифри и знак “долна черта” (“_”). Освен това,
# във файла може да има празни редове; може да има произволен whitespace в началото и в края на
# редовете, както и около символа “=”. Също така са допустими и коментари в даден ред: всичко след
# символ “#” се приема за коментар.
# Под <date> ще разбираме текущото време, върнато от командата date без параметри; под <user>
# ще разбираме името на текущият потребител.
# Напишете shell скрипт set_value.sh, който приема 3 позиционни аргумента – име на конфигура-
# ционен файл, ключ (foo) и стойност (bar). Ако ключът:
# • присъства във файла с друга стойност, скриптът трябва да:
# – да закоментира този ред като сложи # в началото на реда и добави в края на реда #
# edited at <date> by <user>
# – да добави нов ред foo = bar # added at <date> by <user> точно след стария ред
# • не присъства във файла, скриптът трябва да добави ред от вида foo = bar # added at
# <date> by <user> на края на файла
# Примерен foo.conf:
# # route description
# from = Sofia
# to = Varna # my favourite city!
# type = t2_1
# Примерно извикване:
# ./set_value.sh foo.conf to Plovdiv
# Съдържание на foo.conf след извикването:
# # route description
# from = Sofia
# # to = Varna # my favourite city! # edited at Tue Aug 25 15:48:29 EEST 2020 by human
# to = Plovdiv # added at Tue Aug 25 15:48:29 EEST 2020 by human
# type = t2_1

#!/bin/bash

if [ $# -ne 3 ]; then
    echo "provide 3 args"
    exit 99
fi

if [ ! -f "$1"  ]; then
    echo "first arg should be a file name"
    exit 98
fi

conf="$1"
key="$2"
value="$3"

user=$(whoami)
date=$(date)

info_key=$(cat "$conf" | sed -e 's/^ \+//g' -e 's/ \+$//g' -e 's/ \+/ /g' | grep -n -E "^#? ?\b$key = \b*")
if [ ! -z "$info_key" ]; then
    num_line=$(echo "$info_key" | awk -F ':' '{print $1}')
    info_line=$(echo "$info_key" | awk -F ':' '{print $2}')
    key_line=$(echo "$info_line" | awk -F '=' '{print $2}')

    if [[ $key_line == *#* ]]; then
        key_line=$(echo "$key_line" | cut -d '#' -f 1)
    fi

    key_line=$(echo "$key_line" | sed -e 's/^ \+//g' -e 's/ \+$//g')

    sed -i "s/$key_line/$value/g" "$conf"
    tmp=$(sed -n "${num_line}p" "$conf")
    sed -i "${num_line}c\\$tmp # edited at $date by $user" $conf
else
    echo "$key = $value # added at $date by $user" >> "$conf"
fi

