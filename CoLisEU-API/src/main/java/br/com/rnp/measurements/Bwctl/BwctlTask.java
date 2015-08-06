package br.com.rnp.measurements.Bwctl;

import android.content.Context;

import br.com.rnp.measurements.Iperf.Iperf2Task;
import br.com.rnp.measurements.Iperf.IperfResult;
import br.com.rnp.measurements.Iperf.Iperf3Task;
import br.com.rnp.measurements.Utils.DialogUtils;
import br.com.rnp.qoslibrary.R;


/**
 * Created by matias on 21/01/15.
 */

public class BwctlTask {

    public static int TOOL_IPERF = 0;
    public static int TOOL_IPERF3 = 1;
    public static int TCP_DOWNLINK = 0;
    public static int TCP_UPLINK = 1;
    public static int UDP_DOWNLINK = 2;
    public static int UDP_UPLINK = 3;

    private Context context;

    public BwctlTask(Context contex) {
        this.context = contex;
    }

    public IperfResult execute(int tool, String serverIp, String serverPort, int testType){
        try {
            String command = "";
            switch (tool) {
                case 0:
                    //char **commandf = (char *[]){"bwctl", "-T", "iperf", "-t", "20", "-i", "1", "-f", "m", "-c", "150.162.249.82:4823", "-a", "3"};
                    command = "bwctl -a 10 -T " + Iperf2Task.makeCommand(testType, (serverIp + ":" + serverPort));
                    break;
                case 1:
                    command = "bwctl -a 10 -T " + Iperf3Task.makeCommand(testType, (serverIp + ":" + serverPort));
                    break;
            }

            String result = stringFromJNI(command, context.getCacheDir().getAbsolutePath());

            //BwctlResult resultBwctl = new BwctlResult(result);
            System.out.println("BWCTL result: \n" + result);
            if(result.contains("bwctl: remote peer cancelled test")){
                DialogUtils.makeDialog(context.getResources().getString(R.string.erro), context.getResources().getString(R.string.remote_peer_cancelled), context);
                return null;
            } else if (result.contains("erro 123")){
                //make dialog
                return null;
            } else {
                IperfResult resultIperf = new IperfResult(result, testType, 2);
                return resultIperf;
            }

        } catch (Exception e){
            e.printStackTrace();
            return null;
        }
    }

    public BwctlResult execute(String command){
        try {

            String result = stringFromJNI(command, context.getCacheDir().getAbsolutePath());

            //BwctlResult resultBwctl = new BwctlResult(result);
            System.out.println(result);

            //return resultBwctl;
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
