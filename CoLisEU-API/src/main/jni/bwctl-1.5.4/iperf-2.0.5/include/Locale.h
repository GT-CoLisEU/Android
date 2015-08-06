/*--------------------------------------------------------------- 
 * Copyright (c) 1999,2000,2001,2002,2003                              
 * The Board of Trustees of the University of Illinois            
 * All Rights Reserved.                                           
 *--------------------------------------------------------------- 
 * Permission is hereby granted, free of char*ge, to any person    
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
 * Locale.h
 * by Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * & Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * Strings and other stuff that is locale specific.
 * ------------------------------------------------------------------- */

#ifndef LOCALE_H
#define LOCALE_H

#ifdef __cplusplus
extern "C" {
#endif
/* -------------------------------------------------------------------
 * usage
 * ------------------------------------------------------------------- */

extern const char* usage_short_i2;

extern const char* usage_long1_i2;
extern const char* usage_long2_i2;

extern const char* version;

/* -------------------------------------------------------------------
 * settings
 * ------------------------------------------------------------------- */

extern const char* separator_line_i2;

extern const char* server_port_i2;

extern const char* client_port_i2;

extern const char* bind_address_i2;

extern const char* multicast_ttl_i2;

extern const char* join_multicast_i2;

extern const char* client_datagram_size_i2;

extern const char* server_datagram_size_i2;

extern const char* tcp_window_size_i2;

extern const char* udp_buffer_size_i2;

extern const char* window_default_i2;

extern const char* wait_server_threads_i2;

/* -------------------------------------------------------------------
 * reports
 * ------------------------------------------------------------------- */

extern const char** report_read_lengths_i2;

extern const char* report_read_length_times_i2;

extern const char* report_bw_header_i2;

extern const char* report_bw_format_i2;

extern const char* report_sum_bw_format_i2;

extern const char* report_bw_jitter_loss_header_i2;

extern const char* report_bw_jitter_loss_format_i2;

extern const char* report_sum_bw_jitter_loss_format_i2;

extern const char* report_outoforder_i2;

extern const char* report_sum_outoforder_i2;

extern const char* report_peer_i2;

extern const char* report_mss_unsupported_i2;

extern const char* report_mss_i2;

extern const char* report_datagrams_i2;

extern const char* report_sum_datagrams_i2;

extern const char* server_reporting_i2;

extern const char* reportCSV_peer_i2;

extern const char* reportCSV_bw_format_i2;

extern const char* reportCSV_bw_jitter_loss_format_i2;

/* -------------------------------------------------------------------
 * warnings
 * ------------------------------------------------------------------- */

extern const char* warn_window_requested_i2;

extern const char* warn_window_small_i2;

extern const char* warn_delay_large_i2;

extern const char* warn_no_pathmtu_i2;

extern const char* warn_no_ack_i2;

extern const char* warn_ack_failed_i2;

extern const char* warn_fileopen_failed_i2;

extern const char* unable_to_change_win_i2;

extern const char* opt_estimate_i2;

extern const char* report_interval_small_i2;

extern const char* warn_invalid_server_option_i2;

extern const char* warn_invalid_client_option_i2;

extern const char* warn_invalid_compatibility_option_i2;

extern const char* warn_implied_udp_i2;

extern const char* warn_implied_compatibility_i2;

extern const char* warn_buffer_too_small_i2;

extern const char* warn_invalid_single_threaded_i2;

extern const char* warn_invalid_report_style_i2;

extern const char* warn_invalid_report_i2;

#ifdef __cplusplus
} /* end extern "C" */
#endif
#endif // LOCALE_H










