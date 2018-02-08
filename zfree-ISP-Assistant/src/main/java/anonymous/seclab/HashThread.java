package anonymous.seclab;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.projectfloodlight.openflow.protocol.OFPacketIn;
import org.projectfloodlight.openflow.types.EthType;
import org.projectfloodlight.openflow.types.IPv4Address;
import org.projectfloodlight.openflow.types.IpProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import anonymous.seclab.Conn2Proxy2.SendMode;
import net.floodlightcontroller.core.types.SwitchMessagePair;
import net.floodlightcontroller.packet.Ethernet;
import net.floodlightcontroller.packet.IPv4;
import net.floodlightcontroller.packet.TCP;

public class HashThread extends Thread{
	
	public ConcurrentHashMap<String, SwitchMessagePair> packetsAndHash;
	public ConcurrentLinkedQueue<SwitchMessagePair> queuedPackets;
	public HashFromProxy2s hashFromProxy2s;
	public ConcurrentHashMap<String, Long> hashedTime;
	public boolean blockingMode;
	
	private Logger logger;
	
	public HashThread(BlockingPackets rootThread) {
		this.packetsAndHash = rootThread.getPackets();
		this.queuedPackets = rootThread.getQueuedPackets();
		this.hashFromProxy2s = rootThread.getHashFromP2();
		this.hashedTime = rootThread.getHashedTime();
		this.blockingMode = rootThread.blockingMode;
		
		logger =  LoggerFactory.getLogger(HashThread.class);
	}

	@Override
	public void run() {
		
		while (true){
			SwitchMessagePair pair = queuedPackets.poll();
			if (pair != null) {
				OFPacketIn message = (OFPacketIn) pair.getMessage();
				byte[] data = message.getData();
				Ethernet eth = new Ethernet();
				eth.deserialize(data, 0, data.length);
				
				// filter white list packet and non-tcp packets
				SendMode mode = SendMode.Full_Pkt;
				if (eth.getEtherType() != EthType.IPv4){
					HashMatching.sendPktOutMsg(pair, mode);
					continue;
				}
				
				IPv4 iPv4 = (IPv4) eth.getPayload();
				IPv4Address dstIp = iPv4.getDestinationAddress();
				IPv4Address srcIp = iPv4.getSourceAddress();
				if (iPv4.getProtocol() != IpProtocol.TCP){
					HashMatching.sendPktOutMsg(pair, mode);
					continue;
				}
				
				if (srcIp.toString().equals("10.0.1.20")||srcIp.toString().equals("10.0.1.30")){
					// let the proxy2's packets and send-out packets go
					HashMatching.sendPktOutMsg(pair, mode);
					continue;
				}
				
				// end filter-----------------------------------

				
				// logging
				//logger.info("Src IP: {}", srcIp.toString());
				//logger.info("Dst IP: {}", dstIp.toString());
				
				TCP tcp = (TCP) iPv4.getPayload();
				String hash = hashTcpPacket(tcp);
				System.out.println("Get Hash: " + hash + "at time: " + System.currentTimeMillis());
				if (hash!=null) {
				    String logMsg = "Packet hash: " + hash + " src: " + srcIp.toString() + " dst: " + dstIp.toString();
				    logMsg += "\n";
				    logMsg += logBytes("TCP Bytes: ", tcp.serialize());
				    
					logger.info(logMsg);
					
					boolean rst = HashMatching.matchingWhenPacketIn(hash, pair, packetsAndHash, 
							hashFromProxy2s.getHashFromProxy2(), hashedTime, this.blockingMode);
					if (rst) {
						logger.info("(Case2 Hash received from P2 firstly) Matching Hash: {}", hash);
					}
				}
				
			}else{
				try {
					Thread.sleep(3);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
	private String logBytes(String prefix, byte[] data){
	    String logMsg = prefix + "Hex: ";
	    for (int i = 0; i < data.length; i++){
	        logMsg += Integer.toHexString(data[i] & 0xFF);
	        logMsg += " ";
	    }
	    return logMsg;
	}
	
	private String hashTcpPacket(TCP tcp){
		
		String hashStr = null;
		try {
		    byte[] dataForHash = tcp.serialize();
		    // use only data part.
//		    byte[] dataForHash = tcp.getPayload().serialize();
		    
		    // include tcp headers without checksum
		    dataForHash[16] = 0;
		    dataForHash[17] = 0;
		    // ignore option field
            if (dataForHash.length > 32){
                for (int i = 23; i < 32; i++){
                    dataForHash[i] = 0;
                }    
            }
		    
		    String logMsg = logBytes("Zero Checksum ", dataForHash);
            logger.info(logMsg);
		    
			MessageDigest digest = MessageDigest.getInstance("SHA-256");
			byte[] tcpHash = digest.digest(dataForHash);
			hashStr = String.format("%064x", new java.math.BigInteger(1, tcpHash));
			
		} catch (NoSuchAlgorithmException e) {
			
			e.printStackTrace();
		}
		return hashStr.trim();
	}

}
