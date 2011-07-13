#!/usr/bin/python

'''
Initialization routines for a CA.
'''

def single(width):
    '''single cell at the centre'''
    l = [0]*width
    l[width/2] = 1
    return l

def random(width):
    '''random pattern of black and white cells'''
    import random
    return [random.randint(0, 1) for i in range(width)]

def checkboard(width):
    '''checkboard pattern'''
    return [i % 2 for i in range(width)]

