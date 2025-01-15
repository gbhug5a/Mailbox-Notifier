#  UPDATE-2025


##  Software

The original software used IFTTT to send the notifications.  However, IFTTT has since become a subscription service for using things like Webhooks.  So the new sketch uses the Pushover app.  Pushover is not free, but requires only a single $4.99 payment for lifetime access up to 10,000 push notifications per month.  The new software works with either the original circuit or the modified circuit described below.

##  Hardware

With the original circuit, if the mailbox door is left open, power to the D1 Mini Pro will stay on.  The new circuit prevents that, so power is shut down after the notification is sent in all cases.  However, there is no longer any way to detect if the door is left open.  In the new circuit, the door switch sends a positive pulse to the base of the NPN transistor through a capacitor.  That makes the pulse temporary even if the switch remains closed because the mailbox door has been left open.

Note that capacitor C1 in the schematic can be either one 220nF or two 100nF caps in parallel.  100nF is the standard bypass cap, so it is much more likely to be in your stash.

The 1000uF electolytic capacitor in the original circuit has been removed.  Testing couldn't establish that it is needed or provides any benefit.

Thanks to @Zero999 and @golden_labels of the EEVblog forum for their suggestions for the new circuit.

