package br.com.rnp.measurements.Network;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import br.com.rnp.measurements.OWAMP.OWAMP;
import br.com.rnp.measurements.OWAMP.OWAMPArguments;
import br.com.rnp.measurements.OWAMP.OWAMPResult;

/**
 * Created by matias on 24/02/15.
 */
public class GatewayManager {


    public GatewayManager(){

    }

    public String selectGateway(ArrayList<String> listOfGateways){
        ArrayList<OWAMPResult> results = new ArrayList<OWAMPResult>();
        for(String url:listOfGateways) {
            OWAMPArguments arguments = null;
            /*if(url.startsWith("https://")){
                arguments = new PingArguments.Builder().url(url.replaceAll("https://", ""))
                        .timeout(iperf-2.0.5).count(2).bytes(32).build();
            }else {*/
                arguments = new OWAMPArguments.Builder().url(url)
                        .timeout(5).count(2).bytes(32).build();
            //}

            OWAMPResult result = OWAMP.ping(arguments, OWAMP.Backend.UNIX);
            //System.out.println(result.address() + " delay: " + result.rtt_avg());
            results.add(result);
        }

        //Sorting
        Collections.sort(results, new Comparator<OWAMPResult>() {
            @Override
            public int compare(OWAMPResult result1, OWAMPResult result2) {
                if (result1 == null) {
                    return (result2 == null) ? 0 : 1;
                }
                if (result2 == null) {
                    return -1;
                }
                return Float.valueOf(result1.rtt_avg()).compareTo(result2.rtt_avg());
            }
        });

        for(OWAMPResult result: results){
            if(result != null){
                return result.getOriginalAddress();
            }
        }
        return null;

    }
}
