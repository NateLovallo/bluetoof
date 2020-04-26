#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gio/gio.h>

#include <assert.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Manager for interacting with bluez using an OO class
//
// This borrows heavily from:
// https://www.linumiz.com/bluetooth-adapter-scan-for-new-devices-using-startdiscovery/
// with my own OO sauce on top
class BluetoothManager
{
public:
	//////////////////////////////////////////////////////////////////////////////
	BluetoothManager()
	{
		mConnection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
		if (mConnection == NULL)
		{
			g_print("FFFFFFF\n");
			assert(false);
		}

		//mLoop = g_main_loop_new(NULL, FALSE);
	}

	//////////////////////////////////////////////////////////////////////////////
	void Discover()
	{
		guint interfaceAdded = g_dbus_connection_signal_subscribe(
			mConnection,
			"org.bluez",
			"org.freedesktop.DBus.ObjectManager",
			"InterfacesAdded",
			NULL,
			NULL,
			G_DBUS_SIGNAL_FLAGS_NONE,
			DeviceAppearedCb,
			this,
			NULL);

		unsigned int seconds = 0;
		InvokeG("StartDiscovery");

		for (int seconds = 0; seconds < 10; seconds++)
		{ 
			g_main_context_iteration(NULL, FALSE);
			g_usleep(1000000);
			g_thread_yield();
		}

		g_dbus_connection_signal_unsubscribe(mConnection, interfaceAdded);
		InvokeG("StopDiscovery");
		printf("Found %u devices! in %u seconds\n", mDevices.size(), seconds);

	}

	//////////////////////////////////////////////////////////////////////////////
	~BluetoothManager()
	{
		g_object_unref(mConnection);
	}



private:

	//////////////////////////////////////////////////////////////////////////////
	bool InvokeG(const char* method)
	{
		bool retval = true;
		GError* error = NULL;
		g_dbus_connection_call_sync(mConnection,
			"org.bluez",
			"/org/bluez/hci0",
			"org.bluez.Adapter1",
			method,
			NULL,
			NULL,
			G_DBUS_CALL_FLAGS_NONE,
			-1,
			NULL,
			&error);

		if (error != NULL)
		{
			printf("OH NO\n");
			retval = false;
		}
		return retval;
	}

	//////////////////////////////////////////////////////////////////////////////
	static void PropertyGetter(const gchar* key, GVariant* value)
	{
		const gchar* type = g_variant_get_type_string(value);

		g_print("\t%s : ", key);
		switch (*type) 
		{
			case 'o':
			case 's':
				g_print("%s\n", g_variant_get_string(value, NULL));
				break;
			case 'b':
				g_print("%d\n", g_variant_get_boolean(value));
				break;
			case 'u':
				g_print("%d\n", g_variant_get_uint32(value));
				break;
			case 'a':
				// TODO Handling only 'as', but not array of dicts 
				if (g_strcmp0(type, "as"))
					break;
				g_print("\n");
				const gchar* uuid;
				GVariantIter i;
				g_variant_iter_init(&i, value);
				while (g_variant_iter_next(&i, "s", &uuid))
				{
					g_print("\t\t%s\n", uuid);
				}
				break;
			default:
				g_print("Other\n");
				break;
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	static void DeviceAppearedCb(
		GDBusConnection* sig,
		const gchar* sender,
		const gchar* path,
		const gchar* interface,
		const gchar* signal,
		GVariant* param,
		gpointer userData)
	{
		GVariantIter* interfaces;
		const char* object;
		const gchar* interface_name;
		GVariant* properties;

		assert(userData != NULL);
		BluetoothManager* me = static_cast<BluetoothManager*>(userData);

		g_variant_get(param, "(&oa{sa{sv}})", &object, &interfaces);
		while (g_variant_iter_next(interfaces, "{&s@a{sv}}", &interface_name, &properties)) 
		{
			if (g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "device")) 
			{
				g_print("[ %s ]\n", object);
				const gchar* propertyName;
				GVariantIter i;
				GVariant* propertyValue;
				g_variant_iter_init(&i, properties);
				while (g_variant_iter_next(&i, "{&sv}", &propertyName, &propertyValue))
				{ 
					PropertyGetter(propertyName, propertyValue);
				}
				g_variant_unref(propertyValue);


				me->mDevices.push_back(std::string(object));
			}

			g_variant_unref(properties);
		}
		return;
	}

	GDBusConnection* mConnection = nullptr;
	//GMainLoop* mLoop = ;
	std::vector <std::string> mDevices;
};
