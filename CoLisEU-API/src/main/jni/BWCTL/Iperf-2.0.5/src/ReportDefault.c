/*--------------------------------------------------------------- 
 * Copyright (c) 1999,2000,2001,2002,2003                              
 * The Board of Trustees of the University of Illinois            
 * All Rights Reserved.                                           
 *--------------------------------------------------------------- 
 * Permission is hereby granted, free of charge, to any person    
 * obtaining a copy of this software (Iperf) and associated       
 * documentation files (the "Software"), to deal in the Software  
 * without restriction, including without limitation the          
 * rights to use, copy, modify, merge, publish, distribute,        
 * sublicense, and/or sell copies of the Software, and to permit     
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions: 
 *
 *     
 * Redistributions of source code must retain the above 
 * copyright notice, this list of conditions and 
 * the following disclaimers. 
 *
 *     
 * Redistributions in binary form must reproduce the above 
 * copyright notice, this list of conditions and the following 
 * disclaimers in the documentation and/or other materials 
 * provided with the distribution. 
 * 
 *     
 * Neither the names of the University of Illinois, NCSA, 
 * nor the names of its contributors may be used to endorse 
 * or promote products derived from this Software without
 * specific prior written permission. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * ________________________________________________________________
 * National Laboratory for Applied Network Research 
 * National Center for Supercomputing Applications 
 * University of Illinois at Urbana-Champaign 
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________ 
 *
 * ReportDefault.c
 * by Kevin Gibbs <kgibbs@nlanr.net>
 *
 * ________________________________________________________________ */

#include "../include/headers.h"
#include "../include/Settings.hpp"
#include "../include/util.h"
#include "../include/Reporter.h"
#include "../include/report_default.h"
#include "../include/Thread.h"
#include "../include/Locale.h"
#include "../include/PerfSocket.hpp"
#include "../include/SocketAddr.h"
#include <stdio.h>
#include <android/log.h>

extern FILE *output_jni;
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Prints transfer reports in default style
 */
void reporter_printstats(Transfer_Info *stats ) {
    static char header_printed = 0;

    byte_snprintf( buffer, sizeof(buffer)/2, (double) stats->TotalLen,
                   toupper( stats->mFormat));
    byte_snprintf( &buffer[sizeof(buffer)/2], sizeof(buffer)/2,
                   stats->TotalLen / (stats->endTime - stats->startTime), 
                   stats->mFormat);

    if ( stats->mUDP != (char)kMode_Server ) {
        // TCP Reporting
        if( !header_printed ) {
            fprintf(output_jni,"%s", report_bw_header);
            __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", report_bw_header);
            header_printed = 1;
        }
        fprintf(output_jni,report_bw_format, stats->transferID,
                stats->startTime, stats->endTime, 
                buffer, &buffer[sizeof(buffer)/2] );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_bw_format, stats->transferID,
                                                                                stats->startTime, stats->endTime,
                                                                                buffer, &buffer[sizeof(buffer)/2] );
    } else {
        // UDP Reporting
        if( !header_printed ) {
            fprintf(output_jni,"%s", report_bw_jitter_loss_header);
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", report_bw_jitter_loss_header);
            header_printed = 1;
        }
        fprintf(output_jni,report_bw_jitter_loss_format, stats->transferID,
                stats->startTime, stats->endTime, 
                buffer, &buffer[sizeof(buffer)/2],
                stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                (100.0 * stats->cntError) / stats->cntDatagrams );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_bw_jitter_loss_format, stats->transferID,
                                                                                stats->startTime, stats->endTime,
                                                                                buffer, &buffer[sizeof(buffer)/2],
                                                                                stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                                                                                (100.0 * stats->cntError) / stats->cntDatagrams );
        if ( stats->cntOutofOrder > 0 ) {
            fprintf(output_jni, report_outoforder,
                    stats->transferID, stats->startTime, 
                    stats->endTime, stats->cntOutofOrder );
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_outoforder,
                                                                                        stats->transferID, stats->startTime,
                                                                                        stats->endTime, stats->cntOutofOrder );
        }
    }
    if ( stats->free == 1 && stats->mUDP == (char)kMode_Client ) {
        fprintf(output_jni, report_datagrams, stats->transferID, stats->cntDatagrams );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_datagrams, stats->transferID, stats->cntDatagrams );
    }
}


/*
 * Prints multiple transfer reports in default style
 */
void reporter_multistats(Transfer_Info *stats ) {

    byte_snprintf( buffer, sizeof(buffer)/2, (double) stats->TotalLen,
                   toupper( stats->mFormat));
    byte_snprintf( &buffer[sizeof(buffer)/2], sizeof(buffer)/2,
                   stats->TotalLen / (stats->endTime - stats->startTime), 
                   stats->mFormat);

    if ( stats->mUDP != (char)kMode_Server ) {
        // TCP Reporting
        fprintf(output_jni,report_sum_bw_format,
                stats->startTime, stats->endTime, 
                buffer, &buffer[sizeof(buffer)/2] );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_sum_bw_format,
                                                                                stats->startTime, stats->endTime,
                                                                                buffer, &buffer[sizeof(buffer)/2] );
    } else {
        // UDP Reporting
        fprintf(output_jni,report_sum_bw_jitter_loss_format,
                stats->startTime, stats->endTime, 
                buffer, &buffer[sizeof(buffer)/2],
                stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                (100.0 * stats->cntError) / stats->cntDatagrams );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_sum_bw_jitter_loss_format,
                                                                                stats->startTime, stats->endTime,
                                                                                buffer, &buffer[sizeof(buffer)/2],
                                                                                stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                                                                                (100.0 * stats->cntError) / stats->cntDatagrams );
        if ( stats->cntOutofOrder > 0 ) {
            fprintf(output_jni,report_sum_outoforder,
                    stats->startTime, 
                    stats->endTime, stats->cntOutofOrder );
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_sum_outoforder,
                                                                                        stats->startTime,
                                                                                        stats->endTime, stats->cntOutofOrder );
        }
    }
    if ( stats->free == 1 && stats->mUDP == (char)kMode_Client ) {
        fprintf(output_jni,report_sum_datagrams, stats->cntDatagrams );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_sum_datagrams, stats->cntDatagrams );
    }
}

/*
 * Prints server transfer reports in default style
 */
void reporter_serverstats(Connection_Info *nused, Transfer_Info *stats ) {
    fprintf(output_jni,server_reporting, stats->transferID );
     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",server_reporting, stats->transferID );
    reporter_printstats( stats );
}

/*
 * Report the client or listener Settings in default style
 */
void reporter_reportsettings( ReporterData *data ) {
    int win, win_requested;

    win = getsock_tcp_windowsize( data->info.transferID,
                  (data->mThreadMode == kMode_Listener ? 0 : 1) );
    win_requested = data->mTCPWin;

    fprintf(output_jni,"%s", separator_line );
     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", separator_line );
    if ( data->mThreadMode == kMode_Listener ) {
        fprintf(output_jni,server_port,
                (isUDP( data ) ? "UDP" : "TCP"), 
                data->mPort );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",server_port,
                                                                                (isUDP( data ) ? "UDP" : "TCP"),
                                                                                data->mPort );
    } else {
        fprintf(output_jni,client_port,
                data->mHost,
                (isUDP( data ) ? "UDP" : "TCP"),
                data->mPort );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI", client_port,
                                                                                data->mHost,
                                                                                (isUDP( data ) ? "UDP" : "TCP"),
                                                                                data->mPort );
    }
    if ( data->mLocalhost != NULL ) {
        fprintf(output_jni,bind_address, data->mLocalhost );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",bind_address, data->mLocalhost );
        if ( SockAddr_isMulticast( &data->connection.local ) ) {
            fprintf(output_jni,join_multicast, data->mLocalhost );
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",join_multicast, data->mLocalhost );
        }
    }

    if ( isUDP( data ) ) {
        fprintf(output_jni, (data->mThreadMode == kMode_Listener ?
                                   server_datagram_size : client_datagram_size),
                data->mBufLen );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",(data->mThreadMode == kMode_Listener ?
                                                                                                   server_datagram_size : client_datagram_size),
                                                                                data->mBufLen );
        if ( SockAddr_isMulticast( &data->connection.peer ) ) {
            fprintf(output_jni, multicast_ttl, data->info.mTTL);
            __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",multicast_ttl, data->info.mTTL);
        }
    }
    byte_snprintf( buffer, sizeof(buffer), win,
                   toupper( data->info.mFormat));
    fprintf(output_jni, "%s: %s", (isUDP( data ) ?
                                udp_buffer_size : tcp_window_size), buffer );
     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s: %s", (isUDP( data ) ?
                                                                                            udp_buffer_size : tcp_window_size), buffer );

    if ( win_requested == 0 ) {
        fprintf(output_jni, " %s", window_default );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI"," %s", window_default );
    } else if ( win != win_requested ) {
        byte_snprintf( buffer, sizeof(buffer), win_requested,
                       toupper( data->info.mFormat));
        fprintf(output_jni, warn_window_requested, buffer );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",warn_window_requested, buffer );
    }
    fprintf(output_jni, "\n" );
    // __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","\n" );
    fprintf(output_jni, "%s", separator_line );
     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", separator_line );
}

/*
 * Report a socket's peer IP address in default style
 */
void *reporter_reportpeer( Connection_Info *stats, int ID ) {
    if ( ID > 0 ) {
        // copy the inet_ntop into temp buffers, to avoid overwriting
        char local_addr[ REPORT_ADDRLEN ];
        char remote_addr[ REPORT_ADDRLEN ];
        struct sockaddr *local = ((struct sockaddr*)&stats->local);
        struct sockaddr *peer = ((struct sockaddr*)&stats->peer);
    
        if ( local->sa_family == AF_INET ) {
            inet_ntop( AF_INET, &((struct sockaddr_in*)local)->sin_addr, 
                       local_addr, REPORT_ADDRLEN);
        }
#ifdef HAVE_IPV6
          else {
            inet_ntop( AF_INET6, &((struct sockaddr_in6*)local)->sin6_addr, 
                       local_addr, REPORT_ADDRLEN);
        }
#endif
    
        if ( peer->sa_family == AF_INET ) {
            inet_ntop( AF_INET, &((struct sockaddr_in*)peer)->sin_addr, 
                       remote_addr, REPORT_ADDRLEN);
        }
#ifdef HAVE_IPV6
          else {
            inet_ntop( AF_INET6, &((struct sockaddr_in6*)peer)->sin6_addr, 
                       remote_addr, REPORT_ADDRLEN);
        }
#endif
    
        fprintf(output_jni, report_peer,
                ID,
                local_addr,  ( local->sa_family == AF_INET ?
                              ntohs(((struct sockaddr_in*)local)->sin_port) :
         //__android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_peer,
         //                                                                       ID,
         //                                                                       local_addr,  ( local->sa_family == AF_INET ?
         //                                                                                    ntohs(((struct sockaddr_in*)local)->sin_port) :
#ifdef HAVE_IPV6
                              ntohs(((struct sockaddr_in6*)local)->sin6_port)),
#else
                              0),
#endif
                remote_addr, ( peer->sa_family == AF_INET ?
                              ntohs(((struct sockaddr_in*)peer)->sin_port) :
#ifdef HAVE_IPV6
                              ntohs(((struct sockaddr_in6*)peer)->sin6_port)));
#else
                              0));
#endif
    }
    return NULL;
}
// end ReportPeer

/* -------------------------------------------------------------------
 * Report the MSS and MTU, given the MSS (or a guess thereof)
 * ------------------------------------------------------------------- */

// compare the MSS against the (MTU - 40) to (MTU - 80) bytes.
// 40 byte IP header and somewhat arbitrarily, 40 more bytes of IP options.

#define checkMSS_MTU( inMSS, inMTU ) (inMTU-40) >= inMSS  &&  inMSS >= (inMTU-80)

void reporter_reportMSS( int inMSS, thread_Settings *inSettings ) {
    if ( inMSS <= 0 ) {
        fprintf(output_jni, report_mss_unsupported, inSettings->mSock );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI", report_mss_unsupported, inSettings->mSock );
    } else {
        char* net;
        int mtu = 0;

        if ( checkMSS_MTU( inMSS, 1500 ) ) {
            net = "ethernet";
            mtu = 1500;
        } else if ( checkMSS_MTU( inMSS, 4352 ) ) {
            net = "FDDI";
            mtu = 4352;
        } else if ( checkMSS_MTU( inMSS, 9180 ) ) {
            net = "ATM";
            mtu = 9180;
        } else if ( checkMSS_MTU( inMSS, 65280 ) ) {
            net = "HIPPI";
            mtu = 65280;
        } else if ( checkMSS_MTU( inMSS, 576 ) ) {
            net = "minimum";
            mtu = 576;
            fprintf(output_jni, "%s", warn_no_pathmtu );
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", warn_no_pathmtu );
        } else {
            mtu = inMSS + 40;
            net = "unknown interface";
        }

        fprintf(output_jni, report_mss,
                inSettings->mSock, inMSS, mtu, net );
        __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_mss,
                                                                               inSettings->mSock, inMSS, mtu, net );
    }
}
// end ReportMSS

#ifdef __cplusplus
} /* end extern "C" */
#endif
