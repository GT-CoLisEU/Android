/*The MIT License (MIT)

Copyright (c) 2012 Thomas Goossens

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

package br.com.rnp.measurements.OWAMP;

/**
 *
 * @author jpingy
 * @page https://code.google.com/p/jpingy/
 *
 * Modified by matias on 13/01/15.
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
