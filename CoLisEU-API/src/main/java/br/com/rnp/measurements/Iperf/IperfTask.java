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

public class IperfTask {

    public static int TCP_DOWNLINK = 0;
    public static int TCP_UPLINK = 1;
    public static int UDP_DOWNLINK = 2;
    public static int UDP_UPLINK = 3;

    private Context context;

    public IperfTask(Context contex) {
        this.context = contex;
    }

    //protected IperfResult doInBackground(String... parans) {
    public IperfResult execute(String serverIp, String serverPort, int testType){
        try {
            ConnectionInfo connection = new ConnectionInfo(context);

            Log.d("JNI", "Starting");
            String result = stringFromJNI(connection.getIpAddress(), serverIp, serverPort, testType, context.getCacheDir().getAbsolutePath());
            Log.d("RESULT_JNI", result);
            //Get the text file
            Log.d("RESULT_JNI", "OPEN FILE");
            File file = new File(context.getCacheDir(), "output.jni");

            //Read text from file
            StringBuilder text = new StringBuilder();

            try {
                BufferedReader br = new BufferedReader(new FileReader(file));
                String line;

                while ((line = br.readLine()) != null) {
                    text.append(line);
                    text.append('\n');
                }
                br.close();
            } catch (IOException e) {
                e.printStackTrace();
                //You'll need to add proper error handling here
                return null;
            }
            file.delete();

            IperfResult resultIperf = new IperfResult(text, testType);
            System.out.println(resultIperf.toString());
            //System.out.println(text);

            return resultIperf;
        } catch (Exception e){
            e.printStackTrace();
            return null;
        }
    }


    /* A native method that is implemented by the
 * 'hello-jni' native library, which is packaged
 * with this application.
 */
    public native String  stringFromJNI(String ipaddr, String serveripaddr,String serverport, int testype, String tmpfile);

    /* This is another native method declaration that is *not*
     * implemented by 'hello-jni'. This is simply to show that
     * you can declare as many native methods in your Java code
     * as you want, their implementation is searched in the
     * currently loaded native libraries only the first time
     * you call them.
     *
     * Trying to call this function will result in a
     * java.lang.UnsatisfiedLinkError exception !
     */
    public native String  unimplementedStringFromJNI();

    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.hellojni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("measurements");
    }
}
