#!/bin/sh

KERNEL_FILE="linux-3.8.3.tar.xz"
DUMMY_PATCH="dummy.patch"
SCHED_PATCH="sched.patch"

LINUX_SRC="linux-3.8.3"
SWAPFS="swap.img"

source ./"ostask2.config.sh"

file_exists() {
	if ! [ -e $1 ]; then
		echo "File $1 doesn't exist."
		exit 1
	fi
}

if [ `uname` != "Linux" ]; then
	echo "You need to be under linux to execute this script."
	exit 1
fi

if [ -z "$1" ]; then
	echo "usage: make, run, patch, init"
	echo "	make [defconfig | clean]    -> Launch Makefile"
	echo "	run                         -> Launch UML VM"
	echo "	patch                       -> Generate the patch file"
	echo "	init                        -> Unarchive files and dummy patch"
	echo ""
	echo "List of patched files by dummy.patch:"
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
	file_exists $ROOTFS
	file_exists $SWAPFS
	$BUILD_PATH/linux ubda="$ROOTFS" ubdb="$SWAPFS" mem=256M con0=fd:0,fd:1 con=pts

elif [ $1 = "init" ]; then
	file_exists "$KERNEL_FILE"
	file_exists "$DUMMY_PATCH"
	
	echo ">> Unarchive kernel (may take some time)"
	tar -xf "$KERNEL_FILE"
	
	echo ">> Patching kernel"
	cd "$LINUX_SRC"
	patch -p1 -i ../"$DUMMY_PATCH"
	cd ..
	
	echo ">> Duplicating kernel directory"
	cp -R "$LINUX_SRC" "$LINUX_SRC-dummy"
	echo "/!\ Do not touch '$LINUX_SRC-dummy/'... it is the base directory to generate the patch file."

elif [ $1 = "patch" ]; then
	diff -pruN "$LINUX_SRC-dummy" "$LINUX_SRC" > "$SCHED_PATCH"

fi

