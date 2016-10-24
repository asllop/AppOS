#!/bin/sh

make clean
make

mkdir -p isodir/boot/grub
cp build/appos.elf isodir/boot/
cp grub.cfg isodir/boot/grub/
grub-mkrescue -o appos.iso isodir

