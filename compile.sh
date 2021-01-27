#!/bin/bash

rm -f *.dbg

gcc -DPRINTALL -Wall -ggdb3 psubscribe.c -lhiredis -levent -o printall.dbg
gcc            -Wall -ggdb3 psubscribe.c -lhiredis -levent -o events.dbg
