package br.com.rnp.measurements.Iperf;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

/**
 * Created by matias on 13/04/15.
 */
public class IperfResult {
    private List<IperfRequest> listRequest = null;
    private long dataTrasnfer;
    private String dataTrasnferFormat = null;;
    private long bandwidthAverage;
    private String bandwidthAverageFormat = null;;
    private String serverIp = null;;
    private int serverPort;
    private String localIp = null;;
    private int localPort;
    private String protocol = null;;
    private String direction = null;;
    private String TCPwindowSize = null;;
    private String UDPbufferSize = null;;
    private long UDPSentDatagrams;
    private long id;

    public long getUDPSendingDatagrams() {
        return UDPSendingDatagrams;
    }

    public void setUDPSendingDatagrams(long UDPSendingDatagrams) {
        this.UDPSendingDatagrams = UDPSendingDatagrams;
    }

    public String getUDPSendingDatagramsFormat() {
        return UDPSendingDatagramsFormat;
    }

    public void setUDPSendingDatagramsFormat(String UDPSendingDatagramsFormat) {
        this.UDPSendingDatagramsFormat = UDPSendingDatagramsFormat;
    }

    private long UDPSendingDatagrams;
    private String UDPSendingDatagramsFormat = null;;

    public IperfResult(){}

    public IperfResult(StringBuilder iperfResult, int testType){
        if(iperfResult != null){
            Log.d("IPERF_CREATE", "Iperf Result: " + iperfResult.toString());
            Scanner scan = new Scanner(iperfResult.toString());
            while (scan.hasNextLine()) {
                Log.d("IPERF_CREATE", "While in");
                String linha = scan.nextLine();
                Log.d("IPERF_CREATE", "Linha: " + linha);
                if(linha.contains("Client connecting to")){
                    serverIp = linha.replace("Client connecting to ","").split(",")[0];
                    serverPort = Integer.valueOf(linha.split("port ")[1]).intValue();
                } else if (linha.contains("window size:")){
                    String[] auxProtWin = linha.split(" window size: ");
                    protocol = auxProtWin[0];
                    TCPwindowSize = auxProtWin[1];
                } else if (linha.contains("buffer size:")) {
                    String[] auxProtWin = linha.split(" buffer size:  ");
                    protocol = auxProtWin[0];
                    UDPbufferSize = auxProtWin[1];
                } else if(linha.contains("local ")){
                    String[] auxLocal = linha.split("local ")[1].split(" port ");
                    localIp = auxLocal[0];
                    localPort = Integer.valueOf(auxLocal[1].split(" connected with ")[0]).intValue();
                }
                else if(linha.contains("Sent") && linha.contains("datagrams")){
                    String auxdatagram = linha.split("Sent ")[1].split(" datagrams")[0];
                    UDPSentDatagrams = Long.valueOf(auxdatagram).longValue();
                } else if(linha.contains("Sending") && linha.contains("datagrams")){
                    // Sending 1470 byte datagrams
                    String[] auxdatagram = linha.split("Sending ")[1].split(" ");
                    UDPSendingDatagrams = Long.valueOf(auxdatagram[0]).longValue();
                    UDPSendingDatagramsFormat = auxdatagram[1];
                } else if(linha.contains("sec") && linha.contains("Kbits") && !linha.contains("NaN")){
                    if(listRequest == null){
                        listRequest = new ArrayList<IperfRequest>();
                    }
                    listRequest.add(new IperfRequest(linha));
                }
            }
            if(listRequest != null && !listRequest.isEmpty()) {
                while(listRequest.get(listRequest.size()-1).getInterval().startsWith("0.0")){
                    IperfRequest finalRequest = listRequest.get(listRequest.size()-1);
                    dataTrasnfer = finalRequest.getTransfer();
                    dataTrasnferFormat = finalRequest.getTransferFormat();
                    bandwidthAverage = finalRequest.getBandwidth();
                    bandwidthAverageFormat = finalRequest.getBandwidthFormat();
                    listRequest.remove(finalRequest);
                }

                if(bandwidthAverage <= 0L){
                    long acumu = 0L;
                    int cont = 0;
                    for(IperfRequest resquest : listRequest){
                        acumu = acumu + resquest.getBandwidth();
                        cont++;
                    }
                    bandwidthAverage = acumu/cont;
                }
            }
        }
            switch (testType) {
                case 0:
                    direction = "Downlink";
                    protocol = "TCP";
                    break;
                case 1:
                    direction = "Uplink";
                    protocol = "TCP";
                    break;
                case 2:
                    direction = "Downlink";
                    protocol = "UDP";
                    break;
                case 3:
                    direction = "Uplink";
                    protocol = "UDP";
                    break;
            }
    }

    public String toString(){
        String ret = "";
        ret = ret + "Server IP: " + serverIp + "\n";
        ret = ret + "Server Port: " + serverPort + "\n";
        ret = ret + "Local IP: " + localIp + "\n";
        ret = ret + "Local Port: " + localPort + "\n";
        ret = ret + "Protocol: " + protocol + "\n";
        ret = ret + "Direction: " + direction + "\n";
        if(protocol.equals("UDP")){
            ret = ret + "Buffer size: " + UDPbufferSize + "\n";
            ret = ret + "Sent datagrams: " + UDPSentDatagrams + "\n";
        } else {
            ret = ret + "Window size: " + TCPwindowSize + "\n";
        }
        ret = ret + "Data Transfer: " + dataTrasnfer + " " + dataTrasnferFormat + "\n";
        ret = ret + "Bandwidth Average: " + bandwidthAverage + " " + bandwidthAverageFormat + "\n";
        if(listRequest!=null) {
            for (IperfRequest request : listRequest) {
                ret = ret + request.toString() + "\n";
            }
        }
        return ret;
    }

    public List<IperfRequest> getListRequest() {
        return listRequest;
    }

    public void setListRequest(List<IperfRequest> listRequest) {
        this.listRequest = listRequest;
    }

    public long getDataTrasnfer() {
        return dataTrasnfer;
    }

    public void setDataTrasnfer(long dataTrasnfer) {
        this.dataTrasnfer = dataTrasnfer;
    }

    public long getBandwidthAverage() {
        return bandwidthAverage;
    }

    public void setBandwidthAverage(long bandwidthAverage) {
        this.bandwidthAverage = bandwidthAverage;
    }

    public String getServerIp() {
        return serverIp;
    }

    public void setServerIp(String serverIp) {
        this.serverIp = serverIp;
    }

    public int getServerPort() {
        return serverPort;
    }

    public void setServerPort(int serverPort) {
        this.serverPort = serverPort;
    }

    public String getLocalIp() {
        return localIp;
    }

    public void setLocalIp(String localIp) {
        this.localIp = localIp;
    }

    public int getLocalPort() {
        return localPort;
    }

    public void setLocalPort(int localPort) {
        this.localPort = localPort;
    }

    public String getProtocol() {
        return protocol;
    }

    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    public String getDirection() {
        return direction;
    }

    public void setDirection(String direction) {
        this.direction = direction;
    }

    public String getWindowSize() {
        return TCPwindowSize;
    }

    public void setWindowSize(String windowSize) {
        this.TCPwindowSize = windowSize;
    }
    public String getDataTrasnferFormat() {
        return dataTrasnferFormat;
    }

    public void setDataTrasnferFormat(String dataTrasnferFormat) {
        this.dataTrasnferFormat = dataTrasnferFormat;
    }

    public String getBandwidthAverageFormat() {
        return bandwidthAverageFormat;
    }

    public void setBandwidthAverageFormat(String bandwidthAverageFormat) {
        this.bandwidthAverageFormat = bandwidthAverageFormat;
    }

    public String getTCPwindowSize() {
        return TCPwindowSize;
    }

    public void setTCPwindowSize(String TCPwindowSize) {
        this.TCPwindowSize = TCPwindowSize;
    }

    public String getUDPbufferSize() {
        return UDPbufferSize;
    }

    public void setUDPbufferSize(String UDPbufferSize) {
        this.UDPbufferSize = UDPbufferSize;
    }

    public long getUDPSentDatagrams() {
        return UDPSentDatagrams;
    }

    public void setUDPSentDatagrams(long UDPSentDatagrams) {
        this.UDPSentDatagrams = UDPSentDatagrams;
    }

    public void setId(long id) {
        this.id = id;
    }

    public long getId() {
        return id;
    }
}
