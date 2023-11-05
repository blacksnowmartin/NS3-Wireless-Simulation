// Define various parameters
std::string rate("2048bps"); // Data rate in bps
std::string phyMode("DsssRate11Mbps"); // PHY mode
std::string tr_name("manet-routing-attack"); // Attack scenario name
int nodeSpeed = 45; // Node speed in meters per second
int nodePause = 1; // Node pause time in seconds
m_protocolName = "protocol"; // Set a protocol name

// Set default configurations for OnOffApplication
Config::SetDefault("ns3::OnOffApplication::PacketSize", StringValue("64")); // Packet size
Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue(rate)); // Data rate

// Configure WiFi MAC and PHY layers
NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
    "DataMode", StringValue(phyMode), // Set the data mode
    "ControlMode", StringValue(phyMode)); // Set the control mode
wifiPhy.Set("TxPowerStart", DoubleValue(txp)); // Set the start transmission power
wifiPhy.Set("TxPowerEnd", DoubleValue(txp)); // Set the end transmission power
wifiMac.SetType("ns3::AdhocWifiMac"); // Set WiFi MAC type

// Install WiFi devices on adhoc nodes
NetDeviceContainer adhocDevices = wifi.Install(wifiPhy, wifiMac, adhocNodes);

// Configure mobility for adhoc nodes
MobilityHelper mobilityAdhoc;
int64_t streamIndex = 0; // Used to get consistent mobility across scenarios
ObjectFactory pos;
pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
pos.Set("X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=300.0]")); // Set X-coordinate range
pos.Set("Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]")); // Set Y-coordinate range
Ptr<PositionAllocator> taPositionAlloc = pos.Create()->GetObject<PositionAllocator>();
streamIndex += taPositionAlloc->AssignStreams(streamIndex);

// Configure random waypoint mobility model
std::stringstream ssSpeed;
ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << nodeSpeed << "]";
std::stringstream ssPause;
ssPause << "ns3::ConstantRandomVariable[Constant=" << nodePause << "]";
mobilityAdhoc.SetMobilityModel("ns3::RandomWaypointMobilityModel",
    "Speed", StringValue(ssSpeed.str()), // Set node speed
    "Pause", StringValue(ssPause.str()), // Set pause time
    "PositionAllocator", PointerValue(taPositionAlloc));
mobilityAdhoc.SetPositionAllocator(taPositionAlloc);
mobilityAdhoc.Install(adhocNodes);
streamIndex += mobilityAdhoc.AssignStreams(adhocNodes, streamIndex);

// Set up OnOff applications for generating traffic
OnOffHelper onoff1("ns3::UdpSocketFactory", Address());
onoff1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]")); // Set OnTime
onoff1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]")); // Set OffTime

// Continue with setting up the OnOff applications as needed for your scenario.
