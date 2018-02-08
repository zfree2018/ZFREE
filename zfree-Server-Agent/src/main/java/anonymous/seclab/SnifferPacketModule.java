package anonymous.seclab;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Map;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.projectfloodlight.openflow.protocol.OFMessage;
import org.projectfloodlight.openflow.protocol.OFPacketIn;
import org.projectfloodlight.openflow.protocol.OFPortDesc;
import org.projectfloodlight.openflow.protocol.OFType;
import org.projectfloodlight.openflow.types.OFPort;
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
import net.floodlightcontroller.util.OFMessageUtils;

public class SnifferPacketModule implements IOFMessageListener,
		IFloodlightModule {
	protected IFloodlightProviderService floodlightProvider;
	protected ConcurrentLinkedQueue<String> packetHashs;
	protected ConcurrentLinkedQueue<OFMessage> packetInMsgs;
	protected SSLServer sslServer;
	protected static Logger logger;
	protected long lastTimePacketFromServer = 0;
	protected long lastEthPacket = 0;
	protected long lastTimeIP = 0;
	protected long lastTcpPacket = 0;

	@Override
	public String getName() {

		return SnifferPacketModule.class.getSimpleName();
	}

	@Override
	public boolean isCallbackOrderingPrereq(OFType type, String name) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isCallbackOrderingPostreq(OFType type, String name) {
		logger.info("module name{}", name);
		if (type.equals(OFType.PACKET_IN) && name.equals("forwarding")) {
			return true;
		}
		return false;
	}

	@Override
	public Collection<Class<? extends IFloodlightService>> getModuleServices() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Map<Class<? extends IFloodlightService>, IFloodlightService> getServiceImpls() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Collection<Class<? extends IFloodlightService>> getModuleDependencies() {
		Collection<Class<? extends IFloodlightService>> l = new ArrayList<Class<? extends IFloodlightService>>();
		l.add(IFloodlightProviderService.class);
		return l;
	}

	@Override
	public void init(FloodlightModuleContext context)
			throws FloodlightModuleException {
		floodlightProvider = context
				.getServiceImpl(IFloodlightProviderService.class);
		packetHashs = new ConcurrentLinkedQueue<String>();
		packetInMsgs = new ConcurrentLinkedQueue<>();
		
		logger = LoggerFactory.getLogger(SnifferPacketModule.class);
		sslServer = new SSLServer(8889, packetHashs);
		sslServer.start();
	}

	@Override
	public void startUp(FloodlightModuleContext context)
			throws FloodlightModuleException {
		floodlightProvider.addOFMessageListener(OFType.PACKET_IN, this);
		logger.info("Sniffer started");
		
		// start Hash Threads
		for(int i=0; i < 2; ++i){
			HashThread hashThread = new HashThread(this);
			hashThread.start();
		}
	}

	@Override
	public net.floodlightcontroller.core.IListener.Command receive(
			IOFSwitch sw, OFMessage msg, FloodlightContext cntx) {		
		
		packetInMsgs.offer(msg);
		return Command.STOP;
	}


}
