#!/bin/bash

cd "$(dirname "$0")/../.." || exit

WORKDIR=lab-3/part-1/ make all &
sleep 1
pid=$(pgrep -u "$(whoami)" lab3-1)
kill -s SIGHUP "${pid}"
