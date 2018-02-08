package anonymous.seclab;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.GeneralSecurityException;
import java.security.KeyStore;

import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.TrustManagerFactory;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SSLClient extends Thread {
	private String hostStr;
	private int port;
	private String trustStorePath;
	private String storeKey;
	private SSLSocket sslSocket;
	private Conn2Proxy2 root;
	private Logger logger;
	
	public String getSSLClientName(){
		return "SSLClient:: IP: " + hostStr + " Port: " + port;
	}
	
	public SSLClient(String host, int port, String storePath, String storeKey, Conn2Proxy2 rootThread) {
		this.hostStr = host;//"localhost"
		this.port = port;//"8889"
		this.trustStorePath = storePath;
		this.storeKey = storeKey;
		this.root = rootThread;
	}

	@Override
	public void run() {
		try {
			this.init();
			this.process();
		} catch (GeneralSecurityException | IOException e) {
			e.printStackTrace();
		}
	}
	
	private void init() throws GeneralSecurityException, IOException{
		SSLContext context = SSLContext.getInstance("SSL");
		
		KeyStore trustKeystore = KeyStore.getInstance("jks");
		FileInputStream trustKeystoreFis = new FileInputStream(this.trustStorePath);
		trustKeystore.load(trustKeystoreFis, this.storeKey.toCharArray());
		
		TrustManagerFactory tmf = TrustManagerFactory.getInstance("sunx509");
		tmf.init(trustKeystore);
		
		
		context.init(null, tmf.getTrustManagers(), null);
		sslSocket = (SSLSocket)context.getSocketFactory().createSocket(hostStr, port);
		
		logger = LoggerFactory.getLogger(SSLClient.class);
		logger.info("Connected to server!");
		System.out.println("Connected with" + this.hostStr);
	}
	
	private void process() throws IOException{
		while(true){
			InputStream in = sslSocket.getInputStream();
			byte[] buffer1 = new byte[2048];
			in.read(buffer1);
			String s= new String(buffer1);
			logger.info("Received hash: {}, at Time: {} ", s, System.currentTimeMillis());

			s = s.trim();
			System.out.println("receive from p2: " + s + " at Time: " + System.currentTimeMillis());
			boolean rst = HashMatching.matchingWhenReceiveHash(s, this.root.packetsProvider.getPackets(),
					this.root.packetsFromProxy2, this.root.packetsProvider.getHashedTime(), this.root.blockingMode);
			if (rst){
				logger.info("Case 1 received packet at proxy1 first.");
			}
		}
	}
}
