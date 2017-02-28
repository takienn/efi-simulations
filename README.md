# efi-simulations

Apparently there is a bug in Wifi MAC protocol. When a node is equipped with two netdevices, one operating as a station netdevice connecting to another AP, the other as an AP to another station; The Station mode netdevice doesn not associate with its AP.
Here is a code to show that.

```cpp
  Ssid ssid = Ssid("AP");
  macHelper.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid));
  NetDeviceContainer apDevice = wifiHelper.Install(wifiPhyHelper, macHelper, ap);

  // Now this will connect to AP "AP"
  macHelper.SetType ("ns3::StaWifiMac",
		     "Ssid", SsidValue (ssid));
  NetDeviceContainer relayStaDevice = wifiHelper.Install(wifiPhyHelper, macHelper, relay);
  
  // Creating a second interface as a new AP "RELAY"
  ssid = Ssid("RELAY");
  macHelper.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid));
  NetDeviceContainer relayApDevice = wifiHelper.Install(wifiPhyHelper, macHelper, relay);
```

relay will never associate with "AP" unless wifiPhyHelper sets a new Ptr<Channel> or changes WifiPhy Channel Number by calling wifiHelper.Set("ChannelNumber", UintegerValue(NEW_CHANNEL_NUMBER));
