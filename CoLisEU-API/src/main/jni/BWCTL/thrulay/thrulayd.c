/*
 * thrulayd.c -- network throughput tester (the server part).
 * 
 * Written by Stanislav Shalunov, http://www.internet2.edu/~shalunov/
 *            Bernhard Lutzmann, belu@users.sf.net
 *            Federico Montesino Pouzols, fedemp@altern.org
 * 
 * @(#) $Id: thrulayd.c,v 1.3.2.17 2006/08/20 18:06:19 fedemp Exp $
 *
 * Copyright 2003, 2006, Internet2.
 * Legal conditions are in file LICENSE
 * (MD5 = ecfa50d1b0bfbb81b658c810d0476a52).
 */

/**
 * @file thrulayd.c
 *
 * @short thrulay server command line tool.
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "server.h"
#include "thrulayd.h"
#include "util.h"

static void
print_usage(void)
{
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "Usage: thrulayd [-h] [-V] [-v] [-a address ] [-w#] "
		"[-p#] [-d] [-j group]\n");
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "\t-h\t\tdisplay this help and exit\n");
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "\t-V\t\toutput version information and exit\n");
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "\t-v\t\tverbose reporting of results\n");
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "\t-a address\tadd address to list of allowed hosts "
			"(CIDR syntax)\n");
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "\t-w#\t\twindow, in bytes (default: %dB)\n",
		THRULAY_DEFAULT_WINDOW);
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "\t-p#\t\tserver port (default: %d)\n",
		THRULAY_DEFAULT_SERVER_TCP_PORT);
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "\t-d \t\tdebug (no daemon, log to stderr)\n");
	__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "\t-j group\tjoin a multicast group\n");
}

void //__attribute__((noreturn))
usage(void)
{
	print_usage();
	//exit(1);
}

static void __attribute__((noreturn))
version(void)
{
	printf(THRULAY_VERSION " (server part) release ");
	printf("Copyright (c) 2006, Internet2.\n");
	printf("See the source for copying conditions.\n");
	//exit(0);
}

//int
//main(int argc, char *argv[])
//{
//	int window = THRULAY_DEFAULT_WINDOW;	/* Window size, in bytes. */
//	int port = SERVER_TCP_PORT;	/* Server TCP port number. */
//	char *mcast_address = NULL;         /* Multicast group to join to. */
//	int argcorig = argc;
//	int log_type = LOGTYPE_SYSLOG;		/* default is syslog */
//	int reporting_verbosity = 0;
//	int ch, rc;
//
//	while ((ch = getopt(argc, argv, "hVva:w:p:dj:")) != -1) {
//		switch (ch) {
//		case 'h':
//			print_usage();
//			exit(0);
//		case 'V':
//			version();
//		case 'v':
//			reporting_verbosity = 1;
//			break;
//		case 'a':
//			if (acl_allow_add(optarg) == -1) {
//				__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "unable to add host to ACL "
//						"list\n");
//				usage();
//			}
//			break;
//		case 'w':
//			window = atoi(optarg);
//			if (window <= 0) {
//				__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "window must be "
//					"a positive integer (in bytes)\n");
//				usage();
//			}
//			break;
//		case 'p':
//			port = atoi(optarg);
//			if (port <= 0) {
//				__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "port must be "
//					"a positive integer\n");
//				usage();
//			}
//			break;
//		case 'd':
//			/* Activate debugging mode. */
//			log_type = LOGTYPE_STDERR;
//			break;
//		case 'j':
//#ifdef ENABLE_THRULAY_MULTICAST
//			mcast_address = strdup(optarg);
//#else
//			__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG","multicast is disabled, continuing\n");
//#endif
//			break;
//		default:
//			usage();
//		}
//	}
//	argc = argcorig;
//
//	argc -= optind;
//	argv += optind;
//
//	if (argc != 0)
//		usage();
//
//	rc = thrulay_server_init(log_type, reporting_verbosity);
//	if (rc < 0) {
//		__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "While initializing: ");
//		error(ERR_FATAL, thrulay_server_strerror(rc));
//	}
//
//	rc = thrulay_server_listen(port, window);
//	if (rc < 0) {
//		__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "Server listen failed: ");
//		error(ERR_FATAL, thrulay_server_strerror(rc));
//	}
//
//	/* Run thrulay server forever */
//	rc = thrulay_server_start(0, mcast_address);
//	if (rc < 0) {
//		__android_log_print(ANDROID_LOG_DEBUG, "JNI_ERROR_LOG", "Server failed: ");
//		error(ERR_FATAL, thrulay_server_strerror(rc));
//	}
//
//	free(mcast_address);
//	return 0;
//}
