2048
====

2048 puzzle game for the command-line interface written in C.

The "sem182" Semaphore library (https://github.com/osue-tuwien/sem182) is used and must be installed.

2048-server [-p power_of_two]
	-p: Play until 2ˆpower_of_two is reached (default: 11)

2048-client [-n | -u <id>]
	-n: Start a new game
	-u: Connect to existing game with the given id
