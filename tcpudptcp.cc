#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main()
{
    // Create nodes for LAN 1
    NodeContainer lan1Nodes;
    lan1Nodes.Create(2);

    // Create nodes for LAN 2
    NodeContainer lan2Nodes;
    lan2Nodes.Create(2);

    // Create nodes for LAN 3
    NodeContainer lan3Nodes;
    lan3Nodes.Create(2);

    // Create CSMA channels for LANs
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    // Install CSMA net devices on LAN 1
    NetDeviceContainer lan1Devices = csma.Install(lan1Nodes);

    // Install CSMA net devices on LAN 2
    NetDeviceContainer lan2Devices = csma.Install(lan2Nodes);

    // Install CSMA net devices on LAN 3
    NetDeviceContainer lan3Devices = csma.Install(lan3Nodes);

    // Install internet stack on all nodes
    InternetStackHelper stack;
    stack.Install(lan1Nodes);
    stack.Install(lan2Nodes);
    stack.Install(lan3Nodes);

    // Assign IP addresses for LAN 1
    Ipv4AddressHelper address1;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer lan1Interfaces = address1.Assign(lan1Devices);

    // Assign IP addresses for LAN 2
    Ipv4AddressHelper address2;
    address2.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer lan2Interfaces = address2.Assign(lan2Devices);

    // Assign IP addresses for LAN 3
    Ipv4AddressHelper address3;
    address3.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer lan3Interfaces = address3.Assign(lan3Devices);

    // Create TCP traffic generator for LAN 1
    uint16_t port1 = 9; // Arbitrary port number for TCP
    OnOffHelper tcpClient1("ns3::TcpSocketFactory", Address(InetSocketAddress(lan3Interfaces.GetAddress(1), port1)));
    tcpClient1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    tcpClient1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    ApplicationContainer tcpApps1 = tcpClient1.Install(lan1Nodes.Get(0));
    tcpApps1.Start(Seconds(1.0));
    tcpApps1.Stop(Seconds(10.0));

    // Create TCP traffic sink for LAN 3
    PacketSinkHelper tcpSink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port1));
    ApplicationContainer tcpSinkApps1 = tcpSink1.Install(lan3Nodes.Get(1));
    tcpSinkApps1.Start(Seconds(0.0));
    tcpSinkApps1.Stop(Seconds(10.0));

    // Create UDP traffic generator for LAN 2
    uint16_t port2 = 10; // Arbitrary port number for UDP
    OnOffHelper udpClient("ns3::UdpSocketFactory", Address(InetSocketAddress(lan3Interfaces.GetAddress(0), port2)));
    udpClient.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    udpClient.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    ApplicationContainer udpApps = udpClient.Install(lan2Nodes.Get(1));
    udpApps.Start(Seconds(1.0));
    udpApps.Stop(Seconds(10.0));

    // Create UDP traffic sink for LAN 3
    PacketSinkHelper udpSink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port2));
    ApplicationContainer udpSinkApps = udpSink.Install(lan3Nodes.Get(0));
    udpSinkApps.Start(Seconds(0.0));
    udpSinkApps.Stop(Seconds(10.0));

    // Run simulation
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
