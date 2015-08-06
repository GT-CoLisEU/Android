package br.com.rnp.measurements.Bwctld;

import android.content.Context;

/**
 * Created by matias on 21/01/15.
 */

public class BwctldTask {
    private Context context;

    public BwctldTask(Context contex) {
        this.context = contex;
    }

    public BwctldResult execute(){
        try {
            String command = "bwctld";

            String result = stringFromJNI(command, context.getCacheDir().getAbsolutePath());

            //BwctldResult resultBwctld = new BwctldResult(result);
            System.out.println(result);

            //return resultBwctld;
            return null;
        } catch (Exception e){
            e.printStackTrace();
            return null;
        }
    }

    public BwctldResult execute(String command){
        try {

            String result = stringFromJNI(command, context.getCacheDir().getAbsolutePath());

            //BwctldResult resultBwctld = new BwctldResult(result);
            System.out.println(result);

            //return resultBwctld;
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
