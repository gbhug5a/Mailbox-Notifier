# MAILBOX NOTIFIER

------------------------------------------

##  Update - 2025

Since IFTTT has become subscription, the software has been rewritten to use the Pushover app.  And the circuit has been modified so the power does not stay on if the door is left open.  See the Update-2025 folder.

------------------------------------------

This is a battery-powered device that notifies you when the mailbox door has been opened, and again a minute later if the mailbox door has been left open.  A Lolin/Wemos D1 Mini or D1 Mini Pro connects to your home router and sends a message to the "If This Then That" server to trigger a Webhooks event, which in turn sends an SMS, or an alert to the IFTTT app on your phone, or any other "That" which you might select.

## Hardware

The biggest challenge is getting WiFi to connect to your router from inside a metal mailbox. If your mailbox is sufficiently RF leaky, the trace antenna on the D1 Mini may work well enough.  If not, a D1 Mini Pro may be needed so you can mount a WiFi antenna outside the mailbox. The D1 Mini Pro used in this project came from Banggood in a package which included a six-inch pigtail and matching RP-SMA WiFi antenna.  Banggood also has 50cm and 100cm RP-SMA pigtails if needed.


## Power

The unit is powered by three Energizer "Ultimate Lithium" lithium iron disulfide AA batteries, which are nominally 1.5V batteries.  These batteries are NOT rechargeable, but have low internal resistance, which does not increase as they discharge, and they work well in the cold.  Another option would be a single rechargeable 18650 lithium-ion cell.

Since the device will be active for only about 15 seconds twice a day, it seemed best to simply shut down power completely at all other times to maximize battery life (the ESP8266 does not deep-sleep very soundly).  Power is switched via a P-channel mosfet.  Its gate is pulled high through a 100K resistor, which keeps the mosfet turned off.  A magnetic switch at the front of the mailbox is open when the magnet is nearby, as is the case when the door is closed, but the switch closes when the door is opened, which grounds the mosfet's gate, turning it on, and power is supplied to the D1 Mini.

Another path to ground for the mosfet's gate is provided by an NPN transistor whose base is connected to pin D2. The first action in the sketch is to set D2 to OUTPUT, HIGH, which turns on the NPN, thus keeping the mosfet turned on even after the door is closed.  See the schematic.

The D1 Mini may use over 100mA when transmitting, so the chosen mosfet should have a low RDSon rating to prevent a voltage sag across the mosfet during such current spikes.  No such mosfet was found in a TO92 package, so a SMD mosfet was used, requiring an adapter.  A 1000µF 6.3V electrolytic capacitor on the 3.3V pin helps maintain Vcc voltage during such spikes, particularly if a clone Mini or Mini Pro is used - they may have marginal voltage regulators.


## Software

The sketch was compiled in the Arduino IDE v1.8.8 with the ESP8266 core v2.5.2 installed.

The sketch software opens a WiFi connection to the router.  Then it makes a secure HTTPS encrypted connection to the IFTTT server on port 443.  However, thanks to the setInsecure() function associated with BearSSL, the server's authentication is not checked, so there is no need to deal with the server's certificate or fingerprint.

After the trigger connection is completed, power is turned off by bringing D2 LOW.  The sketch then delays  for one minute.  If the door has already been closed, turning off D2 will indeed turn off power long before the delay has completed.  But if the mail carrier has left the door open, the magnetic switch will keep the power on even with D2 LOW.  Then the one minute delay will be completed, and a second notification will be initiated for the "Door Left Open" event.  After that, the ESP8266 will go into deep sleep mode.  When the door is eventually closed, the power will be turned off at that time.

The sketch contains many serial print statements so progress can be followed during testing.  In the mailbox, with serial not connected, those statements will have no effect.


Note that a notification will also be sent when you open the mailbox to remove the mail.

