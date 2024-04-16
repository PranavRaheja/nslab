#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main ()
{
    // Set up logging
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create nodes
    NodeContainer csmaNodes0;
    csmaNodes0.Create(3); // n0, n1, n2
    NodeContainer csmaNodes1;
    csmaNodes1.Create(3); // n10, n11, n12
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(5); // n3, n5, n6, n7, n8 (n4 and n9 are APs and will be added separately)

    NodeContainer wifiApNode = csmaNodes0.Get(1); // n1 is also n4
    NodeContainer wifiApNode1 = csmaNodes1.Get(2); // n12 is also n9

    // Define CSMA attributes
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    // Install CSMA devices and assign IP addresses
    NetDeviceContainer csmaDevices0 = csma.Install(csmaNodes0);
    NetDeviceContainer csmaDevices1 = csma.Install(csmaNodes1);

    // The below code setups the wifi stations and access points

    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid),
                "ActiveProbing", BooleanValue(false));

    NetDeviceContainer staDevices;
    staDevices = wifi.Install(phy, mac, wifiStaNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevices = wifi.Install(phy, mac, wifiApNode);
    NetDeviceContainer apDevices1 = wifi.Install(phy, mac, wifiApNode1);

    // Setup mobility
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(5.0),
                                  "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(3),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)));
    mobility.Install(wifiStaNodes);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    mobility.Install(wifiApNode1);

    // Install internet stack
    InternetStackHelper stack;
    stack.Install(csmaNodes0);
    stack.Install(csmaNodes1);
    stack.Install(wifiStaNodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    address.Assign(csmaDevices0);
    address.Assign(csmaDevices1);
    address.Assign(staDevices);
    address.Assign(apDevices);
    address.Assign(apDevices1);

    // Enable global routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));

    // Use NetAnim to visualize the simulation
    AnimationInterface anim("csma-wifi-animation.xml");

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
