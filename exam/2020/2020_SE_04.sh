# Напишете скрипт, който приема два аргумента - имена на директории. Първата
# (SRC ) съществува, докато втората (DST ) трябва да бъде създадена от скрипта. Директорията SRC
# и нейните поддиректории може да съдържат файлове, чиито имена завършат на .jpg. Имената на
# файловете може да съдържат интервали, както и поднизове, оградени със скоби, например:
# A single (very ugly) tree (Outdoor treks) 2.jpg
# Falcons.jpg
# Gorgonzola (cheese).jpg
# Leeches (two different ones) (Outdoor treks).jpg
# Pom Pom Pom.jpg
# За даден низ ще казваме, че е почистен, ако от него са премахнати leading и trailing интервалите
# и всички последователни интервали са сведени до един.
# За всеки файл дефинираме следните атрибути:
# • заглавие - частта от името преди .jpg, без елементи оградени в скоби, почистен. Примери:
# A single tree 2
# Falcons
# Gorgonzola
# Leeches
# Pom Pom Pom
# • албум - последният елемент от името, който е бил ограден в скоби, почистен. Ако албум е
# празен стринг, ползваме стойност по подразбиране misc. Примери:
# Outdoor treks
# misc
# cheese
# Outdoor treks
# misc
# • дата - времето на последна модификация на съдържанието на файла, във формат YYYY-
# MM-DD
# • хеш - първите 16 символа от sha256 сумата на файла. Забележка: приемаме, че в тази идеална
# вселена първите 16 символа от sha256 сумата са уникални за всеки файл от тези, които
# ще се наложи да обработваме.
# Скриптът трябва да създава в директория DST необходимата структура от под-директории, фай-
# лове и symlink–ове, така че да са изпълнени следните условия за всеки файл от SRC :
# • DST /images/хеш.jpg - копие на съответния файл
# • следните обекти са относителни symlink–ове към хеш.jpg:
# – DST /by-date/дата/by-album/албум/by-title/заглавие.jpg
# – DST /by-date/дата/by-title/заглавие.jpg
# – DST /by-album/албум/by-date/дата/by-title/заглавие.jpg
# – DST /by-album/албум/by-title/заглавие.jpg
# – DST /by-title/заглавие.jpg

#!/bin/bash

src="$1"
dst="$2"

function clr_inp {
   echo "$1" | sed -e 's/^ \+//g' -e 's/ \+$//g' -e 's/ \+/ /g'
}

function extr_header {
    tmp=$(echo "$1" | sed 's/([^)]*)//g')
    clr_inp "$tmp"
}

function extr_album {
    tmp=$(echo "$1" | sed -n 's/[^()]*\(([^)]*)\)[^()]*/\1 /gp' | sed 's/)\ (/:/g')
    tmp2=$(clr_inp "$tmp" | cut -c 2- | rev | cut -c 2- | rev | awk -F ':' '{print $NF}')
    
    if [ -z "$tmp2"  ]; then
        echo "misc"
    else
        clr_inp "$tmp2"
    fi
}    

if [ $# -ne 2 ]; then
    echo "provide 2 args"
    exit 99
fi

if [ ! -d "$src"  ];then 
    echo "src should be dir"
    eixt 98
fi

mkdir -p "$dst/by-date"
mkdir -p "$dst/by-album"
mkdir -p "$dst/by-title"

while read -r jpg; do
    name=$(basename "$jpg" .jpg)
    header=$(extr_header "$name")
    album=$(extr_album "$name")
    date=$(stat -c '%y' "$jpg" | cut -c -10)
    hash=$(sha256sum "$jpg" | awk '{print $1}' | cut -c -16)

    trg1="$dst/by-date/$date/by-album/$album/by-title"
    trg2="$dst/by-date/$date/by-title"
    trg3="$dst/by-album/$album/by-date/$date/by-title"
    trg4="$dst/by-album/$album/by-title"
    trg5="$dst/by-title"

    for trg in "$trg1" "$trg2" "$trg3" "$trg4" "$trg5"; do
        mkdir -p "$trg"
        cp "$jpg" "$trg/$header.jpg"
    done
done < <(find "$src" -type f -name "*.jpg")
