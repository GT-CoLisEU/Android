package br.com.rnp.measurements.Network;

import android.bluetooth.BluetoothClass;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;
import android.telephony.TelephonyManager;
import android.util.Log;

import java.math.BigInteger;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;

import br.com.rnp.measurements.OWAMP.OWAMP;
import br.com.rnp.measurements.OWAMP.OWAMPArguments;
import br.com.rnp.measurements.OWAMP.OWAMPResult;
import br.com.rnp.qoslibrary.R;


/**
 * Check device's network connectivity and speed 
 * @author emil http://stackoverflow.com/users/220710/emil
 *
 * Modified by matias on 13/01/15.
 */
public class ConnectionInfo {

    private Context context;

    public ConnectionInfo(Context context){
        this.context = context;
    }



    public String getConnectinType(int type, int subType){
        if(type==ConnectivityManager.TYPE_WIFI){
            return "WIFI";
        }else if(type==ConnectivityManager.TYPE_MOBILE){
            switch(subType){
                case TelephonyManager.NETWORK_TYPE_1xRTT:
                    return "1xRTT"; // ~ 50-100 kbps
                case TelephonyManager.NETWORK_TYPE_CDMA:
                    return "CDMA"; // ~ 14-64 kbps
                case TelephonyManager.NETWORK_TYPE_EDGE:
                    return "EDGE"; // ~ 50-100 kbps
                case TelephonyManager.NETWORK_TYPE_EVDO_0:
                    return "EVDO_0"; // ~ 400-1000 kbps
                case TelephonyManager.NETWORK_TYPE_EVDO_A:
                    return "EVDO_A"; // ~ 600-1400 kbps
                case TelephonyManager.NETWORK_TYPE_GPRS:
                    return "GPRS"; // ~ 100 kbps
                case TelephonyManager.NETWORK_TYPE_HSDPA:
                    return "HSDPA"; // ~ 2-14 Mbps
                case TelephonyManager.NETWORK_TYPE_HSPA:
                    return "HSPA"; // ~ 700-1700 kbps
                case TelephonyManager.NETWORK_TYPE_HSUPA:
                    return "HSUPA"; // ~ 1-23 Mbps
                case TelephonyManager.NETWORK_TYPE_UMTS:
                    return "UMTS"; // ~ 400-7000 kbps
            /*
             * Above API level 7, make sure to set android:targetSdkVersion
             * to appropriate level to use these
             */
                case TelephonyManager.NETWORK_TYPE_EHRPD: // API level 11
                    return "EHRPD"; // ~ 1-2 Mbps
                case TelephonyManager.NETWORK_TYPE_EVDO_B: // API level 9
                    return "EVDO_B"; // ~ 5 Mbps
                case TelephonyManager.NETWORK_TYPE_HSPAP: // API level 13
                    return "HSPAP"; // ~ 10-20 Mbps
                case TelephonyManager.NETWORK_TYPE_IDEN: // API level 8
                    return "IDEN"; // ~25 kbps
                case TelephonyManager.NETWORK_TYPE_LTE: // API level 11
                    return "LTE"; // ~ 10+ Mbps
                // Unknown
                case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                default:
                    return "UNKNOWN";
            }
        }else{
            return "UNKNOWN";
        }
    }


    /**
     * Get the network info
     */
    private NetworkInfo getNetworkInfo(){
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(context.CONNECTIVITY_SERVICE);
        return cm.getActiveNetworkInfo();
    }


    /**
     * Check for connectivity (wifi and mobile)
     *
     * @return true if there is a connectivity, false otherwise
     */
    public boolean hasConnectivity() {
        ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(context.CONNECTIVITY_SERVICE);
        android.net.NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
        return activeNetworkInfo != null && activeNetworkInfo.isConnected();
    }


    /**
     * Check if there is any connectivity to a Wifi network
     */
    public boolean isConnectedWifi(){
        NetworkInfo info = getNetworkInfo();
        return (info != null && info.isConnected() && info.getType() == ConnectivityManager.TYPE_WIFI);
    }

    /**
     * Check if there is any connectivity to a mobile network
     */
    public boolean isConnectedMobile(){
        NetworkInfo info = getNetworkInfo();
        return (info != null && info.isConnected() && info.getType() == ConnectivityManager.TYPE_MOBILE);
    }


    public String getPrimaryDNS(){
        if(hasConnectivity()){
            if(isConnectedWifi()){
                WifiManager wifi = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
                DhcpInfo dhcp = wifi.getDhcpInfo();
                return formatIpAddress(dhcp.dns1);
            } else if (isConnectedMobile()){

            }
        }
        return null;
    }

    public String getSecondaryDNS(){
        if(hasConnectivity()){
            if(isConnectedWifi()){
                WifiManager wifi = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
                DhcpInfo dhcp = wifi.getDhcpInfo();
                return formatIpAddress(dhcp.dns2);
            } else if (isConnectedMobile()){

            }
        }
        return null;
    }

    public String getDefautGateway(){
        if(hasConnectivity()){
            if(isConnectedWifi()){
                WifiManager wifi = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
                DhcpInfo dhcp = wifi.getDhcpInfo();
                return formatIpAddress(dhcp.gateway);
            } else if (isConnectedMobile()){

            }
        }
        return null;
    }

    public String getIpAddress(){
        if(hasConnectivity()){
            if(isConnectedWifi()){
                WifiManager wifi = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
                DhcpInfo dhcp = wifi.getDhcpInfo();
                return formatIpAddress(dhcp.ipAddress);
            } else if (isConnectedMobile()){

            }
        }
        return null;
    }

    public String getSubnetMask(){
        if(hasConnectivity()){
            if(isConnectedWifi()){
                WifiManager wifi = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
                DhcpInfo dhcp = wifi.getDhcpInfo();
                return formatIpAddress(dhcp.netmask);
            } else if (isConnectedMobile()){

            }
        }
        return null;
    }


    private String formatIpAddress(int ip) {
        try {
        byte[] bytes = BigInteger.valueOf(ip).toByteArray();
        InetAddress inet = null;
        inet = InetAddress.getByAddress(bytes);
        String[] str =  inet.getHostAddress().split("\\.");
        return str[3] + "." + str[2] + "." + str[1] + "." + str[0];
        } catch (UnknownHostException e) {
            Log.e("ERRO", "Cating IP bytes erro");
            return null;
        }
    }



}
