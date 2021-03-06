#!/usr/bin/python

from __future__ import absolute_import, print_function, unicode_literals

from gi.repository import GObject
from gi.repository import GLib

import sys
import dbus
import dbus.service
import dbus.mainloop.glib
import gi

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

AGENT_INTERFACE = 'org.bluez.Agent1'
AGENT_PATH = "/test/agent"

bus = None
device_obj = None
dev_path = None

def set_trusted(path):
    props = dbus.Interface(bus.get_object("org.bluez", path), "org.freedesktop.DBus.Properties")
    props.Set("org.bluez.Device1", "Trusted", True)

class Agent(dbus.service.Object):
    exit_on_release = True

    def set_exit_on_release(self, exit_on_release):
        self.exit_on_release = exit_on_release

    @dbus.service.method(AGENT_INTERFACE, in_signature="", out_signature="")
    def Release(self):
        print("Release")
        if self.exit_on_release:
            mainloop.quit()

    @dbus.service.method(AGENT_INTERFACE, in_signature="os", out_signature="")
    def AuthorizeService(self, device, uuid):
        print("AuthorizeService (%s, %s)" % (device, uuid))
        set_trusted(device)
        return

    @dbus.service.method(AGENT_INTERFACE, in_signature="o", out_signature="s")
    def RequestPinCode(self, device):
        print("RequestPinCode (%s)" % (device))
        set_trusted(device)
        return "0000"

    @dbus.service.method(AGENT_INTERFACE, in_signature="o", out_signature="u")
    def RequestPasskey(self, device):
        print("RequestPasskey (%s)" % (device))
        set_trusted(device)
        return dbus.UInt32("0000")

    @dbus.service.method(AGENT_INTERFACE, in_signature="ouq", out_signature="")
    def DisplayPasskey(self, device, passkey, entered):
        print("DisplayPasskey (%s, %06u entered %u)" % (device, passkey, entered))

    @dbus.service.method(AGENT_INTERFACE, in_signature="os", out_signature="")
    def DisplayPinCode(self, device, pincode):
        print("DisplayPinCode (%s, %s)" % (device, pincode))

    @dbus.service.method(AGENT_INTERFACE, in_signature="ou", out_signature="")
    def RequestConfirmation(self, device, passkey):
        print("RequestConfirmation (%s, %06d)" % (device, passkey))
        set_trusted(device)
        return

    @dbus.service.method(AGENT_INTERFACE, in_signature="o", out_signature="")
    def RequestAuthorization(self, device):
        print("RequestAuthorization (%s)" % (device))
        set_trusted(device)
        return

    @dbus.service.method(AGENT_INTERFACE, in_signature="", out_signature="")
    def Cancel(self):
        print("Cancel")

class Handler:
    def onDestroy(self, *args):
        mainloop.quit()

    def onPlayClicked(self, button):
        mplayer = dbus.Interface(mpObj, 'org.bluez.MediaPlayer1')
        mplayer.Play()

    def onPauseClicked(self, button):
        mplayer = dbus.Interface(mpObj, 'org.bluez.MediaPlayer1')
        mplayer.Pause()

    def onNextClicked(self, button):
        mplayer = dbus.Interface(mpObj, 'org.bluez.MediaPlayer1')
        mplayer.Next()

    def onBackClicked(self, button):
        mplayer = dbus.Interface(mpObj, 'org.bluez.MediaPlayer1')
        mplayer.Previous()

def device_property_changed(interface, properties, invalidated, path):
    if interface == 'org.bluez.MediaPlayer1' and path == '/org/bluez/hci0/dev_84_B8_B8_83_B4_C4/player0':
        if 'Track' in properties:
            populateTrackInfo(properties['Track'])


def populateTrackInfo(trackDict = None):
    if trackDict == None:
        props = dbus.Interface(mpObj, 'org.freedesktop.DBus.Properties')
        trackDict = props.Get('org.bluez.MediaPlayer1', 'Track')
    labelText = trackDict['Artist'] + '\n'
    labelText += trackDict['Title'] + '\n'
    labelText += trackDict['Album'] + '\n'
    label.set_text(labelText)

if __name__ == '__main__':
    # bluetooth setup
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus = dbus.SystemBus()

    agent = Agent(bus, AGENT_PATH)

    obj = bus.get_object("org.bluez", "/org/bluez")
    manager = dbus.Interface(obj, "org.bluez.AgentManager1")
    manager.RegisterAgent(AGENT_PATH, "NoInputNoOutput")

    print("A2DP Agent registered")

    manager.RequestDefaultAgent(AGENT_PATH)

    # window setup
    builder = Gtk.Builder()
    builder.add_from_file("gladeTest.glade")
    builder.connect_signals(Handler())

    window = builder.get_object("mediaWindow")
    window.show_all()

    label = builder.get_object("infoLabel")

    mpObj = bus.get_object("org.bluez", "/org/bluez/hci0/dev_84_B8_B8_83_B4_C4/player0")
    mplayer = dbus.Interface(mpObj, 'org.bluez.MediaPlayer1')

    populateTrackInfo()

    bus.add_signal_receiver(
        device_property_changed,
        bus_name='org.bluez',
        signal_name='PropertiesChanged',
        dbus_interface='org.freedesktop.DBus.Properties',
        path_keyword='path'
        )


    mainloop = GLib.MainLoop()
    mainloop.run()
