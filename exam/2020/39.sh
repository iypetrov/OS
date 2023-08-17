# Под пакет ще разбираме директория, която има следната структура:
# <name>
# |-- version
# |-- tree
# |...
# Където <name> е името на пакета, version е текстов файл, който съдържа низ от вида 1.2.3-4 и
# нищо друго, а tree е директория с произволно съдържание.
# За да получим архив на пакет, архивираме (tar) и компресираме (xz) съдържанието на директо-
# рията tree.
# Под хранилище ще разбираме директория, която има следната структура:
# <repo name>
# |-- db
# |-- packages
# |...
# Където <repo name> е името на хранилището, db е текстов файл, чиито редове имат вида <package
# name>-<package version> <package checksum> и са сортирани лексикографски. Директорията packages
# съдържа архиви с имена <package checksum>.tar.xz, които съответстват на редове в db. Под
# <package checksum> имаме предвид sha256 сумата на архива на пакета.
# Напишете скрипт repo_add.sh, който приема два аргумента - път до хранилище и път до пакет,
# който добавя пакета в хранилището. Ако същата версия на пакет вече съществува, архивът се
# заменя с новата версия. В противен случай, новата версия се добавя заедно с другите.
# Заб: Първо си проектирайте общия алгоритъм на работа.
# Примерно хранилище:
# myrepo
# |-- db
# |-- packages
# |-- 6e3549438bc246b86961b2e8c3469321ca22eabd0a6c487d086de7a43a0ef766.tar.xz
# |-- 66b28e48161ba01ae25433b9ac4086a83b14d2ee49a62f2659c96514680ab6e8.tar.xz
# |-- 99c934ad80bd9e49125523c414161e82716b292d4ed2f16bb977d6db7e13d9bc.tar.xz
# Със съдържание на db:
# glibc-2.31-2 6e3549438bc246b86961b2e8c3469321ca22eabd0a6c487d086de7a43a0ef766
# zlib-1.1.15-8 66b28e48161ba01ae25433b9ac4086a83b14d2ee49a62f2659c96514680ab6e8
# zlib-1.2.11-4 99c934ad80bd9e49125523c414161e82716b292d4ed2f16bb977d6db7e13d9bc
# Примерен пакет:
# zlib
# |-- version # contains ’1.2.11-3’
# |-- tree
# |...
# Съдържание на хранилището след изпълнение на ./repo-add.sh myrepo zlib
# myrepo
# |-- db
# |-- packages
# |-- 6e3549438bc246b86961b2e8c3469321ca22eabd0a6c487d086de7a43a0ef766.tar.xz
# |-- 66b28e48161ba01ae25433b9ac4086a83b14d2ee49a62f2659c96514680ab6e8.tar.xz
# |-- b839547ee0aed82c74a37d4129382f1bd6fde85f97c07c5b705eeb6c6d69f162.tar.xz
# |-- 99c934ad80bd9e49125523c414161e82716b292d4ed2f16bb977d6db7e13d9bc.tar.xz
# Със съдържание на db:
# glibc-2.31-2 6e3549438bc246b86961b2e8c3469321ca22eabd0a6c487d086de7a43a0ef766
# zlib-1.1.15-8 66b28e48161ba01ae25433b9ac4086a83b14d2ee49a62f2659c96514680ab6e8
# zlib-1.2.11-3 b839547ee0aed82c74a37d4129382f1bd6fde85f97c07c5b705eeb6c6d69f162
# zlib-1.2.11-4 99c934ad80bd9e49125523c414161e82716b292d4ed2f16bb977d6db7e13d9bc

#!/bin/bash

function build_name_archive {
    echo "$1.tar.xz"
}

function add_to_db {
    db="$1/db"
    packages="$1/packages"
    
    res=$(cat "$db" | grep -n -E "^$2-$3")
    res_num_parts=$(echo "$res" | awk -F ':' '{print NF}')
    if [ $res_num_parts==2  ]; then
        match_num_line=$(echo "$res" | cut -d ':' -f 1)
        sed -i "${match_num_lined}d" "$db"

        #name_removed_archive=$(build_name_archive "$4")
        #rm -rf "$packages/$name_removed_archive"
    fi

    echo "$2-$3 $4" >> "$db"
    sort "$db" -o "$db"
}

function add_to_packages {
   packages="$1/packages"
   tmp=$(build_name_archive "tmp")
   tar cf "tmp.tar" "$2"
   xz -z "tmp.tar"
   checksum=$(sha256sum "$tmp" | awk '{print $1}')
   name_checksum=$(build_name_archive "$checksum")
   mv "$tmp" "$packages/$name_checksum"
   echo "$checksum"
}

if [ "$#" -ne 2  ]; then
    echo "provide 2 args"
    exit 99
fi

if [ ! -d "$1"  ]; then
    echo "provide as first arg repo"
    exit 98
fi

if [ ! -d "$2" ]; then 
    echo "provide as second arg pckt"
    exit 97
fi


repo="$1"
pckt_name="$2"
pckt_ver=$(cat "$2/version")

if [ $(find "$repo" -mindepth 1 -type f -name "db" | wc -l) -ne 1 ]; then
    echo "Invalid repo"
    exit 96
fi

if [ $(find "$repo"  -mindepth 1 -type d -name "packages" | wc -l) -ne 1 ]; then
    echo "Invalid repo"
    exit 95
fi

if [ $(find "$pckt_name" -mindepth 1 -type f -name "version" | wc -l) -ne 1 ]; then
    echo "Invalid package"
    exit 94
fi

if [ $(find "$pckt_name" -mindepth 1 -type d -name "tree" | wc -l) -ne 1 ]; then
    echo "Invalid package"
    exit 93
fi

checksum=$(add_to_packages "$repo" "$pckt_name")
add_to_db "$repo" "$pckt_name" "$pckt_ver" "$checksum"
