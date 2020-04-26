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



static void hark_a_device(GDBusConnection* sig,
	const gchar* sender_name,
	const gchar* object_path,
	const gchar* interface,
	const gchar* signal_name,
	GVariant* parameters,
	gpointer user_data)
{
	printf("HARK\n");
}



int main(int argc, char* args[])
{
	printf("bluetoof\n");
 
	GDBusConnection* connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
	if (connection == NULL)
	{
		g_print("FFFFFFF\n");
		return 1;
	}

	GMainLoop* loop = g_main_loop_new(NULL, FALSE);;
	GVariant* result;
	GError* error = NULL;
	guint iface_added;

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


	iface_added = g_dbus_connection_signal_subscribe(
		connection,
		"org.bluez",
		"org.freedesktop.DBus.ObjectManager",
		"InterfacesAdded",
		NULL,
		NULL,
		G_DBUS_SIGNAL_FLAGS_NONE,
		hark_a_device,
		loop,
		NULL);



	g_main_loop_run(loop);
	result = g_dbus_connection_call_sync(connection,
		"org.bluez",
		"/org/bluez/hci0",
		"org.bluez.Adapter1",
		"StopDiscovery",
		NULL,
		NULL,
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		&error);

	g_variant_unref(result);

	g_object_unref(connection);
   
	return 0;
}
