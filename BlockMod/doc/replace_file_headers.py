#!/usr/bin/python

# replace_file_headers.py
# Copyright 2016 Andreas Nicolai <andreas.nicolai -[at]- tu-dresden.de>
#
# Script to automatically replace the license headers of .h and .cpp files 
# with own LICENSE text. Also adjusts include guards to IBK-Standard.
#
# Syntax: replace_file_headers.py <directory>
#
# Expected to be run from doc directory, relative ../src to source files.
# Expects the file LICENSE to be present in the current directory.

import sys
import os
import glob

# The prefix expected prior to class name when composing filenames
NAMESPACE_PREFIX = "BLOCKMOD_"
FILENAME_PREFIX = "BM_"

def process_header(hdr, licenseText):
	# open file and read all lines
	try:
		hdr_file = open(hdr, 'r')
		hdr = hdr.replace('\\', '/')
		# print '  ' + hdr
		lines = hdr_file.readlines()
		# compose header guard
		hdr_guard = hdr[hdr.rfind('/')+1:-2] + 'H'
		# print hdr_guard
		
		# now process all lines and check if there is a class declaration in the file
		className = ""
		for line in lines:
			if len(line.strip()) == 0:
				continue
			# class declaration found if line is ended with a {
			pos = line.find("class")
			if pos != -1 and line.strip()[-1] == '{':
				pos2 = line.find(':', pos)
				if pos2 == -1:
					pos2 = line.find('{', pos)
				if pos2 != -1:
					className = line[pos+6:pos2].strip()
					break
		if len(className) != 0:
			# check if classname and hdr_guard match
			if FILENAME_PREFIX + className + 'H' != hdr_guard:
				print 'WARNING: mismatch of class name "'+className+'" ('+FILENAME_PREFIX + className + ') and file name for file: '''+ hdr+''
				
		# now create new file content
		new_content = ""
		have_header = False
		skip_next_define = False
		for line in lines:
			if line[-1] == '\n':
				line = line[0:-1]
			if not have_header:
				if line.find('#ifndef') == -1: 
					continue
				have_header = True
				# now insert license
				new_content = licenseText + '\n'
				# insert header guards
				new_content = new_content + '#ifndef ' + hdr_guard + '\n'
				new_content = new_content + '#define ' + hdr_guard + '\n'
				skip_next_define = True
			else:
				if skip_next_define and line.find('#define') != -1:
					skip_next_define = False
					continue
				new_content = new_content + line + '\n'
		# now substitute closing hdr_guard
		pos = new_content.rfind('#endif')
		new_content = new_content[0:pos] + '#endif // ' + hdr_guard + '\n'
		# write content
		hdr_file.close()
		hdr_file = open(hdr, 'w')
		hdr_file.write(new_content)
	except IOError as (errno, strerror):
		print 'Cannot open file (check permissions)', hdr
		print "I/O error({0}): {1}".format(errno, strerror)

def process_cpp(cpp, licenseText):
	# open file and read all lines
	try:
		cpp_file = open(cpp, 'r')
		cpp = cpp.replace('\\', '/')
		# print '  ' + cpp
		lines = cpp_file.readlines()
				
		# now create new file content
		new_content = ""
		have_header = False
		for line in lines:
			if line[-1] == '\n':
				line = line[0:-1]
			if not have_header:
				if line.find('#include') == -1:
					continue
				have_header = True
				# now insert license
				new_content = licenseText + '\n'
				# insert header guards
				new_content = new_content + line + '\n'
			else:
				new_content = new_content + line + '\n'
		# write content
		cpp_file.close()
		cpp_file = open(cpp, 'w')
		cpp_file.write(new_content)
	except IOError as (errno, strerror):
		print 'Cannot open file (check permissions)', hdr
		print "I/O error({0}): {1}".format(errno, strerror)



# *** MAIN PROGRAM ***

# read LICENSE file
fobj = open('LICENSE')
licenseTextLines = fobj.readlines()
fobj.close()
del fobj

licenseText = "/*"
for line in licenseTextLines:
	if line != "\n":
		licenseText = licenseText + '\t' + line
	else:
		licenseText = licenseText + line
licenseText = licenseText + "*/\n"

# glob all header files in the directory
root = "../src"
print 'Processing files in directory: ' + root
headers = glob.glob(root + "/*.h");
# process all files
for hdr in headers:
	process_header(hdr,licenseText)
	
cpps = glob.glob(root + "/*.cpp");
# process all files
for cpp in cpps:
	process_cpp(cpp,licenseText)
	
