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
 * main.cpp
 * by Mark Gates <mgates@nlanr.net>
 * &  Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * main does initialization and creates the various objects that will
 * actually run the iperf program, then waits in the Joinall().
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <string.h>
 *
 *   <signal.h>
 * ------------------------------------------------------------------- */

#define HEADERS()
#include <android/log.h>
#include "../include/headers.h"

#include "../include/Settings.hpp"
#include "../include/PerfSocket.hpp"
#include "../include/Locale.h"
#include "../include/Condition.h"
#include "../include/Timestamp.hpp"
#include "../include/Listener.hpp"
#include "../include/List.h"
#include "../include/util.h"

#ifdef WIN32
#include "../include/service.h"
#endif

/* -------------------------------------------------------------------
 * prototypes
 * ------------------------------------------------------------------- */
// Function called at exit to clean up as much as possible
void cleanup( void );

/* -------------------------------------------------------------------
 * global variables
 * ------------------------------------------------------------------- */
extern "C" {
    // Global flag to signal a user interrupt
    int sInterupted = 0;
    // Global ID that we increment to be used 
    // as identifier for SUM reports
    int groupID = 0;
    // Mutex to protect access to the above ID
    Mutex groupCond;
    // Condition used to signify advances of the current
    // records being accessed in a report and also to
    // serialize modification of the report list
    Condition ReportCond;
    Condition ReportDoneCond;
}

// global variables only accessed within this file

// Thread that received the SIGTERM or SIGINT signal
// Used to ensure that if multiple threads receive the
// signal we do not prematurely exit
nthread_t sThread;
// The main thread uses this function to wait 
// for all other threads to complete
void waitUntilQuit( void );

/* -------------------------------------------------------------------
 * main()
 *      Entry point into Iperf
 *
 * sets up signal handlers
 * initialize global locks and conditions
 * parses settings from environment and command line
 * starts up server or client thread
 * waits for all threads to complete
 * ------------------------------------------------------------------- */
void run_iperf( int argc, char **argv ) {
    if(argc>9){
        cleanup();
    }
    // Set SIGTERM and SIGINT to call our user interrupt function
    my_signal( SIGTERM, Sig_Interupt );
    my_signal( SIGINT,  Sig_Interupt );
    my_signal( SIGALRM,  Sig_Interupt );

#ifndef WIN32
    // Ignore broken pipes
    __android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI", "WIN32 System");
    signal(SIGPIPE,SIG_IGN);
#else
    // Start winsock
    __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_JNI", "UNISX System");
    WSADATA wsaData;
    int rc = WSAStartup( 0x202, &wsaData );
    WARN_errno( rc == SOCKET_ERROR, "WSAStartup" );
	if (rc == SOCKET_ERROR)
		return 0;

     __android_log_print(ANDROID_LOG_DEBUG, "LOG_IPERF_JNI", "Tell windows we want to handle our own signals");
    // Tell windows we want to handle our own signals
    SetConsoleCtrlHandler( sig_dispatcher, true );
#endif

    __android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Initialize global mutexes and conditions");
    // Initialize global mutexes and conditions
    Condition_Initialize ( &ReportCond );
    Condition_Initialize ( &ReportDoneCond );
    Mutex_Initialize( &groupCond );
    Mutex_Initialize( &clients_mutex );

  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Initialize the thread subsystem");
    // Initialize the thread subsystem
    thread_init( );

  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Initialize the interrupt handling thread to 0");
    // Initialize the interrupt handling thread to 0
    sThread = thread_zeroid();

  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "perform any cleanup when quitting Iperf");
    // perform any cleanup when quitting Iperf
    atexit( cleanup );

  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Allocate the \"global\" settings");
    // Allocate the "global" settings
    thread_Settings* ext_gSettings = new thread_Settings;

  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Initialize settings to defaults");
    // Initialize settings to defaults
    Settings_Initialize( ext_gSettings );
  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "read settings from environment variables");
    // read settings from environment variables
    Settings_ParseEnvironment( ext_gSettings );
  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "read settings from command-line parameters");
    // read settings from command-line parameters
    Settings_ParseCommandLine( argc, argv, ext_gSettings );

  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Check for either having specified client or server");
    // Check for either having specified client or server
    if ( ext_gSettings->mThreadMode == kMode_Client 
         || ext_gSettings->mThreadMode == kMode_Listener ) {
#ifdef WIN32
      //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI", "Start the server as a daemon Daemon mode for non-windows in handled in the listener_spawn function");
        // Start the server as a daemon
        // Daemon mode for non-windows in handled
        // in the listener_spawn function
        if ( isDaemon( ext_gSettings ) ) {
            CmdInstallService(argc, argv);
            return 0;
        }
      //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI", "Remove the Windows service if requested");
        // Remove the Windows service if requested
        if ( isRemoveService( ext_gSettings ) ) {
          //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI", "remove the service");
            // remove the service
            if ( CmdRemoveService() ) {
                fprintf(stderr, "IPerf Service is removed.\n");
               //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI","IPerf Service is removed.\n");
                return 0;
            }
        }
#endif
      //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "initialize client(s)");
        // initialize client(s)
        if ( ext_gSettings->mThreadMode == kMode_Client ) {
            client_init( ext_gSettings );
        }

#ifdef HAVE_THREAD
      //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "HAVE THREAD - start up the reporter and client(s) or listener");
        // start up the reporter and client(s) or listener
        {
            thread_Settings *into = NULL;
          //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Create the settings structure for the reporter thread");
            // Create the settings structure for the reporter thread
            Settings_Copy( ext_gSettings, &into );
            into->mThreadMode = kMode_Reporter;

          //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Have the reporter launch the client or listener");
            // Have the reporter launch the client or listener
            into->runNow = ext_gSettings;

          //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "Start all the threads that are ready to go");
            // Start all the threads that are ready to go
            thread_start( into );
        }
#else
      //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "No need to make a reporter thread because we don't have threads");
        // No need to make a reporter thread because we don't have threads
        thread_start( ext_gSettings );
#endif
    } else {
      //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "neither server nor client mode was specified print usage and exit");
        // neither server nor client mode was specified
        // print usage and exit

#ifdef WIN32
      //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI", "In Win32 we also attempt to start a previously defined service. Starting in 2.0 to restart a previously defined service you must call iperf with \"iperf -D\" or using the environment variable.");
        // In Win32 we also attempt to start a previously defined service
        // Starting in 2.0 to restart a previously defined service
        // you must call iperf with "iperf -D" or using the environment variable
        SERVICE_TABLE_ENTRY dispatchTable[] =
        {
            { TEXT(SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main},
            { NULL, NULL}
        };

      //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI", "Only attempt to start the service if "-D" was specified");
        // Only attempt to start the service if "-D" was specified
        if ( !isDaemon(ext_gSettings) ||
           //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI", "starting the service by SCM, there is no arguments will be passed in. the arguments will pass into Service_Main entry."
             // starting the service by SCM, there is no arguments will be passed in.
             // the arguments will pass into Service_Main entry.
             !StartServiceCtrlDispatcher(dispatchTable) )
            // If the service failed to start then print usage
          //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_WIN32_JNI", "If the service failed to start then print usage");
#endif
        fprintf( stderr, usage_short_i2, argv[0], argv[0] );
       //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI",usage_short_i2, argv[0], argv[0] );
         //pthread_exit(0);
        //return 0;
    }
  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "wait for other (client, server) threads to complete");
    // wait for other (client, server) threads to complete
    thread_joinall();

  //__android_log_print(ANDROID_LOG_DEBUG, "DEBUG_IPERF_JNI", "all done!");
    // all done!
    //pthread_exit(0);
    //return 0;

} // end main

/* -------------------------------------------------------------------
 * Signal handler sets the sInterupted flag, so the object can
 * respond appropriately.. [static]
 * ------------------------------------------------------------------- */

void Sig_Interupt( int inSigno ) {
#ifdef HAVE_THREAD
    // We try to not allow a single interrupt handled by multiple threads
    // to completely kill the app so we save off the first thread ID
    // then that is the only thread that can supply the next interrupt
    if ( thread_equalid( sThread, thread_zeroid() ) ) {
        sThread = thread_getid();
    } else if ( thread_equalid( sThread, thread_getid() ) ) {
        sig_exit( inSigno );
    }

    // global variable used by threads to see if they were interrupted
    sInterupted = 1;

    // with threads, stop waiting for non-terminating threads
    // (ie Listener Thread)
    thread_release_nonterm( 1 );

#else
    // without threads, just exit quietly, same as sig_exit()
    sigf_exit( inSigno );
#endif
}

/* -------------------------------------------------------------------
 * Any necesary cleanup before Iperf quits. Called at program exit,
 * either by exit() or terminating main().
 * ------------------------------------------------------------------- */

void cleanup( void ) {
#ifdef WIN32
    // Shutdown Winsock
    WSACleanup();
#endif
    // clean up the list of clients
    Iperf_destroy ( &clients );

    // shutdown the thread subsystem
    thread_destroy( );
} // end cleanup






