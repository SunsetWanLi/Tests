package com.z11.mobile.framework.location;

import org.apache.http.HttpHost;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.conn.params.ConnRouteParams;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpConnectionParams;
import org.json.JSONObject;

import android.app.Activity;
import android.database.Cursor;
import android.net.Uri;

import com.z11.mobile.framework.location.IAddressTask;

public class AddressTask extends IAddressTask {

    public static final int DO_MOBILE = 0;
    public static final int DO_WIFI = 1;
    public static final int DO_GPS = 2;
    private int postType = -1;
    private static final String HTTP_POST = "http://www.google.com/loc/json";
    private static final int CONNECTION_TIMEOUT = 10 * 1000;
    private static final int SO_TIMEOUT = 20 * 1000;
    
    public AddressTask(Activity context, int postType) {
        super(context);
        this.postType = postType;
    }
    
    @Override
    public HttpResponse execute(JSONObject params) throws Exception {
    	if (params == null)
    		return null;
        HttpClient httpClient = new DefaultHttpClient();

        HttpConnectionParams.setConnectionTimeout(httpClient.getParams(), CONNECTION_TIMEOUT);
        HttpConnectionParams.setSoTimeout(httpClient.getParams(), SO_TIMEOUT);
        
        //访问json
        HttpPost post = new HttpPost(HTTP_POST);
        // 设置代理
        if (postType == DO_MOBILE) {
        	Uri uri = Uri.parse("content://telephony/carriers/preferapn"); // 获取当前正在使用的APN接入点
            Cursor mCursor = context.getContentResolver().query(uri, null,
                    null, null, null);
            if (mCursor != null) {
                if(mCursor.moveToFirst()) {
                    String proxyStr = mCursor.getString(mCursor
                            .getColumnIndex("proxy"));
                    if (proxyStr != null && proxyStr.trim().length() > 0) {
                        HttpHost proxy = new HttpHost(proxyStr, 80);
                        httpClient.getParams().setParameter(
                                ConnRouteParams.DEFAULT_PROXY, proxy);
                    }
                }
            }
//            String proxyHost = Proxy.getDefaultHost();
//            if(proxyHost != null) {
//                HttpHost proxy = new HttpHost(proxyHost, 80);
//                httpClient.getParams().setParameter(
//                        ConnRouteParams.DEFAULT_PROXY, proxy);
//            }
        }
        //请求一系列的参数
        StringEntity se = new StringEntity(params.toString());
        post.setEntity(se);
        HttpResponse response = httpClient.execute(post);
        return response;
    }
}