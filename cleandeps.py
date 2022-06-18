#! /usr/bin/env python

import os

folders = os.popen('find ./ -name ".deps"').read().splitlines()
for folder in folders:
	print "cleaning up", folder
	os.system("rm -R "+folder)
