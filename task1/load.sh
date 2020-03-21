#!/bin/sh

# Script to load module and make device file automatically

module="csumdev"
device="csumdev"
mode="666"

/sbin/insmod ./$module.ko $* || exit 1

rm -f /dev/$device

major=$(awk "{if (\$2==\"$module\") print \$1}" /proc/devices)

mknod /dev/$device c $major 0

chmod $mode /dev/$device
