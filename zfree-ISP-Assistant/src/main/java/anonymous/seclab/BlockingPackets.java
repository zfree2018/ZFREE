package anonymous.seclab;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

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
import net.floodlightcontroller.core.types.SwitchMessagePair;
import net.floodlightcontroller.restserver.IRestApiService;


public class BlockingPackets implements IFloodlightModule, IOFMessageListener, TcpPktHashFromProxy1 {

	protected IFloodlightProviderService floodlightProvider;
	protected IRestApiService restApiService;
	protected ConcurrentHashMap<String, SwitchMessagePair> packetsAndHash;
	protected ConcurrentLinkedQueue<SwitchMessagePair> packetsQueue;
	protected ConcurrentHashMap<String, Long> hashedTime;
	protected HashFromProxy2s hashFromP2;
	protected boolean blockingMode;
	

	protected static Logger logger;

	@Override
	public String getName() {
		return BlockingPackets.class.getSimpleName();
	}

	@Override
	public boolean isCallbackOrderingPrereq(OFType type, String name) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isCallbackOrderingPostreq(OFType type, String name) {
		if (type.equals(OFType.PACKET_IN) && name.equals("forwarding")) {
			return true;
		}
		return false;
	}

	@Override
	public net.floodlightcontroller.core.IListener.Command receive(IOFSwitch sw, OFMessage msg,
			FloodlightContext cntx) {
		this.packetsQueue.offer(new SwitchMessagePair(sw, msg));
		return Command.STOP;
		
	}

	@Override
	public Collection<Class<? extends IFloodlightService>> getModuleServices() {
		Collection<Class<? extends IFloodlightService>> l = new ArrayList<Class<? extends IFloodlightService>>();
		l.add(TcpPktHashFromProxy1.class);
		return l;
	}

	@Override
	public Map<Class<? extends IFloodlightService>, IFloodlightService> getServiceImpls() {
		Map<Class<? extends IFloodlightService>, IFloodlightService> m = new HashMap<Class<? extends IFloodlightService>, IFloodlightService>();
		m.put(TcpPktHashFromProxy1.class, this);
		return m;
	}

	@Override
	public Collection<Class<? extends IFloodlightService>> getModuleDependencies() {
		Collection<Class<? extends IFloodlightService>> l = new ArrayList<Class<? extends IFloodlightService>>();
		l.add(IFloodlightProviderService.class);
		l.add(IRestApiService.class);
		return l;
	}

	@Override
	public void init(FloodlightModuleContext context) throws FloodlightModuleException {
		floodlightProvider = context.getServiceImpl(IFloodlightProviderService.class);
		restApiService = context.getServiceImpl(IRestApiService.class);
		hashFromP2 = context.getServiceImpl(HashFromProxy2s.class);

		this.packetsAndHash = new ConcurrentHashMap<>();
		this.hashedTime = new ConcurrentHashMap<>();
		this.packetsQueue = new ConcurrentLinkedQueue<>();
		
		logger = LoggerFactory.getLogger(BlockingPackets.class);
		
		// get parameter
		Map<String, String> options = context.getConfigParams(this);
		String blocking = options.get("blockingMode");
		if(blocking.equals("NO")){
		    this.blockingMode = false;
		}
		else{
		    this.blockingMode = true;
		}
		
	}

	@Override
	public void startUp(FloodlightModuleContext context) throws FloodlightModuleException {
		logger.info("{} Started!", getName());
		floodlightProvider.addOFMessageListener(OFType.PACKET_IN, this);
		restApiService.addRestletRoutable(new ZfreeRestRoutable());
		
		// start hash worker
		// current worker number 6
		for(int i=0; i < 2; ++i){
			HashThread hashThread = new HashThread(this);
			hashThread.start();
		}
	}
	
	public ConcurrentHashMap<String, SwitchMessagePair> getPackets() {
		return this.packetsAndHash;
	}
	
	public ConcurrentHashMap<String, Long> getHashedTime(){
		return this.hashedTime;
	}
	
	public ConcurrentLinkedQueue<SwitchMessagePair> getQueuedPackets(){
		return this.packetsQueue;
	}
	
	public HashFromProxy2s getHashFromP2(){
		return this.hashFromP2;
	}
	
	public static byte[] getTimestamp(byte[] options){
		byte[] rst = null;
		if (options !=null){
			for(int i=0; i < options.length; ++i){
				if((i+1) < options.length){
					if(options[i] == 8 && options[i+1] == 10){
						rst = Arrays.copyOfRange(options, i+2, i+10);
					}
				}
			}
		}
		return rst;
	}

    @Override
    public boolean getWorkingMode() {
        return this.blockingMode;
    }

}
