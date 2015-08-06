package br.com.rnp.measurements;

/**
 * Created by matias on 19/01/15.
 */
public class RTT {
    public RTT(){
        String libName = "bwctl"; // the module name of the library, without .so
        System.loadLibrary( libName );
    }

}
