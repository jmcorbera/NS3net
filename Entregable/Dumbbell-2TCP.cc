//  TOPOLOGIA DUMBBELL:


#include <iostream>
 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "ns3/netanim-module.h" 
#include "ns3/point-to-point-layout-module.h" 

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SOR2-TP2-Dumbbell-2TCP");

int
main (int argc, char *argv[])
{
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (512));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("500kb/s"));

  uint32_t  nSenders = 2;
  uint32_t  nReceivers = 2; 

  std::string animFile = "SOR2-TP2-Dumbbell-2TCP-animation.xml" ;  

  CommandLine cmd;
  cmd.AddValue ("nSenders", "Number of left side leaf nodes", nSenders);
  cmd.AddValue ("nReceivers","Number of right side leaf nodes", nReceivers);
  cmd.AddValue ("animFile",  "File Name for Animation Output", animFile);

  //Este helper crea los enlaces punto a punto
  PointToPointHelper p2pRouter;
  p2pRouter.SetDeviceAttribute  ("DataRate", StringValue ("10Mbps"));
  p2pRouter.SetChannelAttribute ("Delay", StringValue ("1ms"));
  PointToPointHelper p2pNode;
  p2pNode.SetDeviceAttribute    ("DataRate", StringValue ("10Mbps"));
  p2pNode.SetChannelAttribute   ("Delay", StringValue ("1ms"));
 
  //Este helper Facilita la creación de una topología dumbbell con enlaces p2p. 
  PointToPointDumbbellHelper d (nSenders, p2pNode,
                                nReceivers, p2pNode,
                                p2pRouter);

  // Install Stack
  // Este Helper agrega funcionalidad IP/TCP/UDP a los nodos existentes
  // Permite el seguimiento pcap y ascii de eventos en la pila de Internet asociada con un nodo.
  InternetStackHelper stack;
  d.InstallStack (stack);
 
  // Asigna IP Addresses
  d.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                         Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),
                         Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"));
 
  // Este helper Instala la aplicación ON/OFF en todos los nodos emisores
  OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable"));
  ApplicationContainer clientApps;  
  
  for (uint32_t i = 0; i < ((d.RightCount () < d.LeftCount ()) ? d.RightCount () : d.LeftCount ()); ++i)
    {
      // Cree la aplicación ON/OFF para que envíe paquetes al mismo nodo receiver
      AddressValue remoteAddress (InetSocketAddress (d.GetLeftIpv4Address (i), 1000));
      clientHelper.SetAttribute ("Remote", remoteAddress);
      clientApps.Add (clientHelper.Install (d.GetRight (i)));
    }  

  clientApps.Start (Seconds (0.0));
  clientApps.Stop (Seconds (10.0));
 
  // Set the bounding box for animation
  d.BoundingBox (1, 1, 100, 100);
 
  // Create the animation object and configure for specified output
  AnimationInterface anim (animFile);
  anim.EnablePacketMetadata (); // Optional
  anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (10)); // Optional
  
  // Set up the actual simulation
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  Simulator::Run ();
  std::cout << "Animation Trace file created:" << animFile.c_str ()<< std::endl;
  Simulator::Destroy ();
  return 0;                                
}
