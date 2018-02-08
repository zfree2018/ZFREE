package anonymous.seclab;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;
import org.projectfloodlight.openflow.protocol.OFMessage;
import org.projectfloodlight.openflow.protocol.OFType;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.floodlightcontroller.core.FloodlightContext;
import net.floodlightcontroller.core.IFloodlightProviderService;
import net.floodlightcontroller.core.IOFMessageListener;
import net.floodlightcontroller.core.IOFSwitch;
import net.floodlightcontroller.core.module.FloodlightModuleContext;
import net.floodlightcontroller.core.module.FloodlightModuleException;
import net.floodlightcontroller.core.module.IFloodlightModule;
import net.floodlightcontroller.core.module.IFloodlightService;

public class Conn2Proxy2 implements IFloodlightModule, IOFMessageListener, HashFromProxy2s{

	public static enum SendMode {
		Full_Pkt, Buffer_ID
	}

	protected IFloodlightProviderService floodlightProvider;
	protected TcpPktHashFromProxy1 packetsProvider;
	protected Vector<String> p2Addresses;
	protected ConcurrentHashMap<String, String> packetsFromProxy2;
	protected static Logger logger;
	protected String trustStorePath;
	protected String storeKey;
	protected boolean blockingMode;
	

	@Override
	public Collection<Class<? extends IFloodlightService>> getModuleServices() {
		Collection<Class<? extends IFloodlightService>> l = new ArrayList<Class<? extends IFloodlightService>>();
		l.add(HashFromProxy2s.class);
		return l;
	}

	@Override
	public Map<Class<? extends IFloodlightService>, IFloodlightService> getServiceImpls() {
		Map<Class<? extends IFloodlightService>, IFloodlightService> m = new HashMap<Class<? extends IFloodlightService>, IFloodlightService>();
	    m.put(HashFromProxy2s.class, this);
	    return m;
	}

	@Override
	public Collection<Class<? extends IFloodlightService>> getModuleDependencies() {
		Collection<Class<? extends IFloodlightService>> l = new ArrayList<Class<? extends IFloodlightService>>();
		l.add(IFloodlightProviderService.class);
		l.add(TcpPktHashFromProxy1.class);
		return l;
	}

	@Override
	public void init(FloodlightModuleContext context) throws FloodlightModuleException {
		floodlightProvider = context.getServiceImpl(IFloodlightProviderService.class);
		packetsProvider = context.getServiceImpl(TcpPktHashFromProxy1.class);
		
		packetsFromProxy2 = new ConcurrentHashMap<>();
		logger = LoggerFactory.getLogger(Conn2Proxy2.class);
		Map<String, String> options = context.getConfigParams(this);
		
		this.blockingMode = packetsProvider.getWorkingMode();
		String proxy2List = options.get("proxy2List");
		this.trustStorePath = options.get("trustStorePath");
		this.storeKey = options.get("storeKey");

		p2Addresses = parseP2List(proxy2List);
		
	}
	
	private Vector<String> parseP2List(String list){
		Vector<String> vlist = new Vector<>();
		String[] destinations = list.split(",");
		for (String s : destinations) {
			String[] tmp = s.split(":");
			String IP = tmp[0];
			String port = tmp[1];
			vlist.add(IP);
			vlist.add(port);
		}
		return vlist;
	}
	
	@Override
	public void startUp(FloodlightModuleContext context) throws FloodlightModuleException {
		logger.info("{} Started!", Conn2Proxy2.class.getSimpleName());
		floodlightProvider.addOFMessageListener(OFType.PACKET_IN, this);
//		String IP = "192.168.31.50";
//		int portNum = 8889;
		//only one thread below for testing:
//		SSLClient sslClient = new SSLClient(IP, portNum, trustStorePath , storeKey, this);
//		sslClient.start();
		for (int i = 0; i < p2Addresses.size(); i += 2){
			String IP = p2Addresses.get(i);//for test only
			int portNum = Integer.parseInt(p2Addresses.get(i+1));
			// TODO FIXME
			SSLClient sslClient = new SSLClient(IP, portNum, trustStorePath , storeKey, this);
			sslClient.start();
			System.out.println("Connecting with Server Agent");
		}
	}

	@Override
	public String getName() {
		return Conn2Proxy2.class.getSimpleName();
	}

	@Override
	public boolean isCallbackOrderingPrereq(OFType type, String name) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isCallbackOrderingPostreq(OFType type, String name) {
		if (type.equals(OFType.PACKET_IN) && name.equals(BlockingPackets.class.getSimpleName())) {
			return true;
		}
		return false;
	}

	@Override
	public net.floodlightcontroller.core.IListener.Command receive(IOFSwitch sw, OFMessage msg,
			FloodlightContext cntx) {
		// send out packets
//		ConcurrentLinkedQueue<SwitchMessagePair> queuedPackets = packetsProvider.getQueuedPackets();
//		while(!queuedPackets.isEmpty()){
//			SwitchMessagePair pair = queuedPackets.poll();
//			if(pair != null){
//				logger.info("Send packet: ");
//				HashMatching.sendPktOutMsg(pair, SendMode.Full_Pkt);
//			}
//		}
//		ConcurrentHashMap<String, SwitchMessagePair> pkts = packetsProvider.getPackets();
//		if (!pkts.isEmpty()) {
//			for (String key : pkts.keySet()) {
//				SwitchMessagePair pair = pkts.get(key);
//				HashMatching.sendPktOutMsg(pair, SendMode.Full_Pkt);
//				pkts.remove(key);
//			}
//		}
//		pkts = packetsProvider.getPackets();
		// logger.info("packet cleaned? {}", pkts.isEmpty());
		return null;
	}

/*	public void addHash(String h){
		// Second parameter is for price
		// Currently all zero rating service.
		h = h.trim();
		this.packetsFromProxy2.put(h, "0");
		// TODO check hash in BlockingPackets module.
		// Every time get a new hash value from proxy2 we should matching it with the 
		// hash dict in packetsProvider
		boolean rst = HashMatching.matching(h, this.packetsProvider.getPackets(), this.packetsProvider.getPackets(), this.packetsProvider.getHashedTime());
		if (rst) {
			logger.debug("(Case 1 Packet Hash reach P1 first) Matching packet: {}", h);
			this.packetsFromProxy2.remove(h);
		}
		
	}*/

	@Override
	public ConcurrentHashMap<String, String> getHashFromProxy2() {
		
		return this.packetsFromProxy2;
	}
}
