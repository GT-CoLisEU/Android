package br.com.rnp.measurements;

import android.util.Log;

import br.com.rnp.measurements.OWAMP.OWAMP;
import br.com.rnp.measurements.OWAMP.OWAMPArguments;
import br.com.rnp.measurements.OWAMP.OWAMPResult;

public class SampleCode {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

		OWAMPArguments arguments = new OWAMPArguments.Builder().url("8.8.8.8")
				.timeout(5).count(21).bytes(32).build();

		OWAMPResult results = OWAMP.ping(arguments, OWAMP.Backend.UNIX);

	    if(results != null) {
            System.out.println("TTL: " + results.ttl());

            System.out.println("RTT Average: " + results.rtt_avg());

            System.out.println("RTT Minimum: " + results.rtt_min());

            System.out.println("Received : " + results.received());
        } else {
            System.out.println("Result NULL");
        }
        //String str = stringFromNative();
        //System.out.println(str);

        //iperf -t 20 -i 1 -f m -c "200.237.192.68:4823"
        //Iperf iperf = new Iperf();

        /*IperfTask task = new IperfTask();
        task.execute("iperf -t 20 -i 1 -f m -c \"200.237.192.68:4823");*/


       /*TracerouteWithPing tracerouteWithPing;
       int maxTtl = 40;

       List<TracerouteContainer> traces;

       tracerouteWithPing = new TracerouteWithPing();
       traces = new ArrayList<TracerouteContainer>();


        tracerouteWithPing.executeTraceroute("8.8.8.8", maxTtl);*/


        /*PingArguments arguments = new PingArguments.Builder().url("143.54.10.200")
                .timeout(1).count(1).bytes(32).build();
        System.out.println(arguments.getCommand());

        PingResult results = Ping.ping(arguments, Ping.Backend.UNIX, 10L);

        System.out.println(results.toString());*/

        /*ArrayList<String> listGateways = new ArrayList<String>();
        listGateways.add("coliseu.inf.ufrgs.br");
        listGateways.add("200.130.99.55");

        GatewayManager manager = new GatewayManager();
        System.out.println("GATEWAY SELECTED: " + manager.selectGateway(listGateways));*/

	}


}
