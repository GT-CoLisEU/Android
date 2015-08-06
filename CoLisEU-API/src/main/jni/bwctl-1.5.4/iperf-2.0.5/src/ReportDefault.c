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

//extern char log_iperf;
//extern FILE *output_jni;
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
            //fprintf(output_jni,"%s", report_bw_header_i2);
            char output_text[256];
            sprintf(output_text, "%s", report_bw_header_i2);
            log_to_file(output_text);
            //int n = strlen(log_iperf);
            //log_iperf = realloc(n + strlen(output_text)+1)
            //log_iperf[n+1] = '\n';
            //strcat(log_iperf, output_text);

            printf("%s", report_bw_header_i2);
            __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", report_bw_header_i2);
            header_printed = 1;
        }
        printf( report_bw_format_i2, stats->transferID,
                        stats->startTime, stats->endTime,
                        buffer, &buffer[sizeof(buffer)/2] );
            char output_text[256];
            sprintf(output_text, report_bw_format_i2, stats->transferID,
                                                         stats->startTime, stats->endTime,
                                                         buffer, &buffer[sizeof(buffer)/2] );
            log_to_file(output_text);
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_bw_format_i2, stats->transferID,
                                                                                stats->startTime, stats->endTime,
                                                                                buffer, &buffer[sizeof(buffer)/2] );
    } else {
        // UDP Reporting
        if( !header_printed ) {
            char output_text[256];
            sprintf(output_text, "%s", report_bw_jitter_loss_header_i2);
            log_to_file(output_text);
             printf("%s", report_bw_jitter_loss_header_i2);
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", report_bw_jitter_loss_header_i2);
            header_printed = 1;
        }
        char output_text[256];
        sprintf(output_text, report_bw_jitter_loss_format_i2, stats->transferID,
                                                     stats->startTime, stats->endTime,
                                                     buffer, &buffer[sizeof(buffer)/2],
                                                     stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                                                     (100.0 * stats->cntError) / stats->cntDatagrams );
        log_to_file(output_text);
        printf( report_bw_jitter_loss_format_i2, stats->transferID,
                        stats->startTime, stats->endTime,
                        buffer, &buffer[sizeof(buffer)/2],
                        stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                        (100.0 * stats->cntError) / stats->cntDatagrams );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_bw_jitter_loss_format_i2, stats->transferID,
                                                                                stats->startTime, stats->endTime,
                                                                                buffer, &buffer[sizeof(buffer)/2],
                                                                                stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                                                                                (100.0 * stats->cntError) / stats->cntDatagrams );
        if ( stats->cntOutofOrder > 0 ) {
            char output_text[256];
            sprintf(output_text, report_outoforder_i2,
                                                                 stats->transferID, stats->startTime,
                                                                 stats->endTime, stats->cntOutofOrder );
            log_to_file(output_text);
            printf(report_outoforder_i2,
                                stats->transferID, stats->startTime,
                                stats->endTime, stats->cntOutofOrder );
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_outoforder_i2,
                                                                                        stats->transferID, stats->startTime,
                                                                                        stats->endTime, stats->cntOutofOrder );
        }
    }
    if ( stats->free == 1 && stats->mUDP == (char)kMode_Client ) {
        char output_text[256];
        sprintf(output_text, report_datagrams_i2, stats->transferID, stats->cntDatagrams );
        log_to_file(output_text);
        printf(report_datagrams_i2, stats->transferID, stats->cntDatagrams );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_datagrams_i2, stats->transferID, stats->cntDatagrams );
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
        char output_text[256];
                    sprintf(output_text, report_sum_bw_format_i2,
                                                                         stats->startTime, stats->endTime,
                                                                         buffer, &buffer[sizeof(buffer)/2] );
                    log_to_file(output_text);
                printf(report_sum_bw_format_i2,
                                stats->startTime, stats->endTime,
                                buffer, &buffer[sizeof(buffer)/2] );

         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_sum_bw_format_i2,
                                                                                stats->startTime, stats->endTime,
                                                                                buffer, &buffer[sizeof(buffer)/2] );
    } else {
        // UDP Reporting
        char output_text[256];
                    sprintf(output_text, report_sum_bw_jitter_loss_format_i2,
                                                                         stats->startTime, stats->endTime,
                                                                         buffer, &buffer[sizeof(buffer)/2],
                                                                         stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                                                                         (100.0 * stats->cntError) / stats->cntDatagrams );
                    log_to_file(output_text);
                printf(report_sum_bw_jitter_loss_format_i2,
                                stats->startTime, stats->endTime,
                                buffer, &buffer[sizeof(buffer)/2],
                                stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                                (100.0 * stats->cntError) / stats->cntDatagrams );

         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_sum_bw_jitter_loss_format_i2,
                                                                                stats->startTime, stats->endTime,
                                                                                buffer, &buffer[sizeof(buffer)/2],
                                                                                stats->jitter*1000.0, stats->cntError, stats->cntDatagrams,
                                                                                (100.0 * stats->cntError) / stats->cntDatagrams );
        if ( stats->cntOutofOrder > 0 ) {
            char output_text[256];
                        sprintf(output_text, report_sum_outoforder_i2,
                                                                                     stats->startTime,
                                                                                     stats->endTime, stats->cntOutofOrder );

                        log_to_file(output_text);
                    printf(report_sum_outoforder_i2,
                                        stats->startTime,
                                        stats->endTime, stats->cntOutofOrder );

             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_sum_outoforder_i2,
                                                                                        stats->startTime,
                                                                                        stats->endTime, stats->cntOutofOrder );
        }
    }
    if ( stats->free == 1 && stats->mUDP == (char)kMode_Client ) {
        char output_text[256];
                    sprintf(output_text, report_sum_datagrams_i2, stats->cntDatagrams );
                    log_to_file(output_text);
        printf(report_sum_datagrams_i2, stats->cntDatagrams );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_sum_datagrams_i2, stats->cntDatagrams );
    }
}

/*
 * Prints server transfer reports in default style
 */
void reporter_serverstats(Connection_Info *nused, Transfer_Info *stats ) {
    char output_text[256];
                sprintf(output_text, server_reporting_i2, stats->transferID );
                log_to_file(output_text);
    printf(server_reporting_i2, stats->transferID );
     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",server_reporting_i2, stats->transferID );
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

    char output_text[256];
                sprintf(output_text, "%s", separator_line_i2 );
                log_to_file(output_text);
    printf("%s", separator_line_i2 );
     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", separator_line_i2 );
    if ( data->mThreadMode == kMode_Listener ) {
        char output_text[256];
                    sprintf(output_text, server_port_i2,
                                                                         (isUDP( data ) ? "UDP" : "TCP"),
                                                                         data->mPort );
                    log_to_file(output_text);
                printf(server_port_i2,
                                (isUDP( data ) ? "UDP" : "TCP"),
                                data->mPort );

         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",server_port_i2,
                                                                                (isUDP( data ) ? "UDP" : "TCP"),
                                                                                data->mPort );
    } else {
        char output_text[256];
                    sprintf(output_text, client_port_i2,
                                                                         data->mHost,
                                                                         (isUDP( data ) ? "UDP" : "TCP"),
                                                                         data->mPort );

                    log_to_file(output_text);
                printf(client_port_i2,
                                data->mHost,
                                (isUDP( data ) ? "UDP" : "TCP"),
                                data->mPort );

         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI", client_port_i2,
                                                                                data->mHost,
                                                                                (isUDP( data ) ? "UDP" : "TCP"),
                                                                                data->mPort );
    }
    if ( data->mLocalhost != NULL ) {
        char output_text[256];
                    sprintf(output_text, bind_address_i2, data->mLocalhost );
                    log_to_file(output_text);
        printf(bind_address_i2, data->mLocalhost );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",bind_address_i2, data->mLocalhost );
        if ( SockAddr_isMulticast( &data->connection.local ) ) {
            char output_text[256];
                        sprintf(output_text, join_multicast_i2, data->mLocalhost );
                        log_to_file(output_text);
            printf(join_multicast_i2, data->mLocalhost );
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",join_multicast_i2, data->mLocalhost );
        }
    }

    if ( isUDP( data ) ) {
        char output_text[256];
                    sprintf(output_text, (data->mThreadMode == kMode_Listener ?
                                                                                            server_datagram_size_i2 : client_datagram_size_i2),
                                                                         data->mBufLen );
                    log_to_file(output_text);
                printf((data->mThreadMode == kMode_Listener ?
                                                   server_datagram_size_i2 : client_datagram_size_i2),
                                data->mBufLen );

         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",(data->mThreadMode == kMode_Listener ?
                                                                                                   server_datagram_size_i2 : client_datagram_size_i2),
                                                                                data->mBufLen );
        if ( SockAddr_isMulticast( &data->connection.peer ) ) {
            char output_text[256];
                        sprintf(output_text, multicast_ttl_i2, data->info.mTTL);
                        log_to_file(output_text);
            printf(multicast_ttl_i2, data->info.mTTL);
            __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",multicast_ttl_i2, data->info.mTTL);
        }
    }
    byte_snprintf( buffer, sizeof(buffer), win,
                   toupper( data->info.mFormat));
    char output_text1[256];
    sprintf(output_text1, "%s: %s", (isUDP( data ) ?
                                                                     udp_buffer_size_i2 : tcp_window_size_i2), buffer );
                log_to_file(output_text1);
    printf("%s: %s", (isUDP( data ) ?
                                udp_buffer_size_i2 : tcp_window_size_i2), buffer );
                                printf("%s: %s", (isUDP( data ) ?
                                                                udp_buffer_size_i2 : tcp_window_size_i2), buffer );

     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s: %s", (isUDP( data ) ?
                                                                                            udp_buffer_size_i2 : tcp_window_size_i2), buffer );

    if ( win_requested == 0 ) {
        char output_text[256];
                    sprintf(output_text, "%s", window_default_i2 );
                    log_to_file(output_text);
        printf("%s", window_default_i2 );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI"," %s", window_default_i2 );
    } else if ( win != win_requested ) {
        byte_snprintf( buffer, sizeof(buffer), win_requested,
                       toupper( data->info.mFormat));
        char output_text[256];
                    sprintf(output_text, warn_window_requested_i2, buffer );
                    log_to_file(output_text);
        printf(warn_window_requested_i2, buffer );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",warn_window_requested_i2, buffer );
    }
    char output_text2[256];
                sprintf(output_text2, "\n" );
                log_to_file(output_text2);
    printf("\n" );
    // __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","\n" );
    char output_text3[256];
                sprintf(output_text3, "%s", separator_line_i2 );
                log_to_file(output_text3);
    printf("%s", separator_line_i2 );
     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", separator_line_i2 );
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
    
        printf(report_peer_i2,
                ID,
                local_addr,  ( local->sa_family == AF_INET ?
                              ntohs(((struct sockaddr_in*)local)->sin_port) :
         //__android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_peer_i2,
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
        char output_text[256];
                    sprintf(output_text, report_mss_unsupported_i2, inSettings->mSock );
                    log_to_file(output_text);
        printf(report_mss_unsupported_i2, inSettings->mSock );
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI", report_mss_unsupported_i2, inSettings->mSock );
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
            char output_text[256];
                        sprintf(output_text, "%s", warn_no_pathmtu_i2 );
                        log_to_file(output_text);
            printf("%s", warn_no_pathmtu_i2 );
             __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI","%s", warn_no_pathmtu_i2 );
        } else {
            mtu = inMSS + 40;
            net = "unknown interface";
        }

        char output_text[256];
                    sprintf(output_text, report_mss_i2, inSettings->mSock, inMSS, mtu, net );
                    log_to_file(output_text);
                printf(report_mss_i2,
                                inSettings->mSock, inMSS, mtu, net );

        __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_REPORTDEF_JNI",report_mss_i2,
                                                                               inSettings->mSock, inMSS, mtu, net );
    }
}
// end ReportMSS

#ifdef __cplusplus
} /* end extern "C" */
#endif
