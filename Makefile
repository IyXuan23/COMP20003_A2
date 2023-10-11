#first target
dict3: main3.o functions.o treeops_and_listops.o
	gcc -g -Wall -o dict3 main3.o functions.o treeops_and_listops.o

#2nd target
dict4: main4.o functions.o treeops_and_listops.o
	gcc -g -Wall -o dict4 main4.o functions.o treeops_and_listops.o

main3.o: main3.c 
	gcc -g -Wall -c main3.c

main4.o: main4.c
	gcc -g -Wall -c main4.c

functions.o: functions.c functions.h
	gcc -g -Wall -c functions.c

#note; the listops and treeops files were combined in order to facilitate the use of list_t and node_t types
#within treeops (sorry if there's another way that I'm unaware of to keep it in seperate files ><)
treeops_and_listops.o: treeops_and_listops.c treeops_and_listops.h
	gcc -g -Wall -c treeops_and_listops.c

