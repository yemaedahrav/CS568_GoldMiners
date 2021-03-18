#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Mar 18 20:47:01 2021

@author: milind
"""

import sys

file = open(sys.argv[1])
print(sys.argv[1])
x = file.readlines()
a,b = x[-1].split()
c,d = x[-2].split()
val = (max(int(a),int(b)) + max(int(c),int(d)))/(int(a)+int(b)+int(c)+int(d))
print(str(val))