/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <android/log.h>
#include <jni.h>
#include <pthread.h>
//#include "BWCTL/bwctl.h"
//#include "BWCTL/SocketJNIClient.h"
#include "./Iperf-2.0.5/src/iperf_wrapper.h"
#include "./reset_globals.h"
#include <stdio.h>
FILE *output_jni;

#define TCP_DOWNLINK  0
#define TCP_UPLINK  1
#define UDP_DOWNLINK  2
#define UDP_UPLINK  3

int cont = 0;

const char* ConvertJString(JNIEnv* env, jstring str)
{
   //if ( !str ) LString();

   const jsize len      = (*env)->GetStringUTFLength(env, str);
   const char* strChars = (*env)->GetStringUTFChars(env, str, (jboolean *)0);

   return strChars;
}


/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
jstring
Java_br_com_rnp_measurements_Iperf_IperfTask_stringFromJNI( JNIEnv* env, jobject thiz, jstring ipaddr, jstring serveripaddr, jstring serverport, jint testtype, jstring tmpdir)
{

    char* tmpdirpath = ConvertJString( env, tmpdir );
    char* tmpipaddr = ConvertJString( env, ipaddr );

    __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "Temp dir = %s, Android IP: %s", tmpdirpath, tmpipaddr);

//    char **command = (char *[]){"bwctl", "-T", "iperf", "-c", "200.130.99.55", "-a 1"};
//    __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "Starting BWCTL");
//    bwctlRun(tmpipaddr, tmpdirpath, 6, command);


      char *str_file = "/output.txt";
      char *str_path = (char *) malloc(1 + strlen(tmpdirpath)+ strlen(str_file) );
      strcpy(str_path, tmpdirpath);
      strcat(str_path, str_file);

        __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "%s", str_path );
      if(!(output_jni = fopen(str_path,"w+"))){
           __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "Unable to create temporary file");
      }


     //char **command = (char *[]){"iperf", "-u", "-t", "21", "-i", "1", "-c", "200.130.99.55", "-p", "12001"};
     __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "Starting Iperf");
     char **command = (char *[]){"iperf", "-t", "21", "-i", "1", "-c", "200.130.99.55", "-p", "12001"};
     iperf_wrapper(9, command);

    fclose(output_jni);
    return (*env)->NewStringUTF(env, "Finish...");

}




//    char **command = (char *[]){"bwctl", "-T", "iperf", "-c", "200.130.99.55", "-a 1"};
//    __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "Starting BWCTL");
//    bwctlRun(tmpipaddr, tmpdirpath, 6, command);
