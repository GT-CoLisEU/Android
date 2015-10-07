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
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>

#include "./bwctl-1.5.4/bwctl/bwctl.h"
#include "./bwctl-1.5.4/bwctld/bwctld.h"

#include "./bwctl-1.5.4/iperf-2.0.5/src/iperf_wrapper.h"
#include "./bwctl-1.5.4/iperf-2.0.5/reset_globals.h"

#include "./bwctl-1.5.4/iperf-3.0.11/src/iperf_main.h"

#include "./bwctl-1.5.4/Log.h"

char *glob_var;

//FILE *output_jni;
char *tmp_dir;
char *conf_dir;

#ifndef MIN
#define MIN(a,b)    ((a<b)?a:b)
#endif
#ifndef MAX
#define MAX(a,b)    ((a>b)?a:b)
#endif

int cont = 0;

char* ConvertJString(JNIEnv* env, jstring str)
{
   //if ( !str ) LString();

   jsize len      = (*env)->GetStringUTFLength(env, str);
   char* strChars = (*env)->GetStringUTFChars(env, str, (jboolean *)0);

   return strChars;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

jstring
Java_br_com_rnp_measurements_Bwctld_BwctldTask_stringFromJNI( JNIEnv* env, jobject thiz, jstring commandp, jstring tmpdir, jstring confdir)
{
    char *command = ConvertJString( env, commandp );
    tmp_dir = ConvertJString( env, tmpdir );
    conf_dir = ConvertJString( env, confdir );


    char **commandf = str_split(command, ' ');
    if(commandf){
        int nargs = 0;
         while(commandf[nargs]){
            nargs++;
         }
         bwctldRun(nargs, commandf);
         free(commandf);
    }


    char* result = get_log();
    return (*env)->NewStringUTF(env, result);
}

jstring
Java_br_com_rnp_measurements_Bwctl_BwctlTask_stringFromJNI( JNIEnv* env, jobject thiz, jstring commandp, jstring tmpdir)
{
    //TO-DO: Tornar alocação dinamica
    glob_var = mmap(NULL, 8000, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    char *command = ConvertJString( env, commandp );
    tmp_dir = ConvertJString( env, tmpdir );

     __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "run: %s", command);

    char **commandf = str_split(command, ' ');
    if(commandf){
        int nargs = 0;
        while(commandf[nargs]){
            nargs++;
        }
        bwctlRun(nargs, commandf);
        free(commandf);
    }

    //char* result = get_log();
    //munmap(glob_var, sizeof *glob_var);
    return (*env)->NewStringUTF(env, glob_var);
}


jstring
Java_br_com_rnp_measurements_Iperf_Iperf2Task_stringFromJNI( JNIEnv* env, jobject thiz, jstring commandp, jstring tmpdir)
{

      //TO-DO: Tornar alocação dinamica
      glob_var = mmap(NULL, 8000, PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

      char *command = ConvertJString( env, commandp );
      tmp_dir = ConvertJString( env, tmpdir );

      __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "run: %s", command);

        char **commandf = str_split(command, ' ');
        if(commandf){
              reset_all();
              int nargs = 0;
              while(commandf[nargs]){
                  __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "[%d] = %s", nargs, commandf[nargs]);
                  nargs++;
              }
              iperf_wrapper(nargs, commandf);
              free(commandf);
        }

      //char* result = get_log();
      //munmap(glob_var, sizeof *glob_var);
      return (*env)->NewStringUTF(env, glob_var);
}

jstring
Java_br_com_rnp_measurements_Iperf_Iperf3Task_stringFromJNI( JNIEnv* env, jobject thiz, jstring commandp, jstring tmpdir)
{
    //TO-DO: Tornar alocação dinamica
    glob_var = mmap(NULL, 8000, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    char *command = ConvertJString( env, commandp );
    tmp_dir = ConvertJString( env, tmpdir );

     __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "run: %s", command);

     char **commandf = str_split(command, ' ');
     if(commandf){
         int nargs = 0;
         while(commandf[nargs]){
             nargs++;
         }
         run_iperf3(nargs, commandf);
         free(commandf);
     }

    //char* result = get_log();
    //munmap(glob_var, sizeof *glob_var);
    return (*env)->NewStringUTF(env, glob_var);
}