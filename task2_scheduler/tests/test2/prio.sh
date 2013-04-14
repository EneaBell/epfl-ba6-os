#!/bin/sh

echo 2 > /proc/sys/kernel/sched_dummy_timeslice
echo 6 > /proc/sys/kernel/sched_dummy_age_threshold

nice -n 16 ../loop/loop A &
nice -n 17 ../loop/loop B &
nice -n 18 ../loop/loop C &

wait
echo

