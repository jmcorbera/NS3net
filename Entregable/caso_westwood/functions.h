#include <string>
#include <fstream>
#include <stddef.h>                    
#include <iomanip>                     
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-dumbbell.h"

using namespace ns3;

static Ptr<OutputStreamWrapper> cWndStream_n2i0;
static Ptr<OutputStreamWrapper> ssThreshStream_n2i0;
static bool first_SsThresh_n2i0 = true;

static Ptr<OutputStreamWrapper> cWndStream_n3i0;
static Ptr<OutputStreamWrapper> ssThreshStream_n3i0;
static bool first_SsThresh_n3i0 = true;

static void
Cwnd_n2i0_Change (uint32_t oldval, uint32_t newval)
{
  *cWndStream_n2i0->GetStream () << Simulator::Now ().GetSeconds () << " " << (double)newval/1024 << std::endl;
}

static void
SsThresh_n2i0_Change (uint32_t oldval, uint32_t newval)
{
  if (first_SsThresh_n2i0)
  {   
     first_SsThresh_n2i0 = false;
     return;
  }
  *ssThreshStream_n2i0->GetStream () << Simulator::Now ().GetSeconds () << " " << (double)newval/1024 << std::endl;
}

static void
Cwnd_n3i0_Change (uint32_t oldval, uint32_t newval)
{
  *cWndStream_n3i0->GetStream () << Simulator::Now ().GetSeconds () << " " << (double)newval/1024 << std::endl;
}

static void
SsThresh_n3i0_Change (uint32_t oldval, uint32_t newval)
{
  if (first_SsThresh_n3i0)
  {
    first_SsThresh_n3i0 = false;
    return;
  }
  *ssThreshStream_n3i0->GetStream () << Simulator::Now ().GetSeconds () << " " << (double)(newval/1024) << std::endl;
}

static void
Trace_Cwnd_n2i0 (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream_n2i0 = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&Cwnd_n2i0_Change));
}

static void
Trace_Ssthresh_n2i0 (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream_n2i0 = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThresh_n2i0_Change));
}

static void
Trace_Cwnd_n3i0 (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream_n3i0 = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&Cwnd_n3i0_Change));
}

static void
Trace_SsThresh_n3i0 (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream_n3i0 = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThresh_n3i0_Change));
}
