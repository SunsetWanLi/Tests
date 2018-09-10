package com.z11.mobile.framework;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.List;
 
import org.ksoap2.transport.HttpTransportSE;
import org.ksoap2.transport.ServiceConnection;


public class MGAndroidHttpTransport extends HttpTransportSE 
{
	ServiceConnection connection;
	private int timeout = 10000; //默认超时时间为10s
	public MGAndroidHttpTransport(String url) {
		super(url);  
	}  
	public MGAndroidHttpTransport(String url, int timeout) {
		super(url); 
		this.timeout = timeout;
	} 

	protected ServiceConnection getServiceConnection(String url) throws IOException 
	{
		ServiceConnectionSE serviceConnection = new ServiceConnectionSE(url);
		serviceConnection.setConnectionTimeOut(timeout);
		return new ServiceConnectionSE(url);
	}
}

class ServiceConnectionSE implements ServiceConnection {
	private HttpURLConnection connection;
	
	public ServiceConnectionSE(String url) throws IOException {
		this.connection = ((HttpURLConnection)new URL(url).openConnection());
		this.connection.setUseCaches(false);
		this.connection.setDoOutput(true);
		this.connection.setDoInput(true);
	}
	public void connect() throws IOException {
		this.connection.connect();
	}
	public void disconnect() {
		this.connection.disconnect();
	}
	public void setRequestProperty(String string, String soapAction) {
		this.connection.setRequestProperty(string, soapAction);
	}
	public void setRequestMethod(String requestMethod) throws IOException {
		this.connection.setRequestMethod(requestMethod);
	}
	public OutputStream openOutputStream() throws IOException {
		return this.connection.getOutputStream();
	}
	public InputStream openInputStream() throws IOException {
		return this.connection.getInputStream();
	}
	public InputStream getErrorStream() {
		return this.connection.getErrorStream();
	}
	//设置连接服务器的超时时间,毫秒级,此为自己添加的方法
	public void setConnectionTimeOut(int timeout){
		this.connection.setConnectTimeout(timeout);
	}
	@Override
	public String getHost() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public String getPath() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public int getPort() {
		// TODO Auto-generated method stub
		return 0;
	}
	@Override
	public List getResponseProperties() throws IOException {
		// TODO Auto-generated method stub
		return null;
	}

}
