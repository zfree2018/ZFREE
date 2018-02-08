package anonymous.seclab;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

import net.floodlightcontroller.core.module.IFloodlightService;
import net.floodlightcontroller.core.types.SwitchMessagePair;

public interface TcpPktHashFromProxy1 extends IFloodlightService{
	public ConcurrentHashMap<String, SwitchMessagePair> getPackets();
	public ConcurrentHashMap<String, Long> getHashedTime();
	public ConcurrentLinkedQueue<SwitchMessagePair> getQueuedPackets();
	public boolean getWorkingMode();
}
