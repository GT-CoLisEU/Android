/*
 * iperf, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <netinet/tcp.h>

#include "iperf.h"
#include "iperf_api.h"
#include "units.h"
#include "iperf_locale.h"
#include "net.h"
#include <android/log.h>

static int run(struct iperf_test *test);


/**************************************************************************/
int
run_iperf3(int argc, char **argv)
{
     __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "Starting");
    struct iperf_test *test;

    // XXX: Setting the process affinity requires root on most systems.
    //      Is this a feature we really need?
#ifdef TEST_PROC_AFFINITY
    __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "TEST_PROC_AFFINITY");
    /* didnt seem to work.... */
    /*
     * increasing the priority of the process to minimise packet generation
     * delay
     */
    int rc = setpriority(PRIO_PROCESS, 0, -15);

    if (rc < 0) {
        perror("setpriority:");
        fprintf(stderr, "setting priority to valid level\n");
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "setting priority to valid level");
        rc = setpriority(PRIO_PROCESS, 0, 0);
    }
    
    /* setting the affinity of the process  */
    cpu_set_t cpu_set;
    int affinity = -1;
    int ncores = 1;

    sched_getaffinity(0, sizeof(cpu_set_t), &cpu_set);
    if (errno){
        perror("couldn't get affinity:");
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "couldn't get affinity:");
    }
    if ((ncores = sysconf(_SC_NPROCESSORS_CONF)) <= 0){
        err("sysconf: couldn't get _SC_NPROCESSORS_CONF");
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "sysconf: couldn't get _SC_NPROCESSORS_CONF");
    }

    CPU_ZERO(&cpu_set);
    CPU_SET(affinity, &cpu_set);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) != 0){
        err("couldn't change CPU affinity");
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "couldn't change CPU affinity");
    }
#endif
     __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "iperf_new_test();");
    test = iperf_new_test();
    if (!test){
        iperf_errexit(NULL, "create new test error - %s", iperf_strerror(i_errno));
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "create new test error - %s", iperf_strerror(i_errno));
    }
    __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "sets defaults");
    iperf_defaults(test);	/* sets defaults */
    __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "DEFAUTS OK");

    if (iperf_parse_arguments(test, argc, argv) < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "PARSE ARGUMENTS");
        iperf_err(test, "parameter error - %s", iperf_strerror(i_errno));
        fprintf(stderr, "\n");
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "parameter error - %s", iperf_strerror(i_errno));
        usage_long();
        //exit(1);
    }

    if (run(test) < 0){
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "RUN TEST");
        iperf_errexit(test, "error - %s", iperf_strerror(i_errno));
        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "error - %s", iperf_strerror(i_errno));
    }
    __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", " iperf_free_test");
    iperf_free_test(test);

    return 0;
}

/**************************************************************************/
static int
run(struct iperf_test *test)
{
    int consecutive_errors;

    switch (test->role) {
        case 's':
	    if (test->daemon) {
		int rc = daemon(0, 0);
		if (rc < 0) {
		    i_errno = IEDAEMON;
		    iperf_errexit(test, "error - %s", iperf_strerror(i_errno));
		    __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "error - %s", iperf_strerror(i_errno));
		}
	    }
	    consecutive_errors = 0;
            for (;;) {
                if (iperf_run_server(test) < 0) {
		    iperf_err(test, "error - %s", iperf_strerror(i_errno));
		    __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "error - %s", iperf_strerror(i_errno));
                    fprintf(stderr, "\n");
		    ++consecutive_errors;
		    if (consecutive_errors >= 5) {
		        fprintf(stderr, "too many errors, exiting\n");
		        __android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "too many errors, exiting\n");
			break;
		    }
                } else
		    consecutive_errors = 0;
                iperf_reset_test(test);
                if (iperf_get_test_one_off(test))
                    break;
            }
            break;
        case 'c':
            if (iperf_run_client(test) < 0){
		iperf_errexit(test, "error - %s", iperf_strerror(i_errno));
		__android_log_print(ANDROID_LOG_DEBUG, "IPERF3_LOG", "error - %s", iperf_strerror(i_errno));
		}
            break;
        default:
            iusage();
            break;
    }

    return 0;
}
