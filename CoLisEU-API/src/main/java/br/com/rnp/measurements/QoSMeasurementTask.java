package br.com.rnp.measurements;

import android.content.Context;
import android.os.AsyncTask;

import java.util.ArrayList;
import java.util.List;

import br.com.rnp.measurements.Iperf.IperfResult;
import br.com.rnp.measurements.Iperf.IperfTask;
import br.com.rnp.measurements.Network.ConnectionInfo;
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
    IperfResult TCPDownlink;
    IperfResult TCPUplink;
    IperfResult UDPDownlink;
    IperfResult UDPUplink;
    OWAMPResult RTT;
    private List<TracerouteContainer> traceRoute;

    public String getMeasurementPoint() {
        return measurementPoint;
    }

    public void setMeasurementPoint(String measurementPoint) {
        this.measurementPoint = measurementPoint;
    }

    public IperfResult getTCPDownlink() {
        return TCPDownlink;
    }

    public void setTCPDownlink(IperfResult TCPDownlink) {
        this.TCPDownlink = TCPDownlink;
    }

    public IperfResult getTCPUplink() {
        return TCPUplink;
    }

    public void setTCPUplink(IperfResult TCPUplink) {
        this.TCPUplink = TCPUplink;
    }

    public IperfResult getUDPDownlink() {
        return UDPDownlink;
    }

    public void setUDPDownlink(IperfResult UDPDownlink) {
        this.UDPDownlink = UDPDownlink;
    }

    public IperfResult getUDPUplink() {
        return UDPUplink;
    }

    public void setUDPUplink(IperfResult UDPUplink) {
        this.UDPUplink = UDPUplink;
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

    public IperfResult measureTCPDownlink(String measurementPort){
        IperfTask iperf = new IperfTask(context);
        TCPDownlink = iperf.execute(measurementPoint, measurementPort, IperfTask.TCP_DOWNLINK);
        return TCPDownlink;
    }

    public IperfResult measureTCPUplink(String measurementPort){
        IperfTask iperf = new IperfTask(context);
        TCPUplink = iperf.execute(measurementPoint, measurementPort, IperfTask.TCP_UPLINK);
        return TCPUplink;
    }

    public IperfResult measureUDPDownlink(String measurementPort){
        IperfTask iperf = new IperfTask(context);
        UDPDownlink = iperf.execute(measurementPoint, measurementPort, IperfTask.UDP_DOWNLINK);
        return UDPDownlink;
    }

    public IperfResult measureUDPUplink(String measurementPort){
        IperfTask iperf = new IperfTask(context);
        UDPUplink = iperf.execute(measurementPoint, measurementPort, IperfTask.UDP_UPLINK);
        return UDPUplink;
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
        return new MeasurementResult(measurementPoint, TCPDownlink, TCPUplink, UDPDownlink, UDPUplink, RTT, traceRoute);
    }
}
