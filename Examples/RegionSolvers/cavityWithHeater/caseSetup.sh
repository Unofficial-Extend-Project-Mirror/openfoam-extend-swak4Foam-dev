#! /bin/bash

pyFoamRunner.py --auto funkySetFields -time 0 -create -field copper -region bottomHeater -expression "pos().x>0.05"
pyFoamRunner.py --auto funkySetFields -time 0 -create -field alumina -region bottomHeater -expression "1-copper"
