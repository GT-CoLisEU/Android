package br.com.rnp.measurements.Iperf;

import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import br.com.rnp.measurements.Network.ConnectionInfo;

/**
 * Created by matias on 21/01/15.
 */

public class Iperf2Task {

    public static int TCP_DOWNLINK = 0;
    public static int TCP_UPLINK = 1;
    public static int UDP_DOWNLINK = 2;
    public static int UDP_UPLINK = 3;

    private Context context;

    public Iperf2Task(Context contex) {
        this.context = contex;
    }

    public IperfResult execute(String serverIp, String serverPort, int testType){
        try {
            String command = makeCommand(testType, serverIp, serverPort);

            String result = stringFromJNI(command, context.getCacheDir().getAbsolutePath());

            System.out.println(result);
            IperfResult resultIperf = new IperfResult(result, testType, 2);
            return resultIperf;
        } catch (Exception e){
            e.printStackTrace();
            return null;
        }
    }

    public static String makeCommand(int testType, String serverIp, String serverPort) {
        switch (testType){
            case 0:
                return "iperf -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -f k";
            case 1:
                return "iperf -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -R -f k";
            case 2:
                return "iperf -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -u -b 1g -f k";
            case 3:
                return "iperf -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -u -R -b 1g -f k";
            default:
                return "iperf -c " + serverIp + " -t 10 -i 1 -p " + serverPort + " -f k";
        }
    }

    public static String makeCommand(int testType, String serverIpPort) {
        switch (testType){
            case 0:
                return "iperf -t 10 -i 1 -f k -c " + serverIpPort;
            case 1:
                return "iperf -t 10 -i 1 -R -f k -c " + serverIpPort;
            case 2:
                return "iperf -t 10 -i 1 -u -b 1g -f k -c " + serverIpPort;
            case 3:
                return "iperf -t 10 -i 1 -u -R -b 1g -f k -c " + serverIpPort;
            default:
                return "iperf -t 10 -i 1 -f k -c " + serverIpPort;
        }
    }

    public IperfResult execute(String command){
        try {

            String result = stringFromJNI(command, context.getCacheDir().getAbsolutePath());

            System.out.println(result);
            //IperfResult resultIperf = new IperfResult(result);
            //return resultIperf;
            return null;
        } catch (Exception e){
            e.printStackTrace();
            return null;
        }
    }

    public native String  stringFromJNI(String command, String tmpfile);

    public native String  unimplementedStringFromJNI();

    static {
        System.loadLibrary("measurements");
    }
}
