// Create a spectrum channel helper
SpectrumChannelHelper channelHelper = SpectrumChannelHelper::Default ();

// Create a spectrum channel
Ptr<SpectrumChannel> channel = channelHelper.Create ();

// Create a factory for WiFi spectrum values with a bandwidth of 5 MHz
WifiSpectrumValue5MhzFactory sf;

// Set the transmission power in Watts
double txPower = 0.1; // Watts

// Specify the channel number
uint32_t channelNumber = 1;

// Create a power spectral density for transmission
Ptr<SpectrumValue> txPsd = sf.CreateTxPowerSpectralDensity(txPower, channelNumber);

// Calculate the power spectral density for thermal noise
// using Boltzmann's constant and temperature
const double k = 1.381e-23; // Boltzmann's constant
const double T = 290; // Temperature in Kelvin
double noisePsdValue = k * T; // Watts per Hertz

// Create a constant power spectral density for noise
Ptr<SpectrumValue> noisePsd = sf.CreateConstant(noisePsdValue);

// Create a helper for AdhocAlohaNoackIdealPhy
AdhocAlohaNoackIdealPhyHelper deviceHelper;

// Set the channel, transmission power spectral density, and noise power spectral density
deviceHelper.SetChannel(channel);
deviceHelper.SetTxPowerSpectralDensity(txPsd);
deviceHelper.SetNoisePowerSpectralDensity(noisePsd);

// Set the physical layer attribute for the transmission rate (1Mbps)
deviceHelper.SetPhyAttribute("Rate", DataRateValue(DataRate("1Mbps")));

// Install network devices using the device helper
NetDeviceContainer devices = deviceHelper.Install(c);

// Create a packet socket helper
PacketSocketHelper packetSocket;

// Install packet sockets on network devices
packetSocket.Install(c);

// Create a packet socket address for communication
PacketSocketAddress socket;

// Set the source device (index) for the socket
socket.SetSingleDevice(devices.Get(0)->GetIfIndex());

// Set the physical address of the destination device
socket.SetPhysicalAddress(devices.Get(1)->GetAddress());

// Set the protocol (e.g., 1 for IP)
socket.SetProtocol(1);

// Create an On/Off application helper for packet transmission
OnOffHelper onoff("ns3::PacketSocketFactory", Address(socket));

// Set a constant data transmission rate (0.5Mbps)
onoff.SetConstantRate(DataRate("0.5Mbps"));

// Set the packet size to 125 bytes
onoff.SetAttribute("PacketSize", UintegerValue(125));

// Install the On/Off applications on the first network device
ApplicationContainer apps = onoff.Install(c.Get(0));

// Start the applications after 0.1 seconds and stop them after 0.104 seconds
apps.Start(Seconds(0.1));
apps.Stop(Seconds(0.104));

// Set up a socket to receive packets on the second network device
Ptr<Socket> recvSink = SetupPacketReceive(c.Get(1));

// Stop the simulation after 10 seconds
Simulator::Stop(Seconds(10.0));

// Connect various trace points for collecting simulation data
Config::Connect("/NodeList/*/DeviceList/*/Phy/TxStart", MakeCallback(&PhyTxStartTrace));
Config::Connect("/NodeList/*/DeviceList/*/Phy/TxEnd", MakeCallback(&PhyTxEndTrace));
Config::Connect("/NodeList/*/DeviceList/*/Phy/RxStart", MakeCallback(&PhyRxStartTrace));
Config::Connect("/NodeList/*/DeviceList/*/Phy/RxEndOk", MakeCallback(&PhyRxEndOkTrace));
Config::Connect("/NodeList/*/DeviceList/*/Phy/RxEndError", MakeCallback(&PhyRxEndErrorTrace));
