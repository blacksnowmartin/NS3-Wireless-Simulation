#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/mpi-interface.h"

// Check if NS3_MPI is enabled
#ifdef NS3_MPI
#include <mpi.h>
#else
// If NS3_MPI is not enabled, show an error message
#error "ndn-simple-mpi scenario can be compiled only if NS3_MPI is enabled"
#endif

namespace ns3 {
int main(int argc, char* argv[]) {
    // Set default configurations for the network
    Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Gbps"));
    Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("1ms"));
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("10"));

    // Initialize variables
    bool nullmsg = false;
    CommandLine cmd;
    cmd.AddValue("nullmsg", "Enable the use of null-message synchronization", nullmsg);
    cmd.Parse(argc, argv);

    // Configure the simulation to use NullMessage or Distributed Simulator Implementation
    if (nullmsg) {
        GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::NullMessageSimulatorImpl"));
    } else {
        GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::DistributedSimulatorImpl"));
    }

    // Enable MPI for parallel execution
    MpiInterface::Enable(&argc, &argv);
    
    // Get system ID and system count for MPI
    uint32_t systemId = MpiInterface::GetSystemId();
    uint32_t systemCount = MpiInterface::GetSize();

    // Check if MPI should be used with exactly two processors
    if (systemCount != 2) {
        std::cout << "Simulation will run on a single processor only" << std::endl;
        std::cout << "To run using MPI, run" << std::endl;
        std::cout << "mpirun -np 2 ./waf --run=ndn-simple-mpi" << std::endl;
    }

    // Create two nodes
    Ptr<Node> node1 = CreateObject<Node>(0);
    Ptr<Node> node2 = CreateObject<Node>(systemCount == 2 ? 1 : 0);

    // Set up a point-to-point connection between nodes
    PointToPointHelper p2p;
    p2p.Install(node1, node2);

    // Set up the NDN stack on both nodes
    ndn::StackHelper ndnHelper;
    ndnHelper.InstallAll();

    // Configure Forwarding Information Base (FIB) routes
    ndn::FibHelper::AddRoute(node1, "/prefix/1", node2, 1);
    ndn::FibHelper::AddRoute(node2, "/prefix/2", node1, 1);

    // Create NDN applications: consumers and producers
    ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    consumerHelper.SetAttribute("Frequency", StringValue("100")); // 10 interests a second
    ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

    if (systemCount != 2 || systemId == 0) {
        // Configure node 1 for consumer and producer
        consumerHelper.SetPrefix("/prefix/1"); // request /prefix/1/*
        consumerHelper.Install(node1);
        producerHelper.SetPrefix("/prefix/2"); // serve /prefix/2/*
        producerHelper.Install(node1);
        ndn::L3RateTracer::Install(node1, "node1.txt", Seconds(0.5));
    }

    if (systemCount != 2 || systemId == 1) {
        // Configure node 2 for consumer and producer
        consumerHelper.SetPrefix("/prefix/2"); // request /prefix/2/*
        consumerHelper.Install(node2);
        producerHelper.SetPrefix("/prefix/1"); // serve /prefix/1/*
        producerHelper.Install(node2);
        ndn::L3RateTracer::Install(node2, "node2.txt", Seconds(0.5));
    }

    // Set the simulation stop time and run the simulation
    Simulator::Stop(Seconds(400.0));
    Simulator::Run();
    Simulator::Destroy();

    // Disable MPI
    MpiInterface::Disable();

    return 0;
}
}

int main(int argc, char* argv[]) {
    return ns3::main(argc, argv);
}
