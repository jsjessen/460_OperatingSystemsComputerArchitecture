#!/bin/sh

qemu-system-i386 -hda vdisk -no-fd-bootchk -localtime -serial mon:stdio
