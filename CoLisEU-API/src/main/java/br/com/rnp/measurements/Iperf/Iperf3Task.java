package br.com.rnp.measurements.Iperf;

import android.content.Context;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import br.com.rnp.measurements.Iperf.IperfResult;
import br.com.rnp.measurements.Network.ConnectionInfo;

/**
 * Created by matias on 21/01/15.
 */

public class Iperf3Task {

    public static int TCP_DOWNLINK = 0;
    public static int TCP_UPLINK = 1;
    public static int UDP_DOWNLINK = 2;
    public static int UDP_UPLINK = 3;

    private Context context;

    public Iperf3Task(Context contex) {
        this.context = contex;
    }

    public IperfResult execute(String serverIp, String serverPort, int testType){
        try {
            String command = makeCommand(testType, serverIp, serverPort);
            File asset = new File("file:///android_asset/conf");
            String result = stringFromJNI(command, context.getCacheDir().getAbsolutePath(), asset.getAbsolutePath());

            //
            System.out.println(result);
            IperfResult resultIperf = new IperfResult(result, testType, 3);
            return resultIperf;
            //return null;
        } catch (Exception e){
            e.printStackTrace();
            return null;
        }
    }

    public IperfResult execute(String command){
        try {

            File asset = new File("file:///android_asset/conf");
            String result = stringFromJNI(command, context.getCacheDir().getAbsolutePath(), asset.getAbsolutePath());

            System.out.println(result);
            //IperfResult resultIperf = new IperfResult(result, 3);
            //return resultIperf;
            return null;
        } catch (Exception e){
            e.printStackTrace();
            return null;
        }
    }

    public static String makeCommand(int testType, String serverIp, String serverPort) {
        switch (testType){
            case 0:
                return "iperf3 -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -f k";
            case 1:
                return "iperf3 -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -R -f k";
            case 2:
                return "iperf3 -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -u -b 1g -f k";
            case 3:
                return "iperf3 -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -u -R -b 1g -f k --get-server-output";
            default:
                return "iperf3 -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -f k";
        }
    }

    public static String makeCommand(int testType, String serverIpPort) {
        //bwctl -T iperf3 -t 30 -O 4 -c "ps-bw.ansp.ampath.net:4823"
        switch (testType){
            case 0:
                return "iperf3 -t 10 -i 1 -f k -c " + serverIpPort;
            case 1:
                return "iperf3 -t 10 -i 1 -R -f k -c " + serverIpPort;
            case 2:
                return "iperf3 -t 10 -i 1 -u -b 1g -f k -c " + serverIpPort;
            case 3:
                return "iperf3 -t 10 -i 1 -u -R -b 1g -f k --get-server-output -c " + serverIpPort;
            default:
                return "iperf3 -t 10 -i 1 -f k -c " + serverIpPort;
        }
    }

    public native String  stringFromJNI(String command, String tmpfile, String confdir);

    public native String  unimplementedStringFromJNI();

    static {
        System.loadLibrary("measurements");
    }
}
