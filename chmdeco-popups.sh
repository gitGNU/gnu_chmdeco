#!/bin/sh
# You need to run this with sh - the Bourne shell

# Decompile
chmdeco $1

# Enter the directory
cd $1

{
	# Print the section name
	/bin/echo -en \\n[TEXT\ POPUPS]\\n;

	# Find text files with the required strings in them
	find -iname \*.txt | xargs grep -l '^[[:space:]]*\.topic[[:space:]]\+[_a-zA-Z0-9]\+'

# Remove the ./, Convert / to \, Convert \n to \r\n and Append to the hhp.
} | sed -e 's/[.]\///g' | tr / '\\' | gawk 'BEGIN { ORS = "\r\n" } { print }' >> `find \#recreated -iname \*.hhp`
