#!/bin/sh
# Лабораторная работа 1. Файлы и каталоги.
# Скрипт создаёт тот же каталог, что и программа create.
#
# a/b/cat.txt   ("catts")
#  /c/dog.txt   (пустой)
#  /d           (ссылка на /c)
#  /e/f.txt     (жёсткая ссылка на b/cat.txt)
#    /g.bin     (500 байт, случайный)
#    /h.bin     (35 байт, заполнен 0)

set -e

mkdir a
cd a
mkdir b c e
ln -s c d

cd b
echo catts > cat.txt

cd ../c
touch dog.txt

cd ../e
ln ../b/cat.txt f.txt
head -c 500 /dev/urandom > g.bin
truncate -s 35 h.bin

cd ../..
