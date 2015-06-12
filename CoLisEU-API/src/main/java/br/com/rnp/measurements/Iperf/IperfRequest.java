package br.com.rnp.measurements.Iperf;

/**
 * Created by matias on 13/04/15.
 */
public class IperfRequest {
    private String interval;
    private long transfer;
    private String transferFormat;
    private long bandwidth;
    private String bandwidthFormat;

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    private long id;

    public IperfRequest(){}

    public IperfRequest(String interval, long transfer, String transferFormat, long bandwidth, String bandwidthFormat){
        this.setInterval(interval);
        this.setTransfer(transfer);
        this.setTransferFormat(transferFormat);
        this.setBandwidth(bandwidth);
        this.setBandwidthFormat(bandwidthFormat);
    }

    public IperfRequest(String iperfRequestLine){
        //[ 37]  0.0- 1.0 sec  2304 KBytes  18874 Kbits/sec
        while(iperfRequestLine.contains("  ")) {
            iperfRequestLine = iperfRequestLine.replaceAll("  ", " ");
        }
        iperfRequestLine = iperfRequestLine.replaceAll("- ", "-");
        String[] lineSplit = iperfRequestLine.split(" ");
        //lineSplit[0] = [
        //lineSplit[1] = 37]
        //lineSplit[2] = 0.0-
        //lineSplit[3] = 1.0
        //lineSplit[4] = sec
        //lineSplit[5] = 2304
        //lineSplit[6] = KBytes
        //lineSplit[7] = 18874
        //lineSplit[8] = Kbits/sec
        //for(int x = 0;x<lineSplit.length;x++){
        //    System.out.println(x + " = " +lineSplit[x]);
        //}
        if(lineSplit[0].equals("[")) {
            this.setInterval(lineSplit[2] + lineSplit[3]);
            this.setTransfer(toLong(lineSplit[4]));
            this.setTransferFormat(lineSplit[5]);
            this.setBandwidth(toLong(lineSplit[6]));
            this.setBandwidthFormat(lineSplit[7]);
        } else {
            this.setInterval(lineSplit[1] + lineSplit[2]);
            this.setTransfer(toLong(lineSplit[3]));
            this.setTransferFormat(lineSplit[4]);
            this.setBandwidth(toLong(lineSplit[5]));
            this.setBandwidthFormat(lineSplit[6]);
        }

    }

    private long toLong(String value) {
        try {

            Long rValue;
            if (value.contains("0.00")) {
                return 0L;
            } else if (value.contains(".")) {
                value = value.replace(".", "");
                rValue = Long.valueOf(value).longValue() * 8;
            } else {
                rValue = Long.valueOf(value).longValue();
            }
            if ((rValue / 1000) > 100) {
                //rValue = rValue/(rValue.toString().length()-2);
                return 0L;
            }
            return rValue;
        } catch (NumberFormatException e){
            return 0L;
        }
    }

    public String toString(){
        String ret = "";
        ret = ret + "Interval: " + interval + "  ";
        ret = ret + "Transfer: " + transfer + " " + transferFormat + "  ";
        ret = ret + "Bandwidth: " + bandwidth + " " + bandwidthFormat;
        return ret;
    }

    public String getInterval() {
        return interval;
    }

    public void setInterval(String interval) {
        this.interval = interval;
    }

    public long getTransfer() {
        return transfer;
    }

    public void setTransfer(long transfer) {
        this.transfer = transfer;
    }

    public String getTransferFormat() {
        return transferFormat;
    }

    public void setTransferFormat(String transferFormat) {
        this.transferFormat = transferFormat;
    }

    public long getBandwidth() {
        return bandwidth;
    }

    public void setBandwidth(long bandwidth) {
        this.bandwidth = bandwidth;
    }

    public String getBandwidthFormat() {
        return bandwidthFormat;
    }

    public void setBandwidthFormat(String bandwidthFormat) {
        this.bandwidthFormat = bandwidthFormat;
    }
}
