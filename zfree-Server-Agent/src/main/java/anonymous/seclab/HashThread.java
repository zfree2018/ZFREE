package anonymous.seclab;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.projectfloodlight.openflow.protocol.OFMessage;
import org.projectfloodlight.openflow.protocol.OFPacketIn;
import org.projectfloodlight.openflow.types.EthType;
import org.projectfloodlight.openflow.types.IPv4Address;
import org.projectfloodlight.openflow.types.IpProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.floodlightcontroller.packet.Ethernet;
import net.floodlightcontroller.packet.IPv4;
import net.floodlightcontroller.packet.TCP;

public class HashThread extends Thread{
	
	public ConcurrentLinkedQueue<String> packetHashs;
	public ConcurrentLinkedQueue<OFMessage> packetInMsgs;
	private Logger logger;
	
	public HashThread(SnifferPacketModule rootThread) {
		this.packetHashs = rootThread.packetHashs;
		this.packetInMsgs = rootThread.packetInMsgs;
		
		logger =  LoggerFactory.getLogger(HashThread.class);
	}

	@Override
	public void run() {
		while (true){
			try {
				OFMessage ofMessage = packetInMsgs.poll();
				if (ofMessage != null){
					OFPacketIn inMsg = (OFPacketIn) ofMessage;
					byte[] data = inMsg.getData();
					Ethernet eth = new Ethernet();
					eth.deserialize(data, 0, data.length);
					
					if (eth.getEtherType() != EthType.IPv4){
						continue;
					}
					
					IPv4 iPv4 = (IPv4) eth.getPayload();
					IPv4Address dstIp = iPv4.getDestinationAddress();
					IPv4Address srcIp = iPv4.getSourceAddress();
					if (iPv4.getProtocol() != IpProtocol.TCP){
						continue;
					}
					TCP tcp = (TCP) iPv4.getPayload();
					String hash = hashTcpPacket(tcp);
					if (hash != null){
						packetHashs.offer(hash);
					}
					// logging
					String logMsg = "Src IP: " + srcIp.toString() + " ; Dst IP: " + dstIp.toString();
					logMsg += "\n";
					logMsg += "Packet Hash: " + hash + " at time: " + System.currentTimeMillis();
					// tcp bytes;
					logMsg += "\n";
					logMsg += logBytes("Tcp Bytes: ", tcp.serialize());
					
					logger.info(logMsg);
					
					
				}else{
					Thread.sleep(10);
				}
			} catch (Exception e) {
				e.printStackTrace();
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
		    // only tcp payload;
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
