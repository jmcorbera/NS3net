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
//static uint32_t puerto;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SOR2-TP2-Dumbbell-2TCP");


OnOffHelper ON_OFF_ApplicationHelper (std::string factory, Ipv4Address ipv4RemoteAddress)
{
  uint32_t port = 1000;

  OnOffHelper clienteHelper (factory, Address ());
  clienteHelper.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable"));
  clienteHelper.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable"));
  
  //Formar un punto final (nodo Destino) de transporte ipv4, con ipv4 y puerto.
  AddressValue remoteAddress (InetSocketAddress (ipv4RemoteAddress, port));
  clienteHelper.SetAttribute ("Remote", remoteAddress);

  //clienteHelper.Install (OriginNode);
  
  return clienteHelper;
}


int main (int argc, char *argv[])
{
  uint32_t  nSenders = 3;
  uint32_t  nReceivers = 3; 
  bool tracing = true;

  //puerto = 1000;

  std::string animFile = "SOR2-TP2-Dumbbell-2TCP-animation.xml" ;  

  CommandLine cmd;
  cmd.AddValue ("nSenders", "Number of left side leaf nodes", nSenders);
  cmd.AddValue ("nReceivers","Number of right side leaf nodes", nReceivers);
  cmd.AddValue ("tracing", "Enable/Disable Tracing", tracing);
  cmd.AddValue ("animFile",  "File Name for Animation Output", animFile);

  //Este helper crea los enlaces punto a punto
  PointToPointHelper p2pNode;
  p2pNode.SetDeviceAttribute    ("DataRate", StringValue ("100kb/s"));
  p2pNode.SetChannelAttribute   ("Delay", StringValue ("100ms"));
  p2pNode.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("10p"));

  PointToPointHelper p2pBottleNeck;
  p2pBottleNeck.SetDeviceAttribute  ("DataRate", StringValue ("100kb/s"));
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
  
  ApplicationContainer clienteApps;
  ApplicationContainer servidorApps;

  uint32_t nodo0 = 0;
  uint32_t nodo1 = 1;
  uint32_t nodo2 = 2;

  uint32_t puerto = 1000;

  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application TCP
  // Facilita el trabajo con OnOffApplications (Envio de paquetes).

  //OnOffHelper clienteHelper ("ns3::TcpSocketFactory", Address ());
  //clienteHelper.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable"));
  //clienteHelper.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable"));

  OnOffHelper clienteHelper = ON_OFF_ApplicationHelper("ns3::TcpSocketFactory", d.GetRightIpv4Address (nodo1));
  clienteApps.Add (clienteHelper.Install (d.GetLeft (nodo0)));

  //Formar un punto final (nodo Destino) de transporte ipv4, con ipv4 y puerto.
  //AddressValue remoteAddress (InetSocketAddress (d.GetRightIpv4Address (nodo1), puerto));
  //clienteHelper.SetAttribute ("Remote", remoteAddress);
  //Agrego a mi contenedor de APPs-Cliente, el nuevo cliente junto al puntero del nodo Origen.
  //clienteApps.Add (clienteHelper.Install (d.GetLeft (nodo0)));

  //Crea la aplicacion de tipo servidor/receptor, con nodo Destino y puerto.
  PacketSinkHelper servidor ("ns3::TcpSocketFactory",InetSocketAddress (d.GetRightIpv4Address (nodo1), puerto));
  //Agrego a mi contenedor de APPs-Servidor, el nuevo servidor junto al puntero y nodo Origen (En este caso = a nodo Destino).
  servidorApps.Add (servidor.Install (d.GetRight (nodo1)));
  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application TCP
  //OnOffHelper clienteHelper2 ("ns3::TcpSocketFactory", Address ());
  //clienteHelper2.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable"));
  //clienteHelper2.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable"));

  OnOffHelper clienteHelper2 = ON_OFF_ApplicationHelper("ns3::TcpSocketFactory", d.GetRightIpv4Address (nodo2));
  clienteApps.Add (clienteHelper2.Install (d.GetLeft (nodo1)));

  //AddressValue remoteAddress2 (InetSocketAddress (d.GetRightIpv4Address (nodo2), puerto));
  //clienteHelper2.SetAttribute ("Remote", remoteAddress2);
  //clienteApps.Add (clienteHelper2.Install (d.GetLeft (nodo1)));

  PacketSinkHelper servidor2 ("ns3::TcpSocketFactory",InetSocketAddress (d.GetRightIpv4Address (nodo2), puerto));
  servidorApps.Add (servidor2.Install (d.GetRight (nodo2)));
  //--------------------------------------------------------------------------------------------------------------------------
  //ON-OFF Application UDP
  //OnOffHelper clienteHelper3 ("ns3::UdpSocketFactory", Address ());
  //clienteHelper3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1000.0]"));
  //clienteHelper3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  //AddressValue remoteAddress3 (InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
  //clienteHelper3.SetAttribute ("Remote", remoteAddress3);
  //clienteApps.Add (clienteHelper3.Install (dumbbell.GetLeft (nodo2)));

  //PacketSinkHelper servidor3 ("ns3::TcpSocketFactory",InetSocketAddress (dumbbell.GetRightIpv4Address (nodo0), puerto));
  //servidorApps.Add (servidor3.Install (dumbbell.GetRight (nodo0)));
  //--------------------------------------------------------------------------------------------------------------------------
  //Comienzo y parada de paquetes enviados (Clientes(emisores) -> Servidores(receptores)).
  clienteApps.Start (Seconds (0.0));
  clienteApps.Stop (Seconds (30.0));
  servidorApps.Start (Seconds (0.0));
  servidorApps.Stop (Seconds (100.0));


/* 
  // Este helper Instala la aplicación ON/OFF en todos los nodos emisores
  OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::UniformRandomVariable"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::UniformRandomVariable"));
  ApplicationContainer clientApps;  

  uint32_t puerto = 1000;
  
  for (uint32_t i = 0; i < ((d.RightCount () < d.LeftCount ()) ? d.RightCount () : d.LeftCount ()); ++i)
    {
      // Cree la aplicación ON/OFF para que envíe paquetes al mismo nodo receiver
      // Formar un punto final (nodo Destino) de transporte ipv4, con ipv4 y puerto.
      AddressValue remoteAddress (InetSocketAddress (d.GetLeftIpv4Address (i), puerto));
      clientHelper.SetAttribute ("Remote", remoteAddress);
      // Se Agrega al contenedor de Apps, el nuevo cliente junto al puntero del nodo Origen.
      clientApps.Add (clientHelper.Install (d.GetRight (i)));
    }  

  clientApps.Start (Seconds (0.0));
  clientApps.Stop (Seconds (10.0));
*/

  //Generacion de pcap's. - Captura de los paquetes de cada enlace (aristas).
    if (tracing)
    {
      p2pNode.EnablePcapAll ("SOR2-TP2-Dumbbell-2TCP", false);

      /*
      std::ofstream ascii;
      Ptr<OutputStreamWrapper> ascii_wrap;
      ascii.open ((prefix_file_name + "-ascii").c_str ());
      ascii_wrap = new OutputStreamWrapper ((prefix_file_name + "-ascii").c_str (),
                                            std::ios::out);
      stack.EnableAsciiIpv4All (ascii_wrap);

      Simulator::Schedule (Seconds (0.001), &TraceCwnd20, prefix_file_name + "-20-cwnd.data");
      Simulator::Schedule (Seconds (0.001), &TraceSsThresh20, prefix_file_name + "-20-ssth.data");
      Simulator::Schedule (Seconds (0.001), &TraceRtt20, prefix_file_name + "-20-rtt.data");
      Simulator::Schedule (Seconds (0.001), &TraceRto20, prefix_file_name + "-20-rto.data");
      Simulator::Schedule (Seconds (0.001), &TraceInFlight20, prefix_file_name + "-20-inflight.data");


      Simulator::Schedule (Seconds (0.001), &TraceCwnd30, prefix_file_name + "-30-cwnd.data");
      Simulator::Schedule (Seconds (0.001), &TraceSsThresh30, prefix_file_name + "-30-ssth.data");
      Simulator::Schedule (Seconds (0.001), &TraceRtt30, prefix_file_name + "-30-rtt.data");
      Simulator::Schedule (Seconds (0.001), &TraceRto30, prefix_file_name + "-30-rto.data");
      Simulator::Schedule (Seconds (0.001), &TraceInFlight30, prefix_file_name + "-30-inflight.data");      
      */
      
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
  
  // Set up the actual simulation
  //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  Simulator::Run ();
  std::cout << "Animation Trace file created:" << animFile.c_str ()<< std::endl;
  Simulator::Destroy ();
  return 0;                                
}
