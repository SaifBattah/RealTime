all: parent sender rec spy enc helper spicatcher

parent: parent.c
	gcc -o parent parent.c -pthread

sender: sender.c
	gcc -o sender sender.c

rec: rec.c
	gcc -o rec rec.c

spy: spy.c
	gcc -o spy spy.c -pthread

enc: enc.c
	gcc -o enc enc.c -pthread

helper: helper.c
	gcc -o helper helper.c -pthread

spicatcher: spicatcher.c
	gcc -o spicatcher spicatcher.c -pthread
clean:
	del parent sender rec spy enc helper spicatcher
