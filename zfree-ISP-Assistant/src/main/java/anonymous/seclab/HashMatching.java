package anonymous.seclab;

import java.util.Collections;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import org.projectfloodlight.openflow.protocol.OFPacketIn;
import org.projectfloodlight.openflow.protocol.OFPacketOut;
import org.projectfloodlight.openflow.protocol.action.OFAction;
import org.projectfloodlight.openflow.types.IPv4Address;
import org.projectfloodlight.openflow.types.OFBufferId;
import org.projectfloodlight.openflow.types.OFPort;

import net.floodlightcontroller.core.IOFSwitch;
import net.floodlightcontroller.core.types.SwitchMessagePair;
import net.floodlightcontroller.packet.Ethernet;
import net.floodlightcontroller.packet.IPv4;
import net.floodlightcontroller.packet.TCP;
import net.floodlightcontroller.util.OFMessageUtils;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import anonymous.seclab.Conn2Proxy2.SendMode;

public class HashMatching {
	private static Logger logger = LoggerFactory.getLogger(HashMatching.class);
	public static Integer tryMatch = 0;
	public static Integer matched = 0;
	
	/*
	 * Invoke after hash process done at ISP side
	 * */
	public static boolean matchingWhenPacketIn(String pHash, 
			SwitchMessagePair pair,
			ConcurrentHashMap<String, SwitchMessagePair> packetsContainer,
			ConcurrentHashMap<String, String> hashFromProxy2,
			ConcurrentHashMap<String, Long> hashedTime,
			boolean blocking){
		
		tryMatch ++;
		logger.info("try matching WhenPacketIn {} , at Time: {}", pHash, System.currentTimeMillis());
		logger.info("-------------------------------- matched" + matched);
		pHash = pHash.trim();
		if (hashFromProxy2.containsKey(pHash)){
			// Hash value already got from Server Agent.
			matched ++;
			
			
			// Send Out directly
			HashMatching.sendPktOutMsg(pair, SendMode.Full_Pkt);
			// in blocking mode we will block packet first
            // later need to send it out
            // otherwise client cannot receive 
            if (blocking){
                HashMatching.sendPktOutMsg(pair, SendMode.Full_Pkt);
            }
            logger.info("Send Packet: " + pHash + "at time: " + System.currentTimeMillis());
			
			logOutput(pHash, packetsContainer);
//			if((packetsContainer.size()) > 500){
//				remainPacketsAnalysis(packetsContainer);
//			}
			
			return true;
		}else{
			// current packet hash cannot match any received hash from Server Agent
			// Save packet and hash
			packetsContainer.put(pHash, pair);
			hashedTime.put(pHash, System.currentTimeMillis());
			logger.info("Put Hash {} into packets Container", pHash);
			
			return false;
		}

	}
	
	
	/*
	 * Invoke when receiving hash from Server Agent.
	 * */
	public static boolean matchingWhenReceiveHash( String pHash,
			ConcurrentHashMap<String, SwitchMessagePair> packetsContainer,
			ConcurrentHashMap<String, String> hashFromProxy2,
			ConcurrentHashMap<String, Long> hashedTime,
			boolean blocking){
		tryMatch++;
		logger.info("try matching WhenReceiveHash {} , at Time: {}", pHash, System.currentTimeMillis());
		pHash = pHash.trim();
		if (packetsContainer.containsKey(pHash)){
			matched++;
			
			SwitchMessagePair pair = packetsContainer.get(pHash);
			// in blocking mode we will block packet first
			// later need to send it out
			// otherwise client cannot receive 
			if (blocking){
			    HashMatching.sendPktOutMsg(pair, SendMode.Full_Pkt);
			}
			
			packetsContainer.remove(pHash);
			
			logOutput(pHash, packetsContainer);
			// extra log for packet delay
			Long time = hashedTime.get(pHash);
			logger.info("Time delay: {} ms; of packet {}", System.currentTimeMillis()-time, pHash);
			logger.info("Send Packet: " + pHash + "at time: " + System.currentTimeMillis());
			return true;
		}else{
			// no match
			// save hash received
			hashFromProxy2.put(pHash, "0"); // "0" stand for price or other policy

			return false;
		}
		
	}
	
	public static void logOutput(String pHash, 
			ConcurrentHashMap<String, SwitchMessagePair> packetsContainer ){
		logger.info("Tried times: {}; Matched times: {}", tryMatch, matched);
		logger.info("Send Out packet: {}, at time: {}", pHash, System.currentTimeMillis());
		logger.info("Num of Packet not sent out: {} ", packetsContainer.size());
	}
	
	public static void remainPacketsAnalysis(ConcurrentHashMap<String, SwitchMessagePair> packetsContainer){
		Iterator<Entry<String, SwitchMessagePair>> it = packetsContainer.entrySet().iterator();
		while(it.hasNext()){
			Map.Entry pair = (Map.Entry)it.next();
			SwitchMessagePair swp = (SwitchMessagePair) pair.getValue();
			parsePacket(swp);
		}
	}
	
	public static void parsePacket(SwitchMessagePair messagePair){
		OFPacketIn message = (OFPacketIn) messagePair.getMessage();
		byte[] data = message.getData();
		Ethernet eth = new Ethernet();
		eth.deserialize(data, 0, data.length);
		
		IPv4 iPv4 = (IPv4) eth.getPayload();
		TCP tcp = (TCP) iPv4.getPayload();
		IPv4Address dstIp = null;
		IPv4Address srcIp = null;
		dstIp = iPv4.getDestinationAddress();
		srcIp = iPv4.getSourceAddress();
		logger.info("Remain Packet src IP:{} tcp src port {}", srcIp, tcp.getSourcePort().toString());
		logger.info("Remain packet dst IP:{} tcp dst port {}", dstIp, tcp.getDestinationPort().toString());
		
	}
	
	public static void sendPktOutMsg(SwitchMessagePair pair, SendMode mode) {
		// TODO complete the send full packet and buffer id
		// TODO what will happen if I setup both packet data and buffer ID?
		IOFSwitch sw = pair.getSwitch();
		OFPacketIn message = (OFPacketIn) pair.getMessage();
		switch (mode) {
		case Full_Pkt:
			byte[] data = message.getData();
			OFPacketOut.Builder pob = sw.getOFFactory().buildPacketOut();
			OFPort inPort = OFMessageUtils.getInPort(message);
			pob.setActions(Collections
					.singletonList((OFAction) sw.getOFFactory().actions().output(OFPort.NORMAL, 0xffFFffFF)));
			
			pob.setData(data);
			pob.setBufferId(OFBufferId.NO_BUFFER);
			OFMessageUtils.setInPort(pob, inPort);
			
			OFPacketOut po = pob.build();
			
			try {
				sw.write(po);
			} catch (Exception e) {
				System.err.println(e);
			}
			
			break;
		case Buffer_ID:
			OFBufferId buffer_id = message.getBufferId();
			OFPacketOut buffer_po = sw.getOFFactory().buildPacketOut()
					.setBufferId(buffer_id)
					//.setData(message.getData()) // Set both data and buffer_id
					.setActions(Collections.singletonList((OFAction) sw.getOFFactory().actions().output(OFPort.NORMAL, 0xffFFffFF)))
					.setInPort(OFPort.CONTROLLER).build();
			sw.write(buffer_po);
			break;
		default:
			break;
		}
	}

}
