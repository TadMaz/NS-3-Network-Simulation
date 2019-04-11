/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ScratchSimulator");

int 
main (int argc, char *argv[])
{
  NodeContainer hosts;
  NodeContainer routers;
  
  hosts.Create(3);
  routers.Create(4);

  //Install Internet Stack on each node
  InternetStackHelper stack;
  stack.Install(hosts);
  stack.Install(routers);

  //create point to point helper
  PointToPointHelper p2p;

  //Assigning IP adresses
  Ipv4AddressHelper address; 
  
  //Adding attributes
  
  std::string speed = "10Mbps";
  CommandLine cmd;
  cmd.AddValue("DefaultRate","Default data rate to be used on network devices",speed);
  cmd.Parse(argc,argv);
  std::cout <<"Speed="<<speed<<std::endl;
  
  DataRate rate(speed);
  Time delay("2ms");

  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", DataRateValue(rate));
  Config::SetDefault("ns3::PointToPointChannel::Delay", TimeValue(delay));
  Config::SetDefault("ns3::CsmaNetDevice::DataRate", DataRateValue(rate));
  Config::SetDefault("ns3::CsmaChannel::Delay", TimeValue(delay));
  


  //create subnet1
  NodeContainer subnet1;
  subnet1.Add(hosts.Get(0));
  subnet1.Add(hosts.Get(1));
  subnet1.Add(hosts.Get(2));
  subnet1.Add(routers.Get(0));

  //network devices to installed on each node
  NetDeviceContainer subnet1Devices = p2p.Install(subnet1);

  //Configure subnet address and Mask
  address.SetBase("192.168.20.0","255.255.255.0");

  //Creating interfaces
  Ipv4InterfaceContainer subnet1Interfaces = address.Assign(subnet1Devices);
  
  
  //creating subnet2
  NodeContainer subnet2;
  subnet2.Add(routers.Get(0));
  subnet2.Add(routers.Get(1));
  
  //Configuring delays
  DataRate rate_subnet2("50Mbps");
  Time delay_subnet2(MilliSeconds(4));

  //creating network device to hold devices on each node
  NetDeviceContainer subnet2Devices = p2p.Install(subnet2);

  //configure address for subnet 2
  address.SetBase("192.168.21.0","255.255.255.0");
  
  //interface container for subnet2
  Ipv4InterfaceContainer subnet2Interfaces = address.Assign(subnet2Devices);
  
  //Configs again
  Config::Set("/NodeList/1/DeviceList/2/$ns3::PointToPointNetDevice/DataRate",
  DataRateValue(rate_subnet2));

  Config::Set("/NodeList/2/DeviceList/1/$ns3::PointToPointNetDevice/DataRate",
  DataRateValue(rate_subnet2));

  Config::Set("/ChannelList/1/$ns3::PointToPointChannel/Delay",
  TimeValue(delay_subnet2));
   
  //creating subnet3
  NodeContainer subnet3;
  subnet3.Add(routers.Get(1));
  subnet3.Add(routers.Get(2));

  //creating network device to hold devices on each node
  NetDeviceContainer subnet3Devices = p2p.Install(subnet3);

  //configure address for subnet 2
  address.SetBase("192.168.22.0","255.255.255.0");
  
  //interface container for subnet2
  Ipv4InterfaceContainer subnet3Interfaces = address.Assign(subnet3Devices);
  

  //creating subnet4
  NodeContainer subnet4;
  subnet4.Add(routers.Get(1));
  subnet4.Add(routers.Get(3));

  //creating network device to hold devices on each node
  NetDeviceContainer subnet4Devices = p2p.Install(subnet4);

  // Configuring Subnet4

  Ptr<NetDevice> deviceA = subnet4Devices.Get(0);
  Ptr<NetDevice> deviceB = subnet4Devices.Get(1);
  NetDevice* deviceA_ptr = PeekPointer(deviceA);
  NetDevice* deviceB_ptr = PeekPointer(deviceB);
  
  PointToPointNetDevice* p2pDeviceA = dynamic_cast<PointToPointNetDevice*>(deviceA_ptr);
  PointToPointNetDevice* p2pDeviceB = dynamic_cast<PointToPointNetDevice*>(deviceB_ptr);
  
  DataRate rate_subnet4("100Mbps");
  p2pDeviceA->SetAttribute("DataRate",DataRateValue(rate_subnet4));
  p2pDeviceB->SetAttribute("DataRate",DataRateValue(rate_subnet4));

  Ptr<Channel> channel = ChannelList::GetChannel(3);
  Channel* channel_ptr = PeekPointer(channel);

  Time delay_subnet4("5ms");
  
  PointToPointChannel* p2pChannel =  dynamic_cast<PointToPointChannel*>(channel_ptr);
  p2pChannel->SetAttribute("Delay", TimeValue(delay_subnet4));

  //configure address for subnet 4
  address.SetBase("192.168.23.0","255.255.255.0");
  
  //interface container for subnet4
  Ipv4InterfaceContainer subnet4Interfaces = address.Assign(subnet4Devices);

  //Run simulator
  Simulator::Run();
  Simulator::Destroy();

  //Creating branches
  NodeContainer branch;
  branch.Create(3);
  
  //install TCP/Ip stack
  stack.Install(branch);

  //Creating CSMA Helper
  CsmaHelper csma;

  //Configuring individual subnets
  csma.SetDeviceAttribute("DataRate",StringValue("100Mbps"));
  
  
  //Configuring Subnet 2

  
  //Creating Node Container
  NodeContainer subnet5;
  subnet5.Add(routers.Get(2));
  subnet5.Add(branch);

  //creating network device to hold devices on each node
  NetDeviceContainer subnet5Devices = csma.Install(subnet5);

  //configure address for subnet 5
  address.SetBase("192.168.24.0","255.255.255.0");
 
  //interface container for subnet5
  Ipv4InterfaceContainer subnet5Interfaces = address.Assign(subnet5Devices);
  

  return 0;


  
}
