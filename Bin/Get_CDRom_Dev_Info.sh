#!/bin/sh
DEV_CNT=1
SYS_DEV_INFO=`cat /proc/sys/dev/cdrom/info | grep "drive name"`
[ -f CDRom_List ] && rm CDRom_List && touch CDRom_List

for x in $SYS_DEV_INFO
	do
		echo $x
		if [ -b /dev/cdrom-$x ] ; then
			echo "<dev$DEV_CNT>/dev/cdrom-$x</dev$DEV_CNT>" >> CDRom_List
			let DEV_CNT++
		elif [ -b /dev/$x ] ; then
			echo "<dev$DEV_CNT>/dev/$x</dev$DEV_CNT>" >> CDRom_List
			let DEV_CNT++
		fi

	done
