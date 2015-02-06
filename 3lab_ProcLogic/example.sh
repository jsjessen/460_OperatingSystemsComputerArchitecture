#!/bin/sh

vm="qemu-system-i386"
kc_image="disk/mtximage"

$vm -fda $kc_image -no-fd-bootchk #-localtime -serial mon:stdio
