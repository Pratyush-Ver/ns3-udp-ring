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

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>


#include "ns3/core-module.h"

#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");


int main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (5);

  NodeContainer n0n1 = NodeContainer (nodes.Get (0), nodes.Get (1));
  NodeContainer n1n2 = NodeContainer (nodes.Get (1), nodes.Get (2));
  NodeContainer n2n4 = NodeContainer (nodes.Get (2), nodes.Get (4));
  NodeContainer n3n4 = NodeContainer (nodes.Get (3), nodes.Get (4));
  NodeContainer n0n3 = NodeContainer (nodes.Get (0), nodes.Get (3));

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("500kbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  NetDeviceContainer d3d4 = p2p.Install (n3n4);
  
  //PointToPointHelper pointToPoint2;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  //p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  NetDeviceContainer d0d1 = p2p.Install (n0n1);
  NetDeviceContainer d1d2 = p2p.Install (n1n2);
  NetDeviceContainer d2d4 = p2p.Install (n2n4);
  NetDeviceContainer d0d3 = p2p.Install (n0n3);

  //NetDeviceContainer devices;
  //devices = pointToPoint.Install (nodes);
  //devices.Add(pointToPoint1.Install(nodes.Get(0), nodes.Get(1)));
  //devices.Add(pointToPoint1.Install(nodes.Get(1), nodes.Get(2)));
  //devices.Add(pointToPoint1.Install(nodes.Get(2), nodes.Get(4)));
  //devices.Add(pointToPoint2.Install(nodes.Get(3), nodes.Get(4)));
  //devices.Add(pointToPoint1.Install(nodes.Get(0), nodes.Get(3)));

  InternetStackHelper stack;
  stack.Install (nodes);


  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  //Ipv4InterfaceContainer i0i1 = address.Assign (d0d1);
  address.Assign (d0d1);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  //Ipv4InterfaceContainer i1i2 = address.Assign (d1d2);
  address.Assign (d1d2);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  //Ipv4InterfaceContainer i3i4 = address.Assign (d3d4);
  address.Assign (d3d4);
  
  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i4 = address.Assign (d2d4);
  //address.Assign (d2d4);

  address.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i3 = address.Assign (d0d3);
  //address.Assign (d0d3);
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (2));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (20.0));

  UdpEchoClientHelper echoClient (i2i4.GetAddress (0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (20.0));


  AnimationInterface anim ("udpecho1.xml");

  Ptr<ConstantPositionMobilityModel> s1 = nodes.Get (0)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s2 = nodes.Get (1)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s3 = nodes.Get (2)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s4 = nodes.Get (3)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s5 = nodes.Get (4)->GetObject<ConstantPositionMobilityModel> ();

  s1->SetPosition (Vector ( 0.0, 30.0, 0  ));
  s2->SetPosition (Vector ( 45.0, 30.0, 0  ));
  s3->SetPosition (Vector ( 90.0, 30.0, 0  ));
  s4->SetPosition (Vector ( 30.0, 60.0, 0  ));
  s5->SetPosition (Vector ( 60.0, 60.0, 0  ));


  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
