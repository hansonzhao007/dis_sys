#!/usr/bin/python

import sys
num_cnts = dict()
# input comes from STDIN (standard input)
for line in sys.stdin:
    #for each document create dictionary of numbers
    line = line.strip()
    nums = line.split()
    for num in nums:
        if num not in num_cnts.keys(): num_cnts[num] = 1
        else: num_cnts[num] += 1
        
# emit key-value pairs only for distinct numbers per document
for w in num_cnts.keys():
    print '%s\t%s' % (w,num_cnts[w])
