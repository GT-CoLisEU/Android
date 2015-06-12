package br.com.rnp.measurements.Ping;

/**
 * Created by matias on 13/01/15.
 */

import android.os.AsyncTask;
import android.os.Handler;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeoutException;

import br.com.rnp.measurements.Traceroute.TracerouteContainer;
import br.com.rnp.qoslibrary.R;

public class Ping {

    public enum Backend {
        UNIX {
            @Override
            public PingResult getResult(List<String> output) {
                return new UnixPingResult(output);
            }

        };

        public abstract PingResult getResult(List<String> output);
    }

    public static PingResult ping(PingArguments ping, Backend backend) {
        try {

            Process p = null;
            p = Runtime.getRuntime().exec(ping.getCommand());

            BufferedReader stdInput = new BufferedReader(new InputStreamReader(
                    p.getInputStream()));

            String s;

            List<String> lines = new ArrayList<String>();
            while ((s = stdInput.readLine()) != null) {

                lines.add(s);
                //System.out.println(s);
                if (s.contains("Destination Host Unreachable") || s.contains("0 received, 100% packet loss")) {
                    return null;
                }
            }

            p.destroy();
            PingResult result = backend.getResult(lines);
            result.setOriginalAddress(ping.getUrl());
            return result;
        } catch (Exception e) {
            //e.printStackTrace();
            return null;
        }

    }


}
