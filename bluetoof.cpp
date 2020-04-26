#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gio/gio.h>

#include "bluetoof.h"

int main(int argc, char* args[])
{
	printf("bluetoof\n");

	BluetoothManager bMan;

	bMan.Discover();
 

	return 0;
}
