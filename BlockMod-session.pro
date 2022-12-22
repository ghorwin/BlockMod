# Project for BlockMod session

TEMPLATE=subdirs

# SUBDIRS lists all subprojects
SUBDIRS += BlockMod \
	BlockModDemo \
	SerializationTest \
	ShowNetworkTest

SerializationTest.file = BlockModTests/SerializationTest.pro
ShowNetworkTest.file = BlockModTests/ShowNetworkTest.pro

BlockModDemo.depends = BlockMod
SerializationTest.depends = BlockMod
ShowNetworkTest.depends = BlockMod
