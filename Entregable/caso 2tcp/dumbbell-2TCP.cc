/*
    TOPOLOGIA DUMBBELL:

    TCP                    TCP
       \                  / 
        \ +--+      +--+ /
          |S1| ---- |S2|
        / +--+      +--+ \ 
       /                  \  
    TCP                    TCP
*/

#include <iostream>
 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "ns3/netanim-module.h" 
#include "ns3/point-to-point-layout-module.h" 

#include "ns3/ipv4-address.h"
#include "ns3/node.h"
#include "ns3/ptr.h"

#include "functions.h"

static ApplicationContainer clienteApps;
static uint32_t port;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SOR2-TP2-Dumbbell-2TCP");


OnOffHelper ON_OFF_ApplicationHelper (std::string factory, Ipv4Address ipv4RemoteAddress)
{
  OnOffHelper clientHelper (factory, Address ());
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable"));
  
  //Formar un punto final (nodo Destino) de transporte ipv4, con ipv4 y puerto.
  AddressValue remoteAddress (InetSocketAddress (ipv4RemoteAddress, port));
  clientHelper.SetAttribute ("Remote", remoteAddress);

  return clientHelper;
}


int main (int argc, char *argv[])
{
  uint32_t  nSenders = 3;
  uint32_t  nReceivers = 3; 
  bool tracing = true;

  port = 1000;

  std::string animFile = "SOR2-TP2-Dumbbell-2TCP-animation.xml" ;  

  CommandLine cmd;
  cmd.AddValue ("nSenders", "Number of left side leaf nodes", nSenders);
  cmd.AddValue ("nReceivers","Number of right side leaf nodes", nReceivers);
  cmd.AddValue ("tracing", "Enable/Disable Tracing", tracing);
  cmd.AddValue ("animFile",  "File Name for Animation Output", animFile);

  //Este helper crea los enlaces punto a punto
  PointToPointHelper p2pNode;
  p2pNode.SetDeviceAttribute    ("DataRate", StringValue ("150kb/s"));
  p2pNode.SetChannelAttribute   ("Delay", StringValue ("100ms"));
  p2pNode.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("10p"));

  PointToPointHelper p2pBottleNeck;
  p2pBottleNeck.SetDeviceAttribute  ("DataRate", StringValue ("50kb/s"));
  p2pBottleNeck.SetChannelAttribute ("Delay", StringValue ("100ms"));
  p2pBottleNeck.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("10p"));
 
  //Este helper Facilita la creación de una topología dumbbell con enlaces p2p. 
  PointToPointDumbbellHelper d (nSenders, p2pNode,
                                nReceivers, p2pNode,
                                p2pBottleNeck);

  // Install Stack
  // Este Helper agrega funcionalidad IP/TCP/UDP a los nodos existentes
  // Permite el seguimiento pcap y ascii de eventos en la pila de Internet asociada con un nodo.
  InternetStackHelper stack;
  d.InstallStack (stack);
 
  // Asigna IP Addresses
  d.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                         Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),
                         Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"));

  //Facilita la tarea de administrar el enrutamiento global. Crea una base de datos de enrutamiento e inicializa las tablas de 
  //enrutamiento de los nodos en la simulación. Convierte todos los nodos de la simulación en enrutadores.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  
  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application TCP1
  // Facilita el trabajo con OnOffApplications (Envio de paquetes).
  OnOffHelper clientHelper = ON_OFF_ApplicationHelper("ns3::TcpSocketFactory", d.GetRightIpv4Address (1));
  clientApps.Add (clientHelper.Install (d.GetLeft (0)));
  //ON-OFF Application TCP2
  clientHelper = ON_OFF_ApplicationHelper("ns3::TcpSocketFactory", d.GetRightIpv4Address (2));
  clientApps.Add (clientHelper.Install (d.GetLeft (1)));

  PacketSinkHelper serverHelper ("ns3::TcpSocketFactory",InetSocketAddress (d.GetRightIpv4Address (1), port));
  serverApps.Add (serverHelper.Install (d.GetRight (1)));
  PacketSinkHelper serverHelper2 ("ns3::TcpSocketFactory",InetSocketAddress (d.GetRightIpv4Address (2), port));
  serverApps.Add (serverHelper2.Install (d.GetRight (2)));

  //--------------------------------------------------------------------------------------------------------------------------
  //Comienzo y parada de paquetes enviados (Clientes(emisores) -> Servidores(receptores)).
  clientApps.Start (Seconds (0.0));
  clientApps.Stop (Seconds (50.0));
  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (100.0));

  //Generacion de pcap's. - Captura de los paquetes de cada enlace (aristas).
    if (tracing)
    {
      p2pNode.EnablePcapAll ("SOR2-TP2-Dumbbell-2TCP", false);
  
      std::ofstream ascii;
      std::string prefix_file_name = "SOR2-TP2-Dumbbell";

      Ptr<OutputStreamWrapper> ascii_wrap;
      ascii.open ((prefix_file_name + "-ascii").c_str ());
      ascii_wrap = new OutputStreamWrapper ((prefix_file_name + "-ascii").c_str (),
                                            std::ios::out);
      stack.EnableAsciiIpv4All (ascii_wrap);

      Simulator::Schedule (Seconds (0.001), &Trace_Cwnd_n2i0, prefix_file_name + "-n2i0-cwnd.data");
      Simulator::Schedule (Seconds (0.001), &Trace_Ssthresh_n2i0, prefix_file_name + "-n2i0-ssthresh.data");

      Simulator::Schedule (Seconds (0.001), &Trace_Cwnd_n3i0, prefix_file_name + "-n3i0-cwnd.data");
      Simulator::Schedule (Seconds (0.001), &Trace_SsThresh_n3i0, prefix_file_name + "-n3i0-ssthresh.data");
      
    }

  // Set the bounding box for animation
  d.BoundingBox (1, 1, 100, 100);
 
  // Create the animation object and configure for specified output
  AnimationInterface anim (animFile);
  anim.EnablePacketMetadata (); // Optional
  anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (10)); // Optional
 
  NS_LOG_INFO ("Starting Simulation...");
  Simulator::Run ();
  std::cout << "Animation Trace file created:" << animFile.c_str ()<< std::endl;
  NS_LOG_INFO ("Finishing Simulation!");
  Simulator::Destroy ();
  return 0;                                
}