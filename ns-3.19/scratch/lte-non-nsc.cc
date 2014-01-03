

/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 The university of Utah
 *
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
 *
 * Author: Binh Nguyen <binh@cs.utah.edu>
 */


/**
 * NOTE: This code doesn't support MULTIPLE ENODEBs yet.
 */
#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/lte-global-pathloss-database.h"
#include <math.h>
#include <map>

using namespace ns3;
 /*
  * Experiment topology:
  *     
  *             UE (n3) -------------------- ENB (n2) ==================== SPGW (n0) ==================== End-host (n1)
  *                                                 2:2                  0:3        0:2                 1:1
  *                             radio                  <1Gbps,15ms,1500>               <1Gbps,15ms,1500>
  */

NS_LOG_COMPONENT_DEFINE ("lte-non-nsc-single-ue");

#define KILO 1000
#define ONEBIL 1000000000


/**Simulation variables**/
double simTime = 100;	//simulation time for EACH application
double t = 0.0;
uint16_t isTcp = 1;
/**********************/

/**Topology variables**/
uint16_t numberOfUeNodes = 1;
uint16_t numberOfEnodebs = 1;
double distance = 1000.0;    //With enbTxPower=5, Noise=37 and UeTxPower=50 (NEED TO BE THAT HIGH TO GUARANTEE UPLINK FOR TCP ACK FLOW), noise=9, we have roughly 1000Kb/s downlink bandwidth.

//S1uLink characteristics.
std::string s1uLinkDataRate = "1Gb/s";
double  s1uLinkDelay = 0.015;
uint16_t s1uLinkMtu = 1500;

//p2pLink characteristics.
std::string p2pLinkDataRate = "1Gb/s";
double p2pLinkDelay = 0.015;
uint16_t p2pLinkMtu = 1500;
/**********************/

//Application vars
uint32_t numberOfPackets = 0;
uint32_t packetSize = 900;
std::string dataRate = "100Mb/s";

/**Flow monitor**/
std::ofstream tcpThroughput;
Ptr<ns3::FlowMonitor> monitor;
FlowMonitorHelper flowHelper;
double samplingInterval = 0.005;    /*getTcp() function invoke for each x second*/
uint16_t PUT_SAMPLING_INTERVAL = 50; /*sample a TCP throughput for each x received pkts*/

std::map<Ipv4Address, double> last_tx_time;
std::map<Ipv4Address, double> last_rx_time ;
std::map<Ipv4Address, double> last_tx_bytes ;
std::map<Ipv4Address, double> last_rx_bytes ;
std::map<Ipv4Address, double> tcp_delay ;
std::map<Ipv4Address, double> last_delay_sum ;
std::map<Ipv4Address, double> last_rx_pkts ;
std::map<Ipv4Address, double> last_tx_pkts ;
std::map<Ipv4Address, uint16_t> init_map ;
std::map<Ipv4Address, double> last_put_sampling_time;
/**sending flowS stats***/
std::map<Ipv4Address, double> meanTxRate_send;
std::map<Ipv4Address, double> meanRxRate_send;
std::map<Ipv4Address, double> meanTcpDelay_send;
std::map<Ipv4Address, uint64_t> numOfLostPackets_send;
std::map<Ipv4Address, uint64_t> numOfTxPacket_send;
Ptr<ns3::Ipv4FlowClassifier> classifier;
std::map <FlowId, FlowMonitor::FlowStats> stats;


/********* Ascii output files name *********/
static std::string DIR = "results/data/";
static std::string macro = DIR+"macro_output.dat";
static std::string put_send;
static std::string debugger = "debugger.dat";
static std::string overall = "overall.out";

/********wrappers**********/
static AsciiTraceHelper asciiTraceHelper;
Ptr<OutputStreamWrapper> put_send_wp;
Ptr<OutputStreamWrapper> macro_wp;
Ptr<OutputStreamWrapper> debugger_wp;
Ptr<OutputStreamWrapper> overall_wp;

/**Logging var**/
LogLevel level = (LogLevel) (LOG_LEVEL_ALL | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC);

/**************** Function prototypes **************/
static void getTcpPut();
static void init_wrappers();


void log_component_enable(){
    //Only enable logs for TCP experiments
	if (isTcp==1){
    	//*************Enable logs********************/
        /*Enable ALL components logging*/
        //lteHelper->EnableLogComponents();

        /*Uncomment to enable component logging*/
        // LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
        // LogComponentEnable("UdpEchoClientApplication",LOG_PREFIX_ALL);
        // LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
        // LogComponentEnable("UdpClient",LOG_LEVEL_INFO);
        // LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
        // LogComponentEnable("OnOffApplication",LOG_LEVEL_INFO);
    	// LogComponentEnable("PacketSink",LOG_LEVEL_INFO);
       LogComponentEnable("TcpTahoe", level);
       LogComponentEnable("TcpReno", level);
       LogComponentEnable("TcpNewReno", level);
       LogComponentEnable("TcpWestwood", level);
        // LogComponentEnable("RttEstimator",level);
        // LogComponentEnable("TcpSocketBase",level);
        // LogComponentEnable ("LteRlcUm", level);
        // LogComponentEnable ("LteRlcAm", level);
        // LogComponentEnable ("NscTcpSocketImpl",LOG_LEVEL_DEBUG);
        // LogComponentEnable("OnOffApplication",level);
        // LogComponentEnable("PacketSink",level);
        // LogComponentEnable ("LteHelper",level);
        // LogComponentEnable ("LteUeMac", level);
        // LogComponentEnable ("LteEnbMac", level);
        // LogComponentEnable ("LtePdcp", level);
        // LogComponentEnable ("LtePhy", level);
	}
}

int
main (int argc, char *argv[])
{
    // Command line arguments
    CommandLine cmd;
    cmd.AddValue("numberOfUeNodes", "Number of UeNodes", numberOfUeNodes);
    cmd.AddValue("numberOfEnodebs", "Number of eNodebs", numberOfEnodebs);
    cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
    cmd.AddValue("distance", "Distance between an UE and Enodeb (also depend on UE allocation mode)[m]", distance);
    cmd.AddValue("numberOfPackets", "Number of packets to send, 0 to send unlimited number of packets", numberOfPackets);
    cmd.AddValue("packetSize", "Size of each packet", packetSize);
    cmd.AddValue("s1uLinkDataRate", "S1u Link Data Rate", s1uLinkDataRate);
    cmd.AddValue("s1uLinkDelay", "S1u Link Delay", s1uLinkDelay);
    cmd.AddValue("s1uLinkMtu", "S1u Link Mtu", s1uLinkMtu);
    cmd.AddValue("p2pLinkDataRate", "p2p Link Data Rate", p2pLinkDataRate);
    cmd.AddValue("p2pLinkDelay", "p2p Link Delay", p2pLinkDelay);
    cmd.AddValue("p2pLinkMtu", "p2p Link Mtu", p2pLinkMtu);
    cmd.AddValue("dataRate", "TCP application sending rate", dataRate);
    cmd.AddValue("isTcp", "TCP application if true, Udp if false", isTcp);


    /**ConfigStore setting, default parameters are set by this*/
    Config::SetDefault("ns3::ConfigStore::Filename", StringValue("lte-non-nsc.in"));
    Config::SetDefault("ns3::ConfigStore::FileFormat", StringValue("RawText"));
    Config::SetDefault("ns3::ConfigStore::Mode", StringValue("Load"));
    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults();
    inputConfig.ConfigureAttributes();
    
    cmd.Parse(argc, argv);
    
    /*Initialized wrappers*/
    init_wrappers();
    /*Enable logs*/
    log_component_enable();

    //************lteHeper, epcHelper**************//
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
    Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
    lteHelper->SetEpcHelper (epcHelper);
    lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");   //Set MAC scheduler. Refer to NS-3 for other MAC schedulers.
    
    //*********************Use epcHelper to get the PGW node********************//
    Ptr<Node> pgw = epcHelper->GetPgwNode ();
    epcHelper->SetAttribute("S1uLinkDataRate", DataRateValue (DataRate (s1uLinkDataRate)));
    epcHelper->SetAttribute("S1uLinkDelay", TimeValue (Seconds (s1uLinkDelay)));
    epcHelper->SetAttribute("S1uLinkMtu", UintegerValue (s1uLinkMtu));

    //***********Create a single RemoteHost, install the Internet stack on it*************//
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create (1);
    Ptr<Node> remoteHost = remoteHostContainer.Get (0);
    //Install Internet stack on the remoteHost.
    InternetStackHelper internet;
    internet.Install (remoteHost);

    //***************Create and install a point to point connection between the SPGW and the remoteHost*****************//
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (p2pLinkDataRate)));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (p2pLinkMtu));
    p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (p2pLinkDelay)));
    NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);		//The interfaces between the SPGW and remoteHost were saved in internetDevices.

    // Create the Internet
    Ipv4AddressHelper ipv4h;	//Ipv4AddressHelper is used to assign Ip Address for a typical node.
    ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);		//assign IP addresses in starting at "1.0.0.0" to the SPGW and remoteHost.
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

    //***************************Let the remoteHost know how to route to UE "7.0.0.0"**************************//
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());	//remoteHostStaticRouting now knows how to route to the remoteHost.
    remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);	//Add the routing entry to the remoteHostStaticRouting table.

    //**********************************Create Ue nodes, EnodeBs*******************************//
    NodeContainer ueNodes;
    NodeContainer enbNodes;
    enbNodes.Create(numberOfEnodebs);
    ueNodes.Create(numberOfUeNodes);


    //=============================eNB allocation=================//
    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    enbMobility.SetPositionAllocator ("ns3::GridPositionAllocator",
		    "MinX", DoubleValue (0.0),  //zero point of the grid
		    "MinY", DoubleValue (0.0),  //zero point of the grid
		    "DeltaX", DoubleValue (1000.0),  //distance among ENB nodes in meters.
		    "DeltaY", DoubleValue (1000.0),
		    "GridWidth", UintegerValue (3), //number of nodes on a row
		    "LayoutType", StringValue ("RowFirst"));
    enbMobility.Install (enbNodes); /*===ENB #1 placed at (0.0)====*/
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
    NetDeviceContainer::Iterator enbLteDevIt = enbLteDevs.Begin ();
    Vector enbPosition = (*enbLteDevIt)->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();

    //=========================UEs allocation=======================//
    MobilityHelper ueMobility;
	*debugger_wp->GetStream() << "Allocating UEs with FIXED positions ....\n";
	ueMobility.SetPositionAllocator ("ns3::GridPositionAllocator",
		    "MinX", DoubleValue (distance/sqrt(2)),  //The first UE is put at "distance" meters away from eNB.
		    "MinY", DoubleValue (distance/sqrt(2)),  
		    "DeltaX", DoubleValue (100.0),  //distance among UE nodes
		    "DeltaY", DoubleValue (100.0),
		    "GridWidth", UintegerValue (3), //number of nodes on row
		    "LayoutType", StringValue ("RowFirst"));

    //===========================UEs mobility: stationary=====================//
    ueMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    *debugger_wp->GetStream() << "MOBILITY: ConstantPositionMobilityModel .... \n";
    
    //Install mobility model into UEs. 
    ueMobility.Install (ueNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);   
                                
                                                                                               
    //**********************Assign Ipv4 addresses for UEs. Install the IP stack on the UEs******************//
    internet.Install (ueNodes);	
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
    //UEs routing to SGW
    for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
        Ptr<Node> ueNode = ueNodes.Get (u);
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
        ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }


    //**********************Attach all UEs to eNodeB**********************//
    for (uint16_t i = 0; i < numberOfEnodebs; i++)
    {
        for (uint16_t t = 0; t < numberOfUeNodes; ++t){
            lteHelper->Attach (ueLteDevs.Get(t), enbLteDevs.Get(i));    
        }
    }

    //*****************************Install and start applications on UEs and the remote host****************************//
    uint16_t dlPort = 2000; //downlink port number
    uint16_t ulPort = 7000; //uplink port number
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;
    for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
        ++dlPort;				//each Ue will contact with the remoteHost by a different dlPort (the remoteHost needs this).
        ++ulPort;
        
        if (isTcp == 1){
					/*********TCP Application********/
					//Create a packet sink to receive packet on remoteHost
     			    //LogComponentEnable("Queue",level);    //Only enable Queue monitoring for TCP to accelerate experiment speed.
					PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), dlPort));
					serverApps.Add(sink.Install(ueNodes.Get(u)));
					OnOffHelper onOffHelper("ns3::TcpSocketFactory", Address ( InetSocketAddress(ueIpIface.GetAddress(u) , dlPort) ));
					onOffHelper.SetConstantRate( DataRate(dataRate), packetSize );
					if (numberOfPackets != 0) 
						onOffHelper.SetAttribute("MaxBytes",UintegerValue(packetSize*numberOfPackets));
					clientApps.Add(onOffHelper.Install(remoteHost));
        }
        else{
					PUT_SAMPLING_INTERVAL = PUT_SAMPLING_INTERVAL*40;
					/*********UDP Application********/
					//Create a packet sink to receive packet on remoteHost
					PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), dlPort));
					serverApps.Add(sink.Install(ueNodes.Get(u)));
                    //Downlink Udp.
					OnOffHelper onOffHelper("ns3::UdpSocketFactory", Address ( InetSocketAddress(ueIpIface.GetAddress(u), dlPort) ));
					onOffHelper.SetConstantRate( DataRate(dataRate), packetSize );
					if (numberOfPackets != 0)
						onOffHelper.SetAttribute("MaxBytes",UintegerValue(packetSize*numberOfPackets));
					clientApps.Add(onOffHelper.Install(remoteHost));

                    //Uplink Udp.
					PacketSinkHelper ul_sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), ulPort));
					serverApps.Add(ul_sink.Install(remoteHost));
					OnOffHelper ul_onOffHelper("ns3::UdpSocketFactory", Address ( InetSocketAddress(remoteHostAddr, ulPort) ));
					ul_onOffHelper.SetConstantRate( DataRate(dataRate), packetSize );
					if (numberOfPackets != 0)
						onOffHelper.SetAttribute("MaxBytes",UintegerValue(packetSize*numberOfPackets));
					clientApps.Add(ul_onOffHelper.Install(ueNodes.Get(u)));
        }

    }

    /*******************Start client and server apps***************/
    serverApps.Start (Seconds (0.01));		//All remote hosts start at 0.01s.
    clientApps.Start(Seconds(0.5));         //Clients start a bit later after the remote hosts are ready.
    
    
    /*********LTE layers trace***************/
    lteHelper->EnableTraces ();
    lteHelper->GetPdcpStats()->SetAttribute("EpochDuration", TimeValue( Seconds (0.010)) );		//set collection interval for PDCP.
    lteHelper->GetRlcStats()->SetAttribute("EpochDuration", TimeValue ( Seconds (0.010)))	;		//same for RLC
 

    //Enable wireshark for all interfaces
    if (isTcp==1)
    	p2ph.EnablePcapAll(DIR+"pcap/lte-non-nsc-single-ue");

    /**Set up the flow monitor to get end-to-end throughput, delay*/
    monitor = flowHelper.Install(ueNodes);
    monitor = flowHelper.Install(remoteHost);
    monitor = flowHelper.GetMonitor();


    /****ConfigStore setting****/
    Config::SetDefault("ns3::ConfigStore::Filename", StringValue("lte-non-nsc.out"));
    Config::SetDefault("ns3::ConfigStore::FileFormat", StringValue("RawText"));
    Config::SetDefault("ns3::ConfigStore::Mode", StringValue("Save"));
    ConfigStore outputConfig;
    outputConfig.ConfigureDefaults();
    outputConfig.ConfigureAttributes();

  
    /*=============schedule to get TCP throughput============*/
    Time t = Seconds(0.0);
    Simulator::ScheduleWithContext (0 ,Seconds (0.0), &getTcpPut);


    /*********Start the simulation*****/
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    

    /**************Simulation stops here. Start printing out information (if needed)***********/   
  
  monitor->CheckForLostPackets();
  Ptr<ns3::Ipv4FlowClassifier> classifier = DynamicCast<ns3::Ipv4FlowClassifier> (flowHelper.GetClassifier());
  std::map <FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

  /**Get the average flow goodput, delay, lost ratio, etc at the end of a simulation**/
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter != stats.end(); ++iter){
    ns3::Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);
    /*sending flows, from endhost (1.0.0.2:49153) to Ues (7.0.0.2:200x)*/
      if (iter->second.rxPackets > 1){
        meanTxRate_send[t.sourceAddress] = 8*iter->second.txBytes/(iter->second.timeLastTxPacket.GetDouble()-iter->second.timeFirstTxPacket.GetDouble())*ONEBIL/(1024);
        meanRxRate_send[t.sourceAddress] = 8*iter->second.rxBytes/(iter->second.timeLastRxPacket.GetDouble()-iter->second.timeFirstRxPacket.GetDouble())*ONEBIL/(1024);
        meanTcpDelay_send[t.sourceAddress] = iter->second.delaySum.GetDouble()/iter->second.rxPackets/1000000;
      }
      numOfLostPackets_send[t.sourceAddress] = iter->second.lostPackets; 
      numOfTxPacket_send[t.sourceAddress] = iter->second.txPackets;
  
    *macro_wp->GetStream() << "***Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << ":" << t.sourcePort << " Dst Addr " << t.destinationAddress << ":" << t.destinationPort  << std::endl
    << "Tx Packets " << iter->second.txPackets << std::endl
    << "Rx Packets " << iter->second.rxPackets << std::endl
    << "Lost packets " << iter->second.lostPackets << std::endl
    << "Lost ratio " << double (iter->second.lostPackets)/(iter->second.lostPackets+iter->second.rxPackets) << std::endl;
    if (iter->second.rxPackets > 1){
        *macro_wp->GetStream() << "Average delay received " << iter->second.delaySum/iter->second.rxPackets/1000000 << std::endl
        << "Mean received bitrate " << 8*iter->second.rxBytes/(iter->second.timeLastRxPacket-iter->second.timeFirstRxPacket)*ONEBIL/(1024) << std::endl
        << "Mean transmitted bitrate " << 8*iter->second.txBytes/(iter->second.timeLastTxPacket-iter->second.timeFirstTxPacket)*ONEBIL/(1024) << std::endl;
    }

    *overall_wp->GetStream()  << "***Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << ":" << t.sourcePort << " Dst Addr " << t.destinationAddress << ":" << t.destinationPort  << std::endl
    << "Tx Packets " << iter->second.txPackets << std::endl
    << "Rx Packets " << iter->second.rxPackets << std::endl
    << "Lost packets " << iter->second.lostPackets << std::endl
    << "Lost ratio " << double (iter->second.lostPackets)/(iter->second.lostPackets+iter->second.rxPackets) << std::endl;
    if (iter->second.rxPackets > 1){
        *overall_wp->GetStream() << "Average delay received " << iter->second.delaySum/iter->second.rxPackets/1000000 << std::endl
        << "Mean received bitrate " << 8*iter->second.rxBytes/(iter->second.timeLastRxPacket-iter->second.timeFirstRxPacket)*ONEBIL/(1024) << std::endl
        << "Mean transmitted bitrate " << 8*iter->second.txBytes/(iter->second.timeLastTxPacket-iter->second.timeFirstTxPacket)*ONEBIL/(1024) << std::endl;
    }

  }

  Simulator::Destroy();
  return 0;
    
}

/*
 *Get Tcp's goodput, delay, lost rate, etc DURING a simulation (instantaneous values)
 *This function will be call periodically during a simulation.
 *Values are put in tcp-put.dat
 */
static void
getTcpPut(){
    monitor->CheckForLostPackets();
    classifier = DynamicCast<ns3::Ipv4FlowClassifier> (flowHelper.GetClassifier());
    stats = monitor->GetFlowStats();

    /*==============Get flows information============*/
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter != stats.end(); ++iter){
    ns3::Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);
	if (init_map[t.destinationAddress] != 1){
		init_map[t.destinationAddress] = 1;
		meanTxRate_send[t.destinationAddress] = 0;
		meanRxRate_send[t.destinationAddress] = 0;
		last_tx_time[t.destinationAddress] = 0;
		last_tx_bytes[t.destinationAddress] = 0;
		last_rx_time[t.destinationAddress] = 0;
		last_rx_bytes[t.destinationAddress] = 0;
		meanTcpDelay_send[t.destinationAddress] = 0;
		last_tx_pkts[t.destinationAddress] = 0;
		last_put_sampling_time[t.destinationAddress] = 0;
		tcp_delay[t.destinationAddress] = 0;
		last_delay_sum[t.destinationAddress] = 0;
		last_rx_pkts[t.destinationAddress] = 0;
	}
 	/*sending/receiving rate*/
    if (iter->second.txPackets > last_tx_pkts[t.destinationAddress] + PUT_SAMPLING_INTERVAL && iter->second.timeLastTxPacket > last_tx_time[t.destinationAddress]){
        	meanTxRate_send[t.destinationAddress] = 8*(iter->second.txBytes-last_tx_bytes[t.destinationAddress])/(iter->second.timeLastTxPacket.GetDouble()-last_tx_time[t.destinationAddress])*ONEBIL/KILO;
		meanRxRate_send[t.destinationAddress] = 8*(iter->second.rxBytes-last_rx_bytes[t.destinationAddress])/(iter->second.timeLastRxPacket.GetDouble()-last_rx_time[t.destinationAddress])*ONEBIL/KILO;
		last_tx_time[t.destinationAddress] = iter->second.timeLastTxPacket.GetDouble();
		last_tx_bytes[t.destinationAddress] = iter->second.txBytes;
		last_rx_time[t.destinationAddress] = iter->second.timeLastRxPacket.GetDouble();
		last_rx_bytes[t.destinationAddress] = iter->second.rxBytes;      
		last_tx_pkts[t.destinationAddress] = iter->second.txPackets;
		last_put_sampling_time[t.destinationAddress] = Simulator::Now().GetSeconds();
	}
    numOfLostPackets_send[t.destinationAddress] = iter->second.lostPackets;
	/*end-to-end delay sampling*/
    if (iter->second.rxPackets > last_rx_pkts[t.destinationAddress]){
    		    tcp_delay[t.destinationAddress] = (iter->second.delaySum.GetDouble() - last_delay_sum[t.destinationAddress]) / (iter->second.rxPackets - last_rx_pkts[t.destinationAddress])/(KILO*KILO);
    		    last_delay_sum[t.destinationAddress] = iter->second.delaySum.GetDouble();
    		    last_rx_pkts[t.destinationAddress] = iter->second.rxPackets; 
	}

      numOfTxPacket_send[t.destinationAddress] = iter->second.txPackets;
  }
    std::map<Ipv4Address,double>::iterator it1 = meanRxRate_send.begin();
    std::map<Ipv4Address,uint64_t>::iterator it3 = numOfLostPackets_send.begin();
    std::map<Ipv4Address,uint64_t>::iterator it4 = numOfTxPacket_send.begin();
    std::map<Ipv4Address,double>::iterator it5 = meanTxRate_send.begin();
    std::map<Ipv4Address,double>::iterator it6 = tcp_delay.begin();

    for (;it1 != meanRxRate_send.end(); ){
        *put_send_wp->GetStream() << Simulator::Now().GetSeconds() << "\t\t"
                  << (*it1).first << "\t\t"
                  << (*it1).second << "\t\t"
                  << "x" << "\t\t"
                  << (*it3).second << "\t\t"
                  << (*it4).second << "\t\t"
                  << "x" << "\t\t"
                  << "x" << "\t\t"
                  << "x" << "\t\t"
                  << "x" << "\t"
            	  << (*it5).second << "\t"
            	  << (*it6).second <<  "\n";
        ++it1;
        ++it3;
        ++it4;
        ++it5;
        ++it6;
    }
	
    while (t < simTime){
        t += samplingInterval;
        Simulator::Schedule(Seconds(t),&getTcpPut);
    }
}

/*
 *This function creates output files for the simulation
 */
static void 
init_wrappers(){
    /* create files for wrappers */
    debugger_wp = asciiTraceHelper.CreateFileStream(debugger);
    overall_wp = asciiTraceHelper.CreateFileStream(overall, std::ios::app);
    //********************Initialize wrappers*********************/
    if (isTcp==1){
      put_send = DIR + "tcp-put.dat";
      macro = DIR + "macro_tcp.dat";
    } else{
      put_send = DIR + "udp-put.dat";
      macro = DIR + "macro_udp.dat";
    }
    macro_wp = asciiTraceHelper.CreateFileStream(macro);
    put_send_wp = asciiTraceHelper.CreateFileStream(put_send);
    *put_send_wp->GetStream() << "#DestinationIp\t"
                  << "Time\t"
                  << "Send Tcp throughput\t"
                  << "Send Tcp delay\t"
                  << "Number of Lost Pkts\t"
                  << "Number of Tx Pkts\t"
                  << "ErrorUlTx\t"
                  << "ErrorDlTx\t"
                  << "HarqUlTx\t"
                  << "HarqDlTx\n";
}
