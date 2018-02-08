package anonymous.seclab;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.security.KeyStore;
import java.util.concurrent.ConcurrentLinkedQueue;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLServerSocket;
import javax.net.ssl.TrustManagerFactory;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SSLServer extends Thread {
	public boolean isConnected = false;
	public Socket connectedSocket = null;
	private SSLServerSocket sslServerSocket;
	private Logger logger;
	
	ConcurrentLinkedQueue<String> packetHash;
	
	private int port;
	
	public SSLServer(int port, ConcurrentLinkedQueue<String> packetsHash) {
		this.port = port;
		logger = LoggerFactory.getLogger(SSLServer.class);
		this.packetHash = packetsHash;
	}

	@Override
	public void run() {
		//super.run();
		try {
			init();
			process();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}



	public void init() throws Exception {
		
		String keystorePath = "./src/main/java/anonymous/seclab/sslserverkeys";
		String trustKeystorePath = "./src/main/java/anonymous/seclab/sslservertrust";
		String keystorePassword = "111111";
		SSLContext context = SSLContext.getInstance("SSL");// TLSv1

		// keys 证书库
		KeyStore keystore = KeyStore.getInstance("jks");// jks?
		FileInputStream keystoreFis = new FileInputStream(keystorePath);
		keystore.load(keystoreFis, keystorePassword.toCharArray());
		// 信任证书库
		KeyStore trustKeystore = KeyStore.getInstance("jks");
		FileInputStream trustKeystoreFis = new FileInputStream(
				trustKeystorePath);
		trustKeystore.load(trustKeystoreFis, keystorePassword.toCharArray());

		// 密钥库
		KeyManagerFactory kmf = KeyManagerFactory.getInstance("sunx509");
		kmf.init(keystore, keystorePassword.toCharArray());

		// 信任库
		TrustManagerFactory tmf = TrustManagerFactory.getInstance("sunx509");
		tmf.init(trustKeystore);

		// 初始化SSL上下文
		context.init(kmf.getKeyManagers(), tmf.getTrustManagers(), null);
		// 初始化SSLSocket
		sslServerSocket = (SSLServerSocket) context.getServerSocketFactory()
				.createServerSocket(port);
		// 设置这个SSLServerSocket需要授权的客户端访问
		sslServerSocket.setNeedClientAuth(false);
	}

	public void process() {
		while (true) {
			//logger.info("ssl server daemon");
			try {
				if (isConnected) {
					// logger.info("use connected: ");
					if(!packetHash.isEmpty()){
						String pHash = packetHash.poll();
						while(pHash != null){
							OutputStream out = connectedSocket.getOutputStream();
							out.write(pHash.getBytes());
							out.flush();
							logger.info("Send packet: {} , at time {}", pHash, System.currentTimeMillis());
							
							pHash = this.packetHash.poll();
						}
					}
				} else {
					logger.info("Waiting connection");
					Socket socket = sslServerSocket.accept();
					connectedSocket = socket;
					isConnected = true;
					logger.info("Connected with one");
				}
				//Thread.sleep(2000);
			} 
//			catch (InterruptedException e) {
//				e.printStackTrace();
//			} 
			catch (IOException e) {
				e.printStackTrace();
				isConnected = false;
				connectedSocket = null;
			}

		}

	}
	
	

}
