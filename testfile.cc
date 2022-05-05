/* Probando nuevo archivo
Este es el primer archivo de prueba para compilar y ejecutar:
1)guardar en .../ns-allinone-3.35/ns-3.35/scratch
2)Ir a ../ns-allinone-3.35/ns-3.35
3) Abrir la terminal y ejecutar el siguiente comando:

./waf --run scratch/testfile.cc

 
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;//Genera un contenedor de nodos nombre "nodes"
  nodes.Create (2);//asigna 2 nodos a dicho contenedor

  PointToPointHelper pointToPoint;//point to point es un OBJETO de tipo pointToPointHelper
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));//velocidad
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));//comienza en

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");//esta es la direccion de toda la red.

  Ipv4InterfaceContainer interfaces = address.Assign (devices);//

  UdpEchoServerHelper echoServer (9);//asigna el puerto 9 al server

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));//instala al nodo 2 (1) atributos del server
  serverApps.Start (Seconds (1.0));//cuando arranca y cuando para el server
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);//hace la interfaz del cliente con el servidor 
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));//nodo 1 (0) es el cliente
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  
  NS_LOG_UNCOND ("Comienza del programa de prueba.");//esto sirve como un printf("")
  Simulator::Run ();
  NS_LOG_UNCOND ("Fin del programa de prueba.");
  Simulator::Destroy ();
  return 0;
}
