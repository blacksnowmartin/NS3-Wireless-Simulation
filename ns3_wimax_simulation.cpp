#include "ns3/test.h"
#include "ns3/simulator.h"
#include "ns3/wimax-helper.h"
#include "ns3/cs-parameters.h"
#include "ns3/ipcs-classifier-record.h"
#include "ns3/service-flow.h"
using namespace ns3;

// Define a test case for WiMAX service flow creation
class Ns3WimaxSfCreationTestCase : public TestCase
{
public:
    Ns3WimaxSfCreationTestCase();
    virtual ~Ns3WimaxSfCreationTestCase();
private:
    virtual void DoRun(void);
};

// Constructor for the test case
Ns3WimaxSfCreationTestCase::Ns3WimaxSfCreationTestCase()
    : TestCase("Test the service flow TLV implementation.")
{
}

// Destructor for the test case
Ns3WimaxSfCreationTestCase::~Ns3WimaxSfCreationTestCase()
{
}

// The main logic of the test case
void Ns3WimaxSfCreationTestCase::DoRun(void)
{
    int duration = 2;
    // Set the scheduler type to simple
    WimaxHelper::SchedulerType scheduler = WimaxHelper::SCHED_TYPE_SIMPLE;
    NodeContainer ssNodes;
    NodeContainer bsNodes;
    ssNodes.Create(1);
    bsNodes.Create(1);

    // Create a WiMAX helper
    WimaxHelper wimax;
    NetDeviceContainer ssDevs, bsDevs;

    // Install subscriber station and base station devices
    ssDevs = wimax.Install(ssNodes, WimaxHelper::DEVICE_TYPE_SUBSCRIBER_STATION,
        WimaxHelper::SIMPLE_PHY_TYPE_OFDM, scheduler);
    bsDevs = wimax.Install(bsNodes, WimaxHelper::DEVICE_TYPE_BASE_STATION,
        WimaxHelper::SIMPLE_PHY_TYPE_OFDM, scheduler);

    // Configure modulation type for subscriber station
    ssDevs.Get(0)->GetObject<SubscriberStationNetDevice>()->SetModulationType(WimaxPhy::MODULATION_TYPE_QAM16_12);

    // Create an internet stack for communication
    InternetStackHelper stack;
    stack.Install(bsNodes);
    stack.Install(ssNodes);

    // Set up IP addresses for devices
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer SSinterfaces = address.Assign(ssDevs);
    Ipv4InterfaceContainer BSinterface = address.Assign(bsDevs);

    // Create a downlink service flow
    ServiceFlow* DlServiceFlowUgs = new ServiceFlow(ServiceFlow::SF_DIRECTION_DOWN);
    IpcsClassifierRecord DlClassifierUgs(Ipv4Address("0.0.0.0"), Ipv4Mask("0.0.0.0"),
        Ipv4Address("0.0.0.0"), Ipv4Mask("0.0.0.0"),
        3000, 3000, 0, 35000, 17, 1);
    CsParameters DlcsParam(CsParameters::ADD, DlClassifierUgs);
    DlServiceFlowUgs->SetConvergenceSublayerParam(DlcsParam);
    DlServiceFlowUgs->SetCsSpecification(ServiceFlow::IPV4);
    DlServiceFlowUgs->SetServiceSchedulingType(ServiceFlow::SF_TYPE_UGS);
    DlServiceFlowUgs->SetMaxSustainedTrafficRate(1000000);
    DlServiceFlowUgs->SetMinReservedTrafficRate(1000000);
    DlServiceFlowUgs->SetMinTolerableTrafficRate(1000000);
    DlServiceFlowUgs->SetMaximumLatency(10);
    DlServiceFlowUgs->SetMaxTrafficBurst(1000);
    DlServiceFlowUgs->SetTrafficPriority(1);

    // Create an uplink service flow
    ServiceFlow* UlServiceFlowUgs = new ServiceFlow(ServiceFlow::SF_DIRECTION_UP);
    IpcsClassifierRecord UlClassifierUgs(Ipv4Address("0.0.0.0"), Ipv4Mask("0.0.0.0"),
        Ipv4Address("0.0.0.0"), Ipv4Mask("0.0.0.0"),
        0, 35000, 3000, 3000, 17, 1);
    CsParameters UlcsParam(CsParameters::ADD, UlClassifierUgs);
    UlServiceFlowUgs->SetConvergenceSublayerParam(UlcsParam);
    UlServiceFlowUgs->SetCsSpecification(ServiceFlow::IPV4);
    UlServiceFlowUgs->SetServiceSchedulingType(ServiceFlow::SF_TYPE_UGS);
    UlServiceFlowUgs->SetMaxSustainedTrafficRate(1000000);
    UlServiceFlowUgs->SetMinReservedTrafficRate(1000000);
    UlServiceFlowUgs->SetMinTolerableTrafficRate(1000000);
    UlServiceFlowUgs->SetMaximumLatency(10);
    UlServiceFlowUgs->SetMaxTrafficBurst(1000);
    UlServiceFlowUgs->SetTrafficPriority(1);

    // Add service flows to the subscriber station device
    ssDevs.Get(0)->GetObject<SubscriberStationNetDevice>()->AddServiceFlow(DlServiceFlowUgs);
    ssDevs.Get(0)->GetObject<SubscriberStationNetDevice>()->AddServiceFlow(UlServiceFlowUgs);

    // Stop, run, and destroy the simulator
    Simulator::Stop(Seconds(duration));
    Simulator::Run();
    Simulator::Destroy();
}

// Define a test suite class
class Ns3WimaxServiceFlowTestSuite : public TestSuite
{
public:
    Ns3WimaxServiceFlowTestSuite();
};

// Constructor for the test suite
Ns3WimaxServiceFlowTestSuite::Ns3WimaxServiceFlowTestSuite()
    : TestSuite("wimax-service-flow", UNIT)
{
    // Add the test case to the suite
    AddTestCase(new Ns3WimaxSfCreationTestCase, TestCase::QUICK);
}

// Define the test suite
static Ns3WimaxServiceFlowTestSuite ns3WimaxServiceFlowTestSuite;
