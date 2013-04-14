#!/bin/sh

echo 5    > /proc/sys/kernel/sched_dummy_timeslice
echo 1000 > /proc/sys/kernel/sched_dummy_age_threshold

nice -n 19 ./preem

