#include <iostream>
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
//#include "ns3/buildings-module.h"
#include <ns3/buildings-module.h>
#include "ns3/netanim-module.h"

#include "ns3/flow-monitor-module.h"
#include "ns3/log.h"
#include <sys/timeb.h>
#include <ns3/internet-trace-helper.h>
#include <ns3/spectrum-module.h>
#include <ns3/log.h>
#include <ns3/string.h>
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("end2end");

void
PrintGnuplottableUeListToFile (std::string filename)
{
	std::ofstream outFile;
	outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
	if (!outFile.is_open ())
	{
		NS_LOG_ERROR ("Can't open file" << filename);
		return;
	}
	for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
	{
		Ptr<Node> node = *it;
		int nDevs = node->GetNDevices ();
		for (int j = 0; j < nDevs; j++)
		{
			Ptr<LteUeNetDevice> uedev = node->GetDevice (j)->GetObject<LteUeNetDevice>();
			if (uedev)
			{
				Vector pos = node->GetObject<MobilityModel>()->GetPosition();
				outFile << "set label \"" << uedev->GetImsi ()
						<< "\" at" << pos.x << "," << pos.y << " left font \"Helvetica,4\" textcolor rgb \"grev\" front point pt 1 ps 0.3 lc rgb \"grey\" offset 0,0"
						<< std::endl;
			}
		}
	}
}

void
PrintGnuPlottableEnabListToFile (std::string filename)
{
	std::ofstream outFile;
	outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
	if(!outFile.is_open ())
	{
		NS_LOG_ERROR ("Can't open file" << filename);
		return;
	}
	for (NodeList::Iterator it = NodeList::Begin (); it!=NodeList::End();++it)
	{
		Ptr<Node> node = *it;
		int nDevs = node->GetNDevices();
		for (int j= 0; j < nDevs; j++)
		{
			Ptr<LteEnbNetDevice> enbdev = node->GetDevice(j)->GetObject<LteEnbNetDevice>();
			if (enbdev)
			{
				Vector pos = node->GetObject<MobilityModel>()->GetPosition ();
				outFile << "set label \"" << enbdev->GetCellId()
						<< "\" at " << pos.x << "." << pos.y
						<< " left font \"Helvetica,4\" textcolor rgb \"white\" front point pt2 ps 0.3 lc rgb \"white\" offset 0,0"
						<< std::endl;
			}
		}
	}
}

/////////////////////////////////////////////////////////

AnimationInterface * pAnim = 0;

struct rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct rgb color [] = {
		{255, 0, 0},//red
		{0, 255, 0},//blue
		{0, 0, 255}//green
};

uint32_t resourceId1;
uint32_t resourceId2;
uint32_t resourceId3;
uint32_t resourceId4;
uint32_t resourceId5;
uint32_t resourceId6;

void modify ()
{
	std::ostringstream oss;
	oss << "Update:" << Simulator::Now ().GetSeconds ();
	pAnim->UpdateLinkDescription(0, 1, oss.str ());
	pAnim->UpdateLinkDescription(0, 2, oss.str ());
	pAnim->UpdateLinkDescription(0, 3, oss.str ());
	pAnim->UpdateLinkDescription(0, 4, oss.str ());
	pAnim->UpdateLinkDescription(0, 5, oss.str ());
	pAnim->UpdateLinkDescription(0, 6, oss.str ());
	pAnim->UpdateLinkDescription(1, 7, oss.str ());
	pAnim->UpdateLinkDescription(1, 8, oss.str ());
	pAnim->UpdateLinkDescription(1, 9, oss.str ());
	pAnim->UpdateLinkDescription(1, 11, oss.str ());

	//Every update change the node descrition for node 2
	std::ostringstream node0Oss;
	node0Oss << "_____Node:" << Simulator::Now().GetSeconds();
	pAnim->UpdateNodeDescription (2, node0Oss.str ());
	static uint32_t currentResourceId = resourceId1;
	static uint32_t currentResourceId2 = resourceId2;
	static uint32_t currentResourceId3 = resourceId3;
	static uint32_t currentResourceId4 = resourceId4;
	static uint32_t currentResourceId5 = resourceId5;
	static uint32_t currentResourceId6 = resourceId6;

	pAnim->UpdateNodeSize(1, 150, 70);
	pAnim->UpdateNodeImage(1, currentResourceId4);
	pAnim->UpdateNodeSize(0,150,250);
	pAnim->UpdateNodeImage(0, currentResourceId3);

	for (uint16_t i = 2; i < 5; i++)
	{
		pAnim->UpdateNodeSize (i, 70, 70);
		pAnim->UpdateNodeImage (i, currentResourceId);
	}

	for (uint16_t i = 5; i < 7; i++)
	{
		pAnim->UpdateNodeSize (i, 50, 30);
		pAnim->UpdateNodeImage (i, currentResourceId2);
	}

	for (uint16_t i = 7; i < 10; i++)
	{
		pAnim->UpdateNodeSize (i, 70, 30);
		pAnim->UpdateNodeImage (i, currentResourceId6);
	}
	for (uint16_t i = 10; i < 15; i++)
	{
		pAnim->UpdateNodeSize (i, 50, 50);
		pAnim->UpdateNodeImage (i, currentResourceId5);
	}

	/*for (uint16_t i = 5; i < 15; i++)
	{
		pAnim->UpdateNodeSize (i, 20, 20);
		pAnim->UpdateNodeImage (i, currentResourceId);
	}*/

	//Every update change the color for node 4
	if (Simulator::Now().GetSeconds() < 10) //this is important on the simulation
		//will run endlessly
		Simulator::Schedule(Seconds (0.05), modify);
}


int main(int argc, char *argv[])
{
	Config::SetDefault ("ns3::LteSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (true));
	Config::SetDefault ("ns3::LteSpectrumPhy::DataErrorModelEnabled", BooleanValue (true));
	Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));
	Config::SetDefault ("ns3::LteHelper::UsePdschForCqiGeneration", BooleanValue (true));

	//Uplink Power Control
	Config::SetDefault ("ns3::LteUePhy::EnableUplinkPowerControl", BooleanValue (true));
	Config::SetDefault ("ns3::LteUePowerControl::ClosedLoop", BooleanValue (true));
	Config::SetDefault ("ns3::LteUePowerControl::AccumulationEnabled", BooleanValue (true));

	Time::SetResolution (Time::NS);
	LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

	uint16_t numberOfNodesENB = 2; //the number of enodeB
	uint16_t numberOfNodesEU = 2400;
	double simTime = 0.05;
	double distance = 500.0;
	double interPacketInterval = 1.0;

	//std::string animFile = "lte-4G.xml"; //output animFile

	CommandLine cmd;
	cmd.AddValue("numberOfNodes", "Number of eNodeBs + UE pairs", numberOfNodesENB);
	cmd.AddValue("simTime", "Total Duration of the simulation[s]", simTime);
	cmd.AddValue("distance", "Distance between eNBs [m]", distance);
	cmd.AddValue("inerPacketInterval", "Inter packet interval [ms]", interPacketInterval);
	cmd.AddValue("animFile", "File Name for Animation Output", animFile);
	cmd.Parse (argc, argv);
	ConfigStore inputConfig;
	inputConfig.ConfigureDefaults ();
	cmd.Parse (argc, argv);

	//creation object letHelper
	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

	lteHelper->SetAttribute ("FadingModel", StringValue("ns3::TraceFadingLossModel"));

	std::ifstream ifTraceFile;
	ifTraceFile.open ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad", std::ifstream::in);
	if(ifTraceFile.good())
	{
		//script launched by test.py
		lteHelper->SetFadingModelAttribute("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
	}
	else
	{
		//script launched as an example

		lteHelper->SetFadingModelAttribute("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
	}

    
    Ptr<LteHelper> mmeHelper = CreateObject<mmeHelper> ();
    for(int i = 0; i < numberOfNodesEU; i++){
        mmeHelper.attach(UE.get(i));
    }


    
	//These parameters have to setted only in case of the trace format
	//differs from the standard one, that is
	// -10 seconds length trace
	//- 10,000 samples
	// -0.5 seconds for window size
	// -46 RB
	lteHelper->SetFadingModelAttribute("TraceLength", TimeValue (Seconds (10.0)));
	lteHelper->SetFadingModelAttribute("SamplesNum", UintegerValue(10000));
	lteHelper->SetFadingModelAttribute("WindowSize", TimeValue (Seconds (0.5)));
	lteHelper->SetFadingModelAttribute("RbNum", UintegerValue(100));

	lteHelper->SetEnbDeviceAttribute("DlEarfcn",UintegerValue(100));
	lteHelper->SetEnbDeviceAttribute("UlEarfcn",UintegerValue(18000));

	//Creation de l object epcHepler
	Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
	lteHelper->SetEpcHelper(epcHelper);

	Ptr<Node> pgw = epcHelper->GetPgwNode(); // p-gateway

	//creation RemoteHost
	//container
	NodeContainer remoteHostContainer;
	remoteHostContainer.Create(1);
	Ptr<Node> remoteHost = remoteHostContainer.Get(0);
	InternetStackHelper internet; //inter connection protocol
	internet.Install (remoteHostContainer);

	Ptr<ListPositionAllocator> positionAlloc1 = CreateObject<ListPositionAllocator>();
	positionAlloc1->Add(Vector(500.0, -100.0, 20.0));  //move?
	MobilityHelper mobility1;
	mobility1.SetPositionAllocator (positionAlloc1);
	mobility1.Install (remoteHostContainer);//remote host and mobility1

	//Create the Internet
	PointToPointHelper p2ph;
	p2ph.SetDeviceAttribute("DataRate", DataRateValue (DataRate ("150Mb/s"))); //
	p2ph.SetDeviceAttribute("Mtu", UintegerValue (1500));
	p2ph.SetChannelAttribute("Delay", TimeValue (Seconds (0.010)));
	NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);//connection? yes!
	Ipv4AddressHelper ipv4h;
	ipv4h.SetBase("1.0.0.0", "255.0.0.0");
	Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices); //assign ip address to pgw and remote host

	//interface 0 is localhost, 1 is the p2pdevice
	Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

	Ipv4StaticRoutingHelper ipv4RoutingHelper;   //route
	Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4>());
	remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask ("255.0.0.0"),1);  //static routing

	//creation the EnodeB for ENB at UE
	NodeContainer enbNodes;
	enbNodes.Create(numberOfNodesENB);  // create EnodeB is here!
	NodeContainer ueNodes;
	ueNodes.Create(numberOfNodesEU);

	//to configure the position and movement of nodes
	double x_min = 0.0;
	double x_max = 10.0;
	double y_min = 0.0;
	double y_max = 20.0;
	double z_min = 0.0;
	double z_max = 10.0;

	Ptr<Building> b = CreateObject <Building>();
	b->SetBoundaries(Box (x_min , x_max, y_min, y_max, z_min, z_max));
	b->SetBuildingType(Building::Residential);
	b->SetExtWallsType(Building::ConcreteWithWindows);
	b->SetNFloors(3);
	b->SetNRoomsX(3);
	b->SetNRoomsY(2);

	Ptr<GridBuildingAllocator> gridBuildingAllocator;
	gridBuildingAllocator = CreateObject <GridBuildingAllocator> ();
	gridBuildingAllocator->SetAttribute("GridWidth", UintegerValue(3));
	gridBuildingAllocator->SetAttribute("LengthX", DoubleValue(7));
	gridBuildingAllocator->SetAttribute("LengthY", DoubleValue(13));
	gridBuildingAllocator->SetAttribute("DeltaX", DoubleValue(3));
	gridBuildingAllocator->SetAttribute("DeltaY", DoubleValue(3));
	gridBuildingAllocator->SetAttribute("Height", DoubleValue(3));
	gridBuildingAllocator->SetBuildingAttribute("NRoomsX", UintegerValue(2));
	gridBuildingAllocator->SetBuildingAttribute("NRoomsY", UintegerValue(4));
	gridBuildingAllocator->SetBuildingAttribute("NFloors", UintegerValue(2));
	gridBuildingAllocator->SetAttribute("MinX", DoubleValue(0));
	gridBuildingAllocator->SetAttribute("MinY", DoubleValue(0));
	gridBuildingAllocator->Create(6);

	MobilityHelper mobility;
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.SetPositionAllocator("ns3::GridPositionAllocator",
			"MinX", DoubleValue(-350.0),
			"MinY", DoubleValue(-200.0),
			"DeltaX", DoubleValue(0.0),
			"DeltaY", DoubleValue(150.0),
			"GridWidth", UintegerValue (1),
			"LayoutType",  StringValue ("RowFirst"));

	mobility.Install (enbNodes); //node enodeB mobility
	BuildingsHelper::Install (enbNodes); //install the enodeB the building!!

	//ue mobility
	for (uint16_t i = 0; i< numberOfNodesEU; i++)
	{
		//mobility of the UE
		mobility.SetPositionAllocator("ns3::GridPositionAllocator",
				"MinX", DoubleValue(-900.0),
				"MinY", DoubleValue(-250.0),
				"DeltaX", DoubleValue(120.0),
				"DeltaY", DoubleValue(150.0),
				"GridWidth", UintegerValue (4),
				"LayoutType",  StringValue ("RowFirst"));
		mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
				"Mode", StringValue ("Time"),
				"Time", StringValue ("0.5s"),
				"Speed", StringValue ("ns3::ConstantRandomVariable[Constant=50.0]"),
				"Bounds", RectangleValue (Rectangle(-12000.0, 12000.0, -12000.0, 12000.0)));
	}
	mobility.Install (ueNodes);
	AsciiTraceHelper ascii;
	MobilityHelper::EnableAsciiAll (ascii.CreateFileStream("mobility-trace-example.mob"));

	//enodeb protocol to UE, which is very important
	NetDeviceContainer enbDevs;
	enbDevs = lteHelper->InstallEnbDevice(enbNodes);
	NetDeviceContainer ueDevs;
	ueDevs = lteHelper->InstallUeDevice(ueNodes);
	//BuildingsHelper::MakeMobilityModelConsistent();

	//Install the IP stack on the UEs
	internet.Install (ueNodes);
	Ipv4InterfaceContainer ueIpIface;
	ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueDevs));

	//Assign IP address to UEs, and install applications
	for(uint32_t u=0; u < ueNodes.GetN(); ++u)
	{
		Ptr<Node> ueNode = ueNodes.Get(u);
		//Set the default gateway for the UE
		Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4>());
		ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress(),1);
	}

	//for attach ue to enodeb
	//lteHelper->Attach(ueDevs, enbDevs, Get(0));
	uint16_t j=0;
	for (uint16_t i = 0; i < numberOfNodesEU; i++)
	{
		if(j < numberOfNodesENB)
		{
			lteHelper->Attach (ueDevs.Get(i), enbDevs.Get(j));
			j++;
		}
		else
		{
			j = 0;
			lteHelper->Attach(ueDevs.Get(i), enbDevs.Get(j));
		}
	}

	// Attach all UEs to the closest EnodeB
	//lteHelper->AttachToClosestEnb (ueDevs, enbDevs);
	//Add X2 interface
	//LteHelper -> addX2Interface (enbNodes);
	//X2-based Handover
	//lteHelper->HandoverRequest (Seconds (0.100), ueDevs.Get(0), enbDevs.Get(0), enbDevs.Get(1));

	//to activate the radio support that carries the dennes between ueDevs and EnbDevs

	//??
	Ptr<EpcTft> tft = Create<EpcTft> ();
	EpcTft::PacketFilter pf;
	pf.localPortStart = 1234;
	pf.localPortEnd = 1234;
	tft->Add(pf);
	lteHelper->ActivateDedicatedEpsBearer(ueDevs, EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT), tft);//NGBR video

	uint16_t dlPort = 1234; //down link
	uint16_t ulPort = 2000; //up link
	uint16_t otherPort = 3000; //other link

	ApplicationContainer clientApps;
	ApplicationContainer serverApps;

	for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
	{
		++ulPort;
		++otherPort;
		PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
		PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
		PacketSinkHelper PacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));

		serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(u)));
		serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
		serverApps.Add (PacketSinkHelper.Install (ueNodes.Get(u)));

		UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
		dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
		dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

		UdpClientHelper ulClient (remoteHostAddr, ulPort);
		ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
		ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));


		UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
		client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
		client.SetAttribute ("MaxPackets", UintegerValue(1000000));

		clientApps.Add(dlClient.Install (remoteHost));
		clientApps.Add(ulClient.Install(ueNodes.Get(u)));

		if(u+1 < ueNodes.GetN())
		{
			clientApps.Add (client.Install(ueNodes.Get(u+1)));
		}
		else
		{
			clientApps.Add (client.Install (ueNodes.Get (0)));
		}
	}

	// install and start application on UEs and remote host
	serverApps.Start(Seconds (0.01));
	clientApps.Start(Seconds (0.01));
	//lteHelper-> enableTraces();

	Simulator::Stop (Seconds (simTime));
	/*lteHelper->SetFadingModel("ns3::TraceFadingLossModel");
	 *
	 * lteHelper->SedFadingModelAttribute ("TraceFileName", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
	 * lteHelper->SedFadingModelAttribute ("TraceLength", TimeValue (Seconds(10.0)));
	 * lteHelper->SedFadingModelAttribute ("SamplesNUm", UintegerValue (10000));
	 * lteHelper->SedFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
	 * lteHelper->SedFadingModelAttribute ("RbNum", UintegerValue (100));
	 */

	Config::SetDefault("ns3::LteAmc::AmcModel", EnumValue (LteAmc::PiroEW2010));
	Config::SetDefault("ns3::LteAmc::AmcModel", EnumValue (LteAmc::MiErrorModel));
	Config::SetDefault("ns3::LteAmc::Ber", DoubleValue (0.00005));

	lteHelper->EnablePhyTraces();
	lteHelper->EnableMacTraces();
	lteHelper->EnableRlcTraces();

	//Create the animation object and configure for the specified output
	pAnim = new AnimationInterface (animFile.c_str ()); //!!pay attations

	//Provide the absolute path to the resource
	resourceId2 = pAnim->AddResource("/home/ovsdev/Downloads/ns-allinone-3.26/ns-3.26/scratch/Police-car.png");
	resourceId1 = pAnim->AddResource("/home/ovsdev/Downloads/ns-allinone-3.26/ns-3.26/scratch/eNb.png");
	resourceId3 = pAnim->AddResource("/home/ovsdev/Downloads/ns-allinone-3.26/ns-3.26/scratch/serverepc.png");
	resourceId4 = pAnim->AddResource("/home/ovsdev/Downloads/ns-allinone-3.26/ns-3.26/scratch/RemoteHost.png");
	resourceId5 = pAnim->AddResource("/home/ovsdev/Downloads/ns-allinone-3.26/ns-3.26/scratch/Phone.png");
	resourceId6 = pAnim->AddResource("/home/ovsdev/Downloads/ns-allinone-3.26/ns-3.26/scratch/car.png");
	pAnim->SetBackgroundImage("/home/ovsdev/Downloads/ns-allinone-3.26/ns-3.26/scratch/4g-lte.jpg", -1, 1, 0.5, 0.5,1);

	Simulator::Schedule( Seconds (simTime), modify);

	//Create the animation object and configure for specified output
	//AnimationInterface anim (animFile.c_Str ());
	//anim.SetConstantPostion (remoteHost, 100, 200);
	//anim.SecConstantPostion (Ptr<Node> remoteHostContainer, double x=20, double y= 20));
	/*for (uint16_t i = 0; i< (numberOfNodeEu+numberOfNuodesENB+2); i++)
	 * {
	 * anim.UpdateNodeSize (i, 10.5, 10.5);
	 */

	Simulator::Run();
	Simulator::Destroy();
	delete pAnim;
	return 0;
}
