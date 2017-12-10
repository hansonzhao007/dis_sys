#!/usr/bin/python

from operator import itemgetter
import sys

current_number = None
current_count = 0
num = None
tsum = 0
# input comes from STDIN
for line in sys.stdin:
    # remove leading and trailing whitespace
    line = line.strip()

    # parse the input we got from mapper.py
    num, count = line.split('\t', 1)

    # convert count (currently a string) to int
    try:
        count = int(count)
    except ValueError:
        # count was not a number, so silently
        # ignore/discard this line
        continue

    tsum = tsum + int(num) * count

print '==== sum is:%s ====' %(tsum)
