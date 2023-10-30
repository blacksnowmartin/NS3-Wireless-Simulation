#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

// Define the logging component
NS_LOG_COMPONENT_DEFINE ("Wifi-2-nodes-fixed");

// Function to print the locations of nodes
void PrintLocations(NodeContainer nodes, std::string header) {
    std::cout << header << std::endl;
    for (NodeContainer::Iterator iNode = nodes.Begin(); iNode != nodes.End(); ++iNode) {
        Ptr<Node> object = *iNode;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel>();
        NS_ASSERT(position != 0);
        Vector pos = position->GetPosition();
        std::cout << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    }
    std::cout << std::endl;
}

// Function to print IP addresses
void PrintAddresses(Ipv4InterfaceContainer container, std::string header) {
    std::cout << header << std::endl;
    uint32_t nNodes = container.GetN();
    for (uint32_t i = 0; i < nNodes; ++i) {
        std::cout << container.GetAddress(i, 0) << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    bool verbose = true;
    uint32_t nWifi = 2;
    double xDistance = 10.0;

    CommandLine cmd;
    cmd.AddValue("xDistance", "Distance between two nodes along the x-axis", xDistance);
    cmd.Parse(argc, argv);

    if (verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    // Create nodes and set up Wi-Fi communication
    NodeContainer wifiStaNodes, wifiApNode;
    wifiStaNodes.Create(nWifi);
    wifiApNode = wifiStaNodes.Get(0);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());
    WifiHelper wifi = WifiHelper::Default();
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    Ssid ssid = Ssid("ns-3-ssid");

    // Set up Wi-Fi devices
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    NetDeviceContainer staDevices;
    staDevices = wifi.Install(phy, mac, wifiStaNodes.Get(1));

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid), "BeaconGeneration", BooleanValue(true), "BeaconInterval", TimeValue(Seconds(5)));
    NetDeviceContainer apDevice;
    apDevice = wifi.Install(phy, mac, wifiApNode);

    // Set up node mobility
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(0.0), "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(xDistance), "DeltaY", DoubleValue(10.0), "GridWidth", UintegerValue(3), "LayoutType", StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiStaNodes);

    // Install the Internet stack and configure IP addresses
    InternetStackHelper stack;
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;
    Ipv4InterfaceContainer wifiInterfaces, wifiApInterface;
    address.SetBase("10.1.1.0", "255.255.255.0");
    wifiApInterface = address.Assign(apDevice);
    wifiInterfaces = address.Assign(staDevices);

    // Set up UDP echo applications
    UdpEchoServerHelper echoServer(9); // Port # 9
    ApplicationContainer serverApps = echoServer.Install(wifiApNode);
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(4.0));

    UdpEchoClientHelper echoClient(wifiApInterface.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientApps = echoClient.Install(wifiStaNodes.Get(1));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(3.0));

    // Populate routing tables and run the simulation
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    Simulator::Stop(Seconds(4.0));

    // Print IP addresses and node locations
    PrintAddresses(wifiInterfaces, "IP addresses of base stations");
    PrintAddresses(wifiApInterface, "IP address of AP");
    PrintLocations(wifiStaNodes, "Location of all nodes");

    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
