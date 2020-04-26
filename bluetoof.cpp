#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>


#include <glib.h>
#include <gio/gio.h>


int main(int argc, char* args[])
{
    printf("bluetoof\n");
 
    GDBusConnection* connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
    if (connection == NULL)
    {
        g_print("FFFFFFF\n");
        return 1;
    }

	GVariant* result;
	GError* error = NULL;

	result = g_dbus_connection_call_sync(connection,
		"org.bluez",
		"/org/bluez/hci0",
		"org.bluez.Adapter1",
		"StartDiscovery",
		NULL,
		NULL,
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		&error);

	if (error != NULL)
	{
		printf("OH NO\n");
	}

	g_variant_unref(result);
	return 0;

    g_object_unref(connection);
   
    return 0;
}
