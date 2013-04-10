#!/bin/sh

LINUX_SRC="linux-3.8.3"
BUILD_PATH="/home/thmx/task2_scheduler"
ROOTFS="Gentoo-10.0-x86-root_fs"
SWAPFS="swap.img"

if [ -z "$1" ]; then
	echo "usage: make, run, patch"
	echo "	make [defconfig | clean]    -> Launch Makefile"
	echo "	run                         -> Launch UML VM"
	echo "	patch                       -> Generate the patch file"
	echo ""
	echo "Patched files by dummy.patch:"
	echo "	+ include/linux/init_task.h"
	echo "	+ include/linux/sched.h"
	echo "	+ kernel/sched/core.c"
	echo "	+ kernel/sched/dummy.c"
	echo "	+ kernel/sched/fair.c"
	echo "	+ kernel/sched/Makefile"
	echo "	+ kernel/sched/sched.h"
	echo "	+ kernel/sysctl.c"

elif [ $1 = "make" ]; then
	cd "$LINUX_SRC"
	make ARCH=um O="$BUILD_PATH" $2

elif [ $1 = "run" ]; then
	"$BUILD_PATH"/linux ubda="$ROOTFS" ubdb="$SWAPFS" mem=256M con0=fd:0,fd:1 con=pts

elif [ $1 = "patch" ]; then
	diff -pruN "$LINUX_SRC-dummy" "$LINUX_SRC" > sched.patch

elif [ $1 = "init" ]; then
	echo "TODO"

fi