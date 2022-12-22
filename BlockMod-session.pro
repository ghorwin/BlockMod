# Project for BlockMod session

TEMPLATE=subdirs

# SUBDIRS lists all subprojects
SUBDIRS += BlockMod \
	BlockModDemo \
	BlockModTests 
	
BlockModDemo.depends = BlockMod
BlockModTests.depends = BlockMod
