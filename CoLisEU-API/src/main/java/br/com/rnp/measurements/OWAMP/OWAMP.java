package br.com.rnp.measurements.OWAMP;

/**
 * Created by matias on 13/01/15.
 */

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class OWAMP {
//ping -c 2 -W 5 -s 32 coliseu.inf.ufrgs.br
    public enum Backend {
        UNIX {
            @Override
            public OWAMPResult getResult(List<String> output) {
                return new OWAMPResult(output);
            }

        };

        public abstract OWAMPResult getResult(List<String> output);
    }

    public static OWAMPResult ping(OWAMPArguments ping, Backend backend) {
        try {

            Process p = null;
            p = Runtime.getRuntime().exec(ping.getCommand());

            BufferedReader stdInput = new BufferedReader(new InputStreamReader(
                    p.getInputStream()));

            System.out.println(stdInput.toString());

            String s;

            List<String> lines = new ArrayList<String>();
            while ((s = stdInput.readLine()) != null) {

                lines.add(s);
                System.out.println(s);
                if (s.contains("Destination Host Unreachable") || s.contains("0 received, 100% packet loss")) {
                    return null;
                }
            }
            try {
                p.destroy();
            } catch (Exception e){
                // Do Noting
            }
            if(!lines.isEmpty()) {
                OWAMPResult result = backend.getResult(lines);
                result.setOriginalAddress(ping.getUrl());
                return result;
            }
        } catch (Exception e) {
            //e.printStackTrace();
            return null;
        }
        return null;
    }


}
