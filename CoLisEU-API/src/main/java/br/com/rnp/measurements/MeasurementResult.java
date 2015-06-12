package br.com.rnp.measurements;

import android.util.Log;

import java.util.List;

import br.com.rnp.measurements.Iperf.IperfResult;
import br.com.rnp.measurements.OWAMP.OWAMPResult;
import br.com.rnp.measurements.Traceroute.TracerouteContainer;

/**
 * Created by matias on 22/04/15.
 */
public class MeasurementResult {

    String measurementPoint = "";
    private long id;

    public String getMeasurementPoint() {
        //Log.d("MEASUREMENT_POINT: ", measurementPoint);
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

    private IperfResult TCPDownlink = null;
    private IperfResult TCPUplink = null;
    private IperfResult UDPDownlink = null;
    private IperfResult UDPUplink = null;
    private OWAMPResult RTT = null;
    private List<TracerouteContainer> traceRoute = null;

    public MeasurementResult(String measurementPoint, IperfResult TCPDownlink, IperfResult TCPUplink, IperfResult UDPDownlink, IperfResult UDPUplink, OWAMPResult RTT, List<TracerouteContainer> traceRoute) {
        this.measurementPoint = measurementPoint;
        this.TCPDownlink = TCPDownlink;
        this.TCPUplink = TCPUplink;
        this.UDPDownlink = UDPDownlink;
        this.UDPUplink = UDPUplink;
        this.RTT = RTT;
        this.traceRoute = traceRoute;
    }
    public MeasurementResult(){}

    public void setId(long id) {
        this.id = id;
    }

    public long getId() {
        return id;
    }
}
