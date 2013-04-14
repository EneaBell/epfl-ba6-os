#!/bin/sh

echo 3 > /proc/sys/kernel/sched_dummy_timeslice
echo 3 > /proc/sys/kernel/sched_dummy_age_threshold

nice -n 17 ../loop/loop A &
nice -n 17 ../loop/loop B &
nice -n 17 ../loop/loop C &

wait
echo

