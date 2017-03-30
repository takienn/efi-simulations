# efi-simulations

## Bug 1:
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

## Bug 2:
Apparently when activating "ActiveProbe" on StaWifiMac in our scenario we hit this ASSERTION:

```
assert failed. cond="duration >= MicroSeconds (0)", msg="Please provide test case to maintainers if this assert is hit.", file=../src/wifi/model/mac-low.cc, line=2150
terminate called without an active exception
```
## Bug 3:
For some reason while using ```PsrErrorRateModel``` at WifiPhy the throughput considerably drops, no matter what is the value of the PSR set.
An alternative is to enforce PSR at the ```ReceivePacket``` function.

## Queue monitoring
ApWifimac uses two different types of queuing mechanism based on wether or not QoS is enabled.
* With QoS on, it uses an instance of EdcaQueues to map different access categories ```AcIndex``` and their channel access functions ```EdcaTxopN```. The ```EdcaTxopN``` manages packets queueing via an instance of ```WifiMacQueue```.
* With QoS off, it just uses an instance of DcaTxop. DcaTxop also manages queuein via an instance of ```WifiMacQueue```.

```WifiMacQueue``` is a specialization of ```WifiQueue``` which itself is a child class of ```Queue```. It, thus, provides access to usefull trace sources defined in ```Queue```. Mainly:
- ```Enqueue```: fired when a packet is enqueued in the queue.
- ```Dequeue```: fired when a packet is dequeued from the queue.
- ```Drop```: fired when a packet is droped from the queue.

### Usage
```cpp
Ptr<ApWifiMac> mac = netdevice->GetObject<WifiNetDevice>()->GetMac()->GetObject<ApWifiMac>();
Ptr<WifiMacQueue> wifiMacQueue = mac->GetDcaTxop()->GetQueue();
