package br.com.rnp.measurements;

import android.content.Context;

import java.util.ArrayList;
import java.util.List;

import br.com.rnp.measurements.Bwctl.BwctlTask;
import br.com.rnp.measurements.Iperf.IperfResult;
import br.com.rnp.measurements.Iperf.Iperf2Task;
import br.com.rnp.measurements.Iperf.Iperf3Task;
import br.com.rnp.measurements.OWAMP.OWAMP;
import br.com.rnp.measurements.OWAMP.OWAMPArguments;
import br.com.rnp.measurements.OWAMP.OWAMPResult;
import br.com.rnp.measurements.Traceroute.TracerouteContainer;
import br.com.rnp.measurements.Traceroute.TracerouteWithPing;

/**
 * Created by matias on 14/04/15.
 */
public class QoSMeasurementTask {

    Context context;
    String measurementPoint;
    IperfResult iperfTCPDownlink;
    IperfResult iperfTCPUplink;
    IperfResult iperfUDPDownlink;
    IperfResult iperfUDPUplink;

    OWAMPResult RTT;


    public IperfResult getIperfTCPDownlink() {
        return iperfTCPDownlink;
    }

    public void setIperfTCPDownlink(IperfResult iperfTCPDownlink) {
        this.iperfTCPDownlink = iperfTCPDownlink;
    }

    public IperfResult getIperfTCPUplink() {
        return iperfTCPUplink;
    }

    public void setIperfTCPUplink(IperfResult iperfTCPUplink) {
        this.iperfTCPUplink = iperfTCPUplink;
    }

    public IperfResult getIperfUDPDownlink() {
        return iperfUDPDownlink;
    }

    public void setIperfUDPDownlink(IperfResult iperfUDPDownlink) {
        this.iperfUDPDownlink = iperfUDPDownlink;
    }

    public IperfResult getIperfUDPUplink() {
        return iperfUDPUplink;
    }

    public void setIperfUDPUplink(IperfResult iperfUDPUplink) {
        this.iperfUDPUplink = iperfUDPUplink;
    }


    private List<TracerouteContainer> traceRoute;

    public String getMeasurementPoint() {
        return measurementPoint;
    }

    public void setMeasurementPoint(String measurementPoint) {
        this.measurementPoint = measurementPoint;
    }

    public OWAMPResult getRTT() {
        return RTT;
    }

    public void setRTT(OWAMPResult RTT) {
        this.RTT = RTT;
    }

    public List<TracerouteContainer> getTraceRoute() {
        return traceRoute;
    }

    public void setTraceRoute(List<TracerouteContainer> traceRoute) {
        this.traceRoute = traceRoute;
    }

    public QoSMeasurementTask(Context context, String measurementPoint){
        this.context = context;
        this.measurementPoint = measurementPoint;
    }

    public IperfResult iperf3TCPDownlink(String measurementPort){
        Iperf3Task iperf = new Iperf3Task(context);
        iperfTCPDownlink = iperf.execute(measurementPoint, measurementPort, Iperf3Task.TCP_DOWNLINK);
        return iperfTCPDownlink;
    }

    public IperfResult iperf3TCPUplink(String measurementPort){
        Iperf3Task iperf = new Iperf3Task(context);
        iperfTCPUplink = iperf.execute(measurementPoint, measurementPort, Iperf3Task.TCP_UPLINK);
        return iperfTCPUplink;
    }

    public IperfResult iperf3UDPDownlink(String measurementPort){
        Iperf3Task iperf = new Iperf3Task(context);
        iperfUDPDownlink = iperf.execute(measurementPoint, measurementPort, Iperf3Task.UDP_DOWNLINK);
        return iperfUDPDownlink;
    }

    public IperfResult iperf3UDPUplink(String measurementPort){
        Iperf3Task iperf = new Iperf3Task(context);
        iperfUDPUplink = iperf.execute(measurementPoint, measurementPort, Iperf3Task.UDP_UPLINK);
        return iperfUDPUplink;
    }


    public IperfResult iperf2TCPDownlink(String measurementPort){
        Iperf2Task iperf = new Iperf2Task(context);
        iperfTCPDownlink = iperf.execute(measurementPoint, measurementPort, Iperf2Task.TCP_DOWNLINK);
        return iperfTCPDownlink;
    }

    public IperfResult iperf2TCPUplink(String measurementPort){
        Iperf2Task iperf = new Iperf2Task(context);
        iperfTCPUplink = iperf.execute(measurementPoint, measurementPort, Iperf2Task.TCP_UPLINK);
        return iperfTCPUplink;
    }

    public IperfResult iperf2UDPDownlink(String measurementPort){
        Iperf2Task iperf = new Iperf2Task(context);
        iperfUDPDownlink = iperf.execute(measurementPoint, measurementPort, Iperf2Task.UDP_DOWNLINK);
        return iperfUDPDownlink;
    }

    public IperfResult iperf2UDPUplink(String measurementPort){
        Iperf2Task iperf = new Iperf2Task(context);
        iperfUDPUplink = iperf.execute(measurementPoint, measurementPort, Iperf2Task.UDP_UPLINK);
        return iperfUDPUplink;
    }


    public OWAMPResult measureRTT(){
        OWAMPArguments arguments = new OWAMPArguments.Builder().url(measurementPoint)
                .timeout(5).count(10).bytes(32).build();

        RTT = OWAMP.ping(arguments, OWAMP.Backend.UNIX);

        System.out.println("TTL: " + RTT.ttl());

        System.out.println("RTT Average: " + RTT.rtt_avg());

        System.out.println("RTT Minimum: " + RTT.rtt_min());

        System.out.println("Received : " + RTT.received());

        return  RTT;
    }

    public List<TracerouteContainer> measureTraceroute(){
        TracerouteWithPing tracerouteWithPing;
        int maxTtl = 40;

        List<TracerouteContainer> traces;

        tracerouteWithPing = new TracerouteWithPing(context);
        traces = new ArrayList<TracerouteContainer>();

        tracerouteWithPing.executeTraceroute(measurementPoint, maxTtl, traces);
        this.traceRoute = traces;
        return traces;
    }

    public MeasurementResult getMeasurementResult() {
        return new MeasurementResult(measurementPoint, iperfTCPDownlink, iperfTCPUplink, iperfUDPDownlink, iperfUDPUplink, RTT, traceRoute);
    }

    public IperfResult BWCTLiperf2UDPUplink(String bwctlPort) {
        BwctlTask task = new BwctlTask(context);
        iperfUDPUplink = task.execute(BwctlTask.TOOL_IPERF, measurementPoint, bwctlPort, BwctlTask.UDP_UPLINK);
        return iperfUDPUplink;
    }

    public IperfResult BWCTLiperf2UDPDownlink(String bwctlPort) {
        BwctlTask task = new BwctlTask(context);
        iperfUDPDownlink = task.execute(BwctlTask.TOOL_IPERF, measurementPoint, bwctlPort, BwctlTask.UDP_DOWNLINK);
        return iperfUDPDownlink;
    }

    public IperfResult BWCTLiperf2TCPUplink(String bwctlPort) {
        BwctlTask task = new BwctlTask(context);
        iperfTCPUplink = task.execute(BwctlTask.TOOL_IPERF, measurementPoint, bwctlPort, BwctlTask.TCP_UPLINK);
        return iperfTCPUplink;
    }

    public IperfResult BWCTLiperf2TCPDownlink(String bwctlPort) {
        BwctlTask task = new BwctlTask(context);
        iperfTCPDownlink = task.execute(BwctlTask.TOOL_IPERF, measurementPoint, bwctlPort, BwctlTask.TCP_DOWNLINK);
        return iperfTCPDownlink;
    }
}
