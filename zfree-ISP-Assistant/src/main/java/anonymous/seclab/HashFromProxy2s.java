package anonymous.seclab;

import java.util.concurrent.ConcurrentHashMap;

import net.floodlightcontroller.core.module.IFloodlightService;

public interface HashFromProxy2s extends IFloodlightService {
	public ConcurrentHashMap<String, String> getHashFromProxy2();
}
