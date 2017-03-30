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
```

## Bug 4:

This one is related to queue monitoring.
Hooking directly to the Queue<Item> instance and logging the Enqeueu/Dequeue only accounts for Queuing and Dequeing that actually get downs to there, as a consequence it is unlikely that any dropping will ever happen. More clearly, the NetDeviceQueue implementation handles Items queueing/dequeing to the queue in such a way that it doesn't allow that and stops any packets(items) to be inserted into the queue if it is not able to hold them.

```PacketEnqueued``` traces the ```Queue::Enqueue``` actions and stops the queueing via ```NetDeviceQueue::Stop``` if the queue is not able to store another packet.

The ```TrafficControlLayer``` handles transmission, when it attemps to send a packet it enqueues it into a ```QueueDisc``` object that when run for transmission at ```QueueDisc::Transmit``` checks if the underlying NetDeviceQueue is not stopped (either for full queue or attaing queue defined limits). If the NetDeviceQueue is stopped, it Requeues the packet untill it is Dequeued.

With that said, a better way to monitor queueing behavior is to trace Enqueue/Dequeue/Requeue/Drop in the ```QueueDisc``` hold by a ```TrafficControlLayer``` instance.

A ```TrafficControlLayer``` instance can be access like this:

```cpp
Ptr<TrafficControlLayer> tc = node->GetObject<TrafficControlLayer> ();
```

Then accessing the ```RootQueueDiscList``` stored there for all supported node's devices:

```cpp
ObjectMapValue rootQueueDiscList;
tc->GetAttribute ("RootQueueDiscList", rootQueueDiscList);
```

