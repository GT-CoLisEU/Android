/*
 * ex: set tabstop=4 ai expandtab softtabstop=4 shiftwidth=4:
 *      $Id$
 */
/************************************************************************
 *                                                                      *
 *                          Copyright (C)  2003                         *
 *                              Internet2                               *
 *                          All Rights Reserved                         *
 *                                                                      *
 ************************************************************************/
/*
 *    File:            sapi.c
 *
 *    Author:            Jeff W. Boote
 *                    Internet2
 *
 *    Date:            Tue Sep 16 14:27:01 MDT 2003
 *
 *    Description:    
 *
 *    This file contains the api functions typically called from an
 *    bwlib server application.
 *
 *    License:
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../bwlib/config.h"
#include "../bwlib/bwlibP.h"

static int
OpenSocket(
        BWLContext  ctx    __attribute__((unused)),
        int         family,
        I2Addr      addr
        )
{
    struct addrinfo *fai;
    struct addrinfo *ai;
    int             on;
    int             fd = -1;

    if(!(fai = I2AddrAddrInfo(addr,NULL,BWL_CONTROL_SERVICE_NAME))){
        return -2;
    }

    for(ai = fai;ai;ai = ai->ai_next){
        if(ai->ai_family != family)
            continue;

        fd =socket(ai->ai_family,ai->ai_socktype,ai->ai_protocol);

        if(fd < 0)
            continue;

        on=1;
        if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,
                    sizeof(on)) != 0){
            goto failsock;
        }

        /*
         * TODO Check for the superseded IPV6_BINDV6ONLY sockopt too?
         * (No - not unless someone complains.)
         */
#if    defined(AF_INET6) && defined(IPPROTO_IPV6) && defined(IPV6_V6ONLY)
        on=0;
        if((ai->ai_family == AF_INET6) &&
                setsockopt(fd,IPPROTO_IPV6,IPV6_V6ONLY,&on,
                    sizeof(on)) != 0){
            goto failsock;
        }
#endif

        if(bind(fd,ai->ai_addr,ai->ai_addrlen) == 0){

            I2AddrSetFD(addr,-1,True);
            I2AddrSetSocktype(addr,ai->ai_socktype);
            I2AddrSetSAddr(addr,ai->ai_addr,ai->ai_addrlen);
            I2AddrSetFD(addr,fd,True);

            break;
        }

        if(errno == EADDRINUSE)
            return -2;

failsock:
        while((close(fd) < 0) && (errno == EINTR));
        fd = -1;
    }


    return fd;
}

/*
 * Function:    BWLServerSockCreate
 *
 * Description:    
 *         Used by server to create the initial listening socket.
 *         (It is not required that the server use this interface,
 *         but it will be kept up-to-date and in sync with the
 *         client BWLControlOpen function. For example, both of
 *         these functions currently give priority to IPV6 addresses
 *         over IPV4.)
 *
 *         The addr should be NULL for a wildcard socket, or bound to
 *         a specific interface using I2AddrByNode or I2AddrByAddrInfo.
 *
 *         This function will create the socket, bind it, and set the
 *         "listen" backlog length.
 *
 *         If addr is set using I2AddrByFD, it will cause an error.
 *         (It doesn't really make much sense to call this function at
 *         all if you are going to    create and bind your own socket -
 *         the only thing left is to call "listen"...)
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    
 * Side Effect:    
 */
I2Addr
BWLServerSockCreate(
        BWLContext      ctx,
        I2Addr          addr,
        BWLErrSeverity  *err_ret
        )
{
    int fd = -1;

    *err_ret = BWLErrOK;

    /*
     * I2AddrByFD is invalid.
     */
    if(addr && (I2AddrFD(addr) > -1)){
        BWLError(ctx,BWLErrFATAL,BWLErrINVALID,
                "Invalid I2Addr record - fd already specified.");
        goto error;
    }

    /*
     * If no addr specified, then use wildcard address.
     */
    if((!addr) &&
            !(addr = I2AddrByWildcard(BWLContextErrHandle(ctx),SOCK_STREAM,
                    BWL_CONTROL_SERVICE_NAME))){
        goto error;
    }

     if(!I2AddrSetPassive(addr,True)){
        goto error;
     }

#ifdef    AF_INET6
    /*
     * First try IPv6 addrs only
     */
    fd = OpenSocket(ctx,AF_INET6,addr);

    /*
     * Fall back to IPv4 addrs if necessary.
     */
    if(fd == -1)
#endif
        fd = OpenSocket(ctx,AF_INET,addr);

    /*
     * if we failed to find any IPv6 or IPv4 addresses... punt.
     */
    if(fd < 0){
        BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,
                "BWLServerSockCreate:%M");
        goto error;
    }

    /*
     * We have a bound socket - set the listen backlog.
     */
    if(listen(fd,BWL_LISTEN_BACKLOG) < 0){
        BWLError(ctx,BWLErrFATAL,errno,"listen(%d,%d):%s",
                fd,BWL_LISTEN_BACKLOG,strerror(errno));
        goto error;
    }

    return addr;

error:
    I2AddrFree(addr);
    *err_ret = BWLErrFATAL;
    return NULL;

}

/*
 * Function:    BWLControlAccept
 *
 * Description:    
 *         This function is used to initialiize the communication
 *         to the peer.
 *           
 * In Args:    
 *         connfd,connsaddr, and connsaddrlen are all returned
 *         from "accept".
 *
 * Returns:    Valid BWLControl handle on success, NULL if
 *              the request has been rejected, or error has occurred.
 *              Return value does not distinguish between illegal
 *              requests, those rejected on policy reasons, or
 *              errors encountered by the server during execution.
 * 
 * Side Effect:
 */
BWLControl
BWLControlAccept(
        BWLContext          ctx,            /* library context              */
        int                 connfd,         /* connected socket             */
        struct sockaddr     *connsaddr,     /* connected socket addr        */
        socklen_t           connsaddrlen,   /* connected socket addr len    */
        uint32_t            mode_offered,   /* advertised server mode       */
        BWLNum64            uptime,         /* uptime for server            */
        int                 *retn_on_intr,  /* if *retn_on_intr return      */
        BWLErrSeverity      *err_ret        /* err - return                 */
        )
{
    BWLControl      cntrl;
    uint8_t         challenge[16];
    uint8_t         rawtoken[32];
    uint8_t         token[32];
    int             rc;
    int             ival=0;
    int             *intr = &ival;
    BWLTimeStamp    timestart,timeend;
    char            remotenode[NI_MAXHOST],remoteserv[NI_MAXSERV];
    size_t          remotenodelen = sizeof(remotenode);
    size_t          remoteservlen = sizeof(remoteserv);
    char            localnode[NI_MAXHOST],localserv[NI_MAXSERV];
    size_t          localnodelen = sizeof(localnode);
    size_t          localservlen = sizeof(localserv);
    uint32_t        access_prio = _BWL_DEFAULT_ACCESSPRIO;

    *err_ret = BWLErrFATAL;

    /*
     * Check for valid context.
     */
    if( !ctx->valid){
        BWLError(ctx,BWLErrFATAL,EINVAL,
                "BWLControlAccept(): Invalid context record");
        return NULL;
    }
    if(!ctx->tool_avail){
        BWLError(ctx,BWLErrFATAL,BWLErrINVALID,
                "BWLControlAccept: Context invalid, tools not initialized");
        return NULL;
    }

    (void)BWLContextConfigGetU32(ctx,BWLAccessPriority,&access_prio);

    if(connfd < 0){
        BWLError(ctx,BWLErrFATAL,BWLErrINVALID,
                "BWLControlAccept: Invalid socket fd");
        return NULL;
    }

    if(retn_on_intr){
        intr = retn_on_intr;
    }

    *err_ret = BWLErrOK;
    mode_offered &= BWL_MODE_ALLMODES;

    if( !(cntrl = _BWLControlAlloc(ctx,err_ret))){
        BWLError(ctx,BWLErrFATAL,errno,"_BWLControlAlloc(): %M");
        goto error;
    }

    cntrl->sockfd = connfd;
    cntrl->server = True;

    /*
     * set up remote_addr for policy decisions, and log reporting.
     *
     * If connsaddr is not existant, than create the Addr using the
     * socket only.
     */
    if(!connsaddr || !connsaddrlen){
        if(!(cntrl->remote_addr = I2AddrBySockFD(ctx,connfd,True))){
            BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,
                    "Unable to determine socket peername: %M");
            *err_ret = BWLErrFATAL;
            goto error;
        }
    }
    else{
        if( !(cntrl->remote_addr = I2AddrBySAddr(
                        BWLContextErrHandle(ctx),
                        connsaddr,connsaddrlen,SOCK_STREAM,0))){
            BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,
                    "Unable to determine socket peername: %M");
            *err_ret = BWLErrFATAL;
            goto error;
        }
        if( !I2AddrSetFD(cntrl->remote_addr,connfd,True)){
            BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,
                    "Unable to set socket fd: %M");
            *err_ret = BWLErrFATAL;
            goto error;
        }
    }

    /*
     * set up local_addr for policy decisions, and log reporting.
     */
    if( !(cntrl->local_addr = I2AddrByLocalSockFD(
                    BWLContextErrHandle(ctx),connfd,False))){
        BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,
                    "Unable to determine socketname: %M");
        *err_ret = BWLErrFATAL;
        goto error;
    }

    if( !I2AddrNodeName(cntrl->remote_addr,remotenode,&remotenodelen) ||
            !I2AddrServName(cntrl->remote_addr,remoteserv,&remoteservlen) ||
            !I2AddrNodeName(cntrl->local_addr,localnode,&localnodelen) ||
            !I2AddrServName(cntrl->local_addr,localserv,&localservlen)){
        BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,
                    "Unable to set service names: %M");
        goto error;
    }

    BWLError(ctx,access_prio,BWLErrPOLICY,
            "Connection to (%s:%s) from (%s:%s)",
            localnode,localserv,remotenode,remoteserv);

    /* generate 16 random bytes of challenge and save them away. */
    if(I2RandomBytes(ctx->rand_src,challenge, 16) != 0){
        BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,"I2RandomBytes(): %M");
        *err_ret = BWLErrFATAL;
        goto error;
    }

    if(!BWLGetTimeStamp(ctx,&timestart)){
        BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,"BWLGetTimeStamp(): %M");
        *err_ret = BWLErrFATAL;
        goto error;
    }
    if( (rc = _BWLWriteServerGreeting(cntrl,mode_offered,
                    challenge,intr)) < BWLErrOK){
        *err_ret = (BWLErrSeverity)rc;
        BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,
                "BWLControlAccept(): Unable to write ServerGreeting message");
        goto error;
    }

    /*
     * If no mode offered, immediately close socket after sending
     * server greeting. (Nice way of saying goodbye.)
     */
    if(!mode_offered){
        BWLError(cntrl->ctx,access_prio,BWLErrPOLICY,
                "Control request to (%s:%s) denied from (%s:%s): mode == 0",
                localnode,localserv,remotenode,remoteserv);
        goto error;
    }

    if((rc = _BWLReadClientGreeting(cntrl,&cntrl->mode,rawtoken,
                    cntrl->readIV,intr)) < BWLErrOK){
        *err_ret = (BWLErrSeverity)rc;
        BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,
                "BWLControlAccept(): Unable to read ClientGreeting message");
        goto error;
    }
    if(!BWLGetTimeStamp(ctx,&timeend)){
        BWLError(ctx,BWLErrFATAL,BWLErrUNKNOWN,"BWLGetTimeStamp(): %M");
        *err_ret = BWLErrFATAL;
        goto error;
    }
    cntrl->rtt_bound = BWLNum64Sub(timeend.tstamp,timestart.tstamp);

    /* insure that exactly one mode is chosen */
    if(    (cntrl->mode != BWL_MODE_OPEN) &&
            (cntrl->mode != BWL_MODE_AUTHENTICATED) &&
            (cntrl->mode != BWL_MODE_ENCRYPTED)){
        BWLError(ctx,BWLErrFATAL,BWLErrINVALID,
                "BWLControlAccept(): Invalid mode(%d) in request.",cntrl->mode);
        *err_ret = BWLErrFATAL;
        goto error;
    }

    if(!(cntrl->mode | mode_offered)){ /* can't provide requested mode */
        BWLError(cntrl->ctx,access_prio,BWLErrPOLICY,
                "Control request to (%s:%s) denied from (%s:%s): mode not offered (%u)",
                localnode,localserv,remotenode,remoteserv,cntrl->mode);
        if( (rc = _BWLWriteServerOK(cntrl,BWL_CNTRL_REJECT,(BWLNum64)0,
                        0,intr)) < BWLErrOK){
            *err_ret = (BWLErrSeverity)rc;
        }
        goto error;
    }

    if(cntrl->mode & (BWL_MODE_AUTHENTICATED|BWL_MODE_ENCRYPTED)){
        uint8_t     binKey[16];
        BWLBoolean  getkey_success;

        /* Fetch the encryption key into binKey */
        /*
         * go through the motions of decrypting token even if
         * getkey fails to find username to minimize vulnerability
         * to timing attacks.
         */
        getkey_success = _BWLCallGetAESKey(cntrl->ctx,cntrl->userid_buffer,
                binKey,err_ret);

        /* CallGetAESKey failed due to invalid program state */
        if(!getkey_success && (*err_ret != BWLErrOK)){
            (void)_BWLWriteServerOK(cntrl,BWL_CNTRL_FAILURE,(BWLNum64)0,0,intr);
            goto error;
        }

        if (_BWLDecryptToken(binKey,rawtoken,token) < 0){
            BWLError(cntrl->ctx,BWLErrFATAL,
                    BWLErrUNKNOWN,
                    "Encryption state problem?!?!");
            (void)_BWLWriteServerOK(cntrl,
                                    BWL_CNTRL_FAILURE,(BWLNum64)0,0,intr);
            *err_ret = BWLErrFATAL;
            goto error;
        }

        /* Decrypted challenge is in the first 16 bytes */
        if((memcmp(challenge,token,16) != 0) || !getkey_success){
            if(!getkey_success){
                BWLError(cntrl->ctx,access_prio,BWLErrPOLICY,
                        "Unknown userid (%s)",
                        cntrl->userid_buffer);
            }
            else{
                BWLError(cntrl->ctx,access_prio,BWLErrPOLICY,
                        "Control request to (%s:%s) denied from (%s:%s):Invalid challenge encryption",
                        localnode,localserv,remotenode,remoteserv);
            }
            (void)_BWLWriteServerOK(cntrl,BWL_CNTRL_REJECT,(BWLNum64)0,0,intr);
            goto error;
        }

        /* Authentication ok - set encryption fields */
        cntrl->userid = cntrl->userid_buffer;
        if(I2RandomBytes(cntrl->ctx->rand_src,cntrl->writeIV,16) != 0){
            BWLError(cntrl->ctx,BWLErrFATAL,BWLErrUNKNOWN,
                    "Unable to fetch randomness...");
            (void)_BWLWriteServerOK(cntrl,BWL_CNTRL_FAILURE,(BWLNum64)0,0,intr);
            goto error;
        }
        memcpy(cntrl->session_key,&token[16],16);
        _BWLMakeKey(cntrl,cntrl->session_key); 
    }

    if(!_BWLCallCheckControlPolicy(cntrl,cntrl->mode,cntrl->userid, 
                I2AddrSAddr(cntrl->local_addr,NULL),
                I2AddrSAddr(cntrl->remote_addr,NULL),err_ret)){
        if(*err_ret > BWLErrWARNING){
            BWLError(ctx,access_prio,BWLErrPOLICY,
                    "ControlSession request to (%s:%s) denied from userid(%s):(%s:%s)",
                    localnode,localserv,
                    (cntrl->userid)?cntrl->userid:"nil",
                    remotenode,remoteserv);
            /*
             * send mode of 0 to client, and then close.
             */
            (void)_BWLWriteServerOK(cntrl,BWL_CNTRL_REJECT,(BWLNum64)0,0,intr);
        }
        else{
            BWLError(ctx,*err_ret,BWLErrUNKNOWN,
                    "Policy function failed.");
            (void)_BWLWriteServerOK(cntrl,BWL_CNTRL_FAILURE,(BWLNum64)0,0,intr);
        }
        goto error;
    }

    /*
     * Made it through the gauntlet - accept the control session!
     */
    if( (rc = _BWLWriteServerOK(cntrl,BWL_CNTRL_ACCEPT,uptime,
				ctx->tool_avail,intr)) < BWLErrOK){
        *err_ret = (BWLErrSeverity)rc;
        goto error;
    }
    BWLError(ctx,access_prio,BWLErrPOLICY,
            "ControlSession([%s]:%s) accepted from userid(%s):([%s]:%s)",
            localnode,localserv,
            (cntrl->userid)?cntrl->userid:"nil",
            remotenode,remoteserv);

    return cntrl;

error:
    BWLControlClose(cntrl);
    return NULL;
}

BWLErrSeverity
BWLProcessTestRequest(
        BWLControl              cntrl,
        int                     *retn_on_intr
        )
{
    BWLTestSession  tsession = cntrl->tests;
    BWLErrSeverity  err_ret=BWLErrOK;
    int             rc;
    BWLAcceptType   acceptval = BWL_CNTRL_FAILURE;
    int             ival=0;
    int             *intr = &ival;
    uint32_t        access_prio = _BWL_DEFAULT_ACCESSPRIO;

    if(retn_on_intr){
        intr = retn_on_intr;
    }

    (void)BWLContextConfigGetU32(cntrl->ctx,BWLAccessPriority,&access_prio);

    /*
     * Read the TestRequest and use tsession to hold the information.
     * (during schedule negotiation, only the timestamps from the
     * subsequent requests are used.)
     */
    if((rc = _BWLReadTestRequest(cntrl,intr,&tsession,&acceptval)) !=
            BWLErrOK){
        switch(acceptval){
            case BWL_CNTRL_INVALID:
                return BWLErrFATAL;
            case BWL_CNTRL_FAILURE:
            case BWL_CNTRL_UNSUPPORTED:
                return BWLErrWARNING;
            default:
                /* NOTREACHED */
                abort();
        }
    }

    if(!BWLGetTimeStamp(cntrl->ctx,&tsession->localtime)){
        BWLError(cntrl->ctx,BWLErrFATAL,BWLErrUNKNOWN,
                "BWLGetTimeStamp(): %M");
        err_ret = BWLErrWARNING;
        acceptval = BWL_CNTRL_FAILURE;
        goto error;
    }

    /*
     * Initialize reservation time.
     */
    tsession->reserve_time = BWLULongToNum64(0);

    /*
     * Update of current reservation.
     */
    if(tsession == cntrl->tests){
        /*
         * If req_time is 0, client is cancelling.
         */
        if(tsession->test_spec.req_time.tstamp == 0){
            err_ret = BWLErrINFO;
            acceptval = BWL_CNTRL_REJECT;
            goto error;
        }
    }
    else{
        /*
         * If this "new" session is a receiver session, create a SID for it.
         */
        if(tsession->conf_server && (_BWLCreateSID(tsession) != 0)){
            err_ret = BWLErrWARNING;
            acceptval = BWL_CNTRL_FAILURE;
            goto error;
        }

        /*
         * Get definition record for selected tool
         * (This will fail if an unsupported tool is requested.)
         */
        if( !(tsession->tool = _BWLToolGetDefinition(cntrl->ctx,
                        tsession->test_spec.tool_id))){
            err_ret = BWLErrWARNING;
            acceptval = BWL_CNTRL_UNSUPPORTED;
            goto error;
        }
    }

    /*
     * compute "fuzz" time.
     */
    tsession->fuzz = BWLGetTimeStampError(&tsession->test_spec.req_time);
    tsession->fuzz = BWLNum64Add(tsession->fuzz,
            BWLGetTimeStampError(&tsession->localtime));
    /*
     * Add a constant to make up for the fact that iperf -u usually
     * run for some fraction of a second longer than the -t option
     * specifies.
     *
     * TODO: Make this constant configurable somehow?
     */
    tsession->fuzz = BWLNum64Add(tsession->fuzz,BWLDoubleToNum64(1.5));

    if(!_BWLCallCheckTestPolicy(cntrl,tsession,&err_ret)){
        if(err_ret < BWLErrOK)
            goto error;
        BWLError(cntrl->ctx,access_prio,BWLErrPOLICY,
                "Test not allowed");
        acceptval = BWL_CNTRL_REJECT;
        err_ret = BWLErrINFO;
        goto error;
    }

    /*
     * Validate the test after running CheckTestPolicy as CheckTestPolicy may
     * set or change test spec parameters.
     */
    if (!BWLToolValidateTest(cntrl->ctx,tsession->test_spec.tool_id, tsession->test_spec)) {
        err_ret = BWLErrWARNING;
        acceptval = BWL_CNTRL_UNSUPPORTED;
        goto error;
    }

    if( (rc = _BWLWriteTestAccept(cntrl,intr,BWL_CNTRL_ACCEPT,tsession))
            < BWLErrOK){
        err_ret = (BWLErrSeverity)rc;
        goto err2;
    }

    /*
     * Add tsession to list of tests managed by this control connection.
     */
    cntrl->tests = tsession;

    return BWLErrOK;

error:
    /*
     * If it is a non-fatal error, communication should continue, so
     * send negative accept.
     */
    if(err_ret >= BWLErrWARNING)
        (void)_BWLWriteTestAccept(cntrl,intr,acceptval,tsession);

err2:
    if(tsession)
        _BWLTestSessionFree(cntrl->ctx,tsession,BWL_CNTRL_FAILURE);

    return err_ret;
}

BWLErrSeverity
BWLProcessTimeRequest(
        BWLControl  cntrl,
        int         *retn_on_intr
        )
{
    int             rc;
    int             ival=0;
    int             *intr = &ival;
    BWLTimeStamp    tstamp;

    if(retn_on_intr){
        intr = retn_on_intr;
    }

    if( (rc = _BWLReadTimeRequest(cntrl,intr)) < BWLErrOK)
        return _BWLFailControlSession(cntrl,rc);

    if(!BWLGetTimeStamp(cntrl->ctx,&tstamp))
        return _BWLFailControlSession(cntrl,BWLErrFATAL);

    if( (rc = _BWLWriteTimeResponse(cntrl,&tstamp,intr)) < BWLErrOK)
        return _BWLFailControlSession(cntrl,rc);

    return BWLErrOK;
}

BWLErrSeverity
BWLProcessStartSession(
        BWLControl  cntrl,
        int         *retn_on_intr
        )
{
    int             rc;
    BWLErrSeverity  err=BWLErrOK;
    int             ival=0;
    int             *intr = &ival;
    uint16_t        peerport = 0;

    if(retn_on_intr){
        intr = retn_on_intr;
    }

    if( (rc = _BWLReadStartSession(cntrl,&peerport,intr)) < BWLErrOK)
        return _BWLFailControlSession(cntrl,rc);

    if(!_BWLEndpointStart(cntrl->tests,&peerport,&err)){
        (void)_BWLWriteStartAck(cntrl,intr,0,BWL_CNTRL_FAILURE);
        return _BWLFailControlSession(cntrl,(int)err);
    }

    if( (rc = _BWLWriteStartAck(cntrl,intr,peerport,BWL_CNTRL_ACCEPT))
            < BWLErrOK)
        return _BWLFailControlSession(cntrl,rc);


    return BWLErrOK;
}

/*
 * Function:    BWLSessionStatus
 *
 * Description:    
 *     This function returns the "status" of the test session identified
 *     by the sid. "send" indicates which "side" of the test to retrieve
 *     information about.
 *
 * In Args:    
 *
 * Out Args:    
 *
 * Scope:    
 * Returns:    True if status was available, False otherwise.
 *         aval contains the actual "status":
 *             <0    Test is not yet complete
 *             >=0    Valid BWLAcceptType - see enum for meaning.
 * Side Effect:    
 */
BWLBoolean
BWLSessionStatus(
        BWLControl      cntrl,
        BWLSID          sid,
        BWLAcceptType   *aval
        )
{
    BWLTestSession  tsession;
    BWLErrSeverity  err;

    /*
     * First find the tsession record for this test.
     */
    tsession = cntrl->tests;
    if(!tsession || (memcmp(sid,tsession->sid,sizeof(BWLSID)) != 0))
        return False;

    return _BWLEndpointStatus(cntrl->ctx,tsession,aval,&err);
}

int
BWLSessionsActive(
        BWLControl      cntrl,
        BWLAcceptType   *aval
        )
{
    BWLTestSession  tsession;
    BWLAcceptType   laval = 0;
    BWLErrSeverity  err;

    tsession = cntrl->tests;
    if(tsession && _BWLEndpointStatus(cntrl->ctx,tsession,&laval,&err) && (laval < 0))
        return 1;

    if(aval)
        *aval = laval;

    return 0;
}

BWLErrSeverity
BWLStopSession(
        BWLControl      cntrl,
        int             *retn_on_intr,
        BWLAcceptType   *acceptval_ret    /* in/out    */
        )
{
    BWLErrSeverity  err,err2=BWLErrOK;
    BWLRequestType  msgtype;
    BWLAcceptType   aval=BWL_CNTRL_ACCEPT;
    BWLAcceptType   *acceptval=&aval;
    int             ival=0;
    int             *intr=&ival;
    FILE            *fp;

    if(!cntrl->tests){
        return BWLErrOK;
    }

    if(acceptval_ret){
        acceptval = acceptval_ret;
    }

    if(retn_on_intr){
        intr = retn_on_intr;
    }

    /*
     * Stop the local endpoint. This should not return until
     * the datafile is "flushed" into "localfp".
     */
    (void)_BWLEndpointStop(cntrl->ctx,cntrl->tests,*acceptval,&err2);

    /*
     * If acceptval would have been "success", but stopping of local
     * endpoints failed, send failure acceptval instead and return error.
     * (The endpoint_stop_func should have reported the error.)
     */
    if(!*acceptval && (err2 < BWLErrWARNING)){
        *acceptval = BWL_CNTRL_FAILURE;
        fp = NULL;
    }
    else{
        fp = cntrl->tests->localfp;
    }

    err = (BWLErrSeverity)_BWLWriteStopSession(cntrl,intr,*acceptval,fp);
    if(err < BWLErrWARNING)
        return _BWLFailControlSession(cntrl,BWLErrFATAL);
    err2 = MIN(err,err2);

    msgtype = BWLReadRequestType(cntrl,intr);
    if(msgtype == BWLReqSockClose){
        BWLError(cntrl->ctx,BWLErrFATAL,errno,
                "BWLStopSession:Control socket closed: %M");
        return _BWLFailControlSession(cntrl,BWLErrFATAL);
    }
    if(msgtype != BWLReqStopSession){
        BWLError(cntrl->ctx,BWLErrFATAL,BWLErrINVALID,
                "Invalid protocol message received...");
        return _BWLFailControlSession(cntrl,BWLErrFATAL);
    }

    if( (err = _BWLReadStopSession(cntrl,acceptval,intr,
                    cntrl->tests->remotefp)) != BWLErrOK){
        return _BWLFailControlSession(cntrl,(int)err);
    }
    err2 = MIN(err,err2);

    err = _BWLCallProcessResults(cntrl->tests);
    err2 = MIN(err,err2);

    err = _BWLTestSessionFree(cntrl->ctx,cntrl->tests,*acceptval);

    cntrl->state &= ~_BWLStateTest;

    return MIN(err,err2);
}

int
BWLStopSessionWait(
        BWLControl      cntrl,
        BWLNum64        *wake,
        int             *retn_on_intr,
        BWLAcceptType   *acceptval_ret,
        BWLErrSeverity  *err_ret
        )
{
    struct timeval  reltime;
    struct timeval  *waittime = NULL;
    fd_set          readfds;
    fd_set          exceptfds;
    int             rc;
    int             msgtype;
    BWLErrSeverity  err2=BWLErrOK;
    BWLAcceptType   aval;
    BWLAcceptType   *acceptval=&aval;
    int             ival=0;
    int             *intr=&ival;
    FILE            *fp;

    *err_ret = BWLErrOK;
    if(acceptval_ret){
        acceptval = acceptval_ret;
    }
    *acceptval = BWL_CNTRL_ACCEPT;

    if(retn_on_intr){
        intr = retn_on_intr;
    }

    if(!cntrl || cntrl->sockfd < 0){
        *err_ret = BWLErrFATAL;
        return -1;
    }

    /*
     * If there are no active sessions, get the status and return.
     */
    if(!BWLSessionsActive(cntrl,acceptval) || (*acceptval)){
        /*
         * Sessions are complete - send StopSession message.
         */
        *err_ret = BWLStopSession(cntrl,intr,acceptval);
        return 0;
    }

AGAIN:
    if(wake){
        BWLTimeStamp    currstamp;
        BWLNum64        wakenum;

        if(!BWLGetTimeStamp(cntrl->ctx,&currstamp)){
            BWLError(cntrl->ctx,BWLErrFATAL,BWLErrUNKNOWN,
                    "BWLGetTimeStamp(): %M");
            return -1;
        }

        if(BWLNum64Cmp(currstamp.tstamp,*wake) < 0){
            wakenum = BWLNum64Sub(*wake,currstamp.tstamp);
            BWLNum64ToTimeval(&reltime,wakenum);
        }
        else{
            tvalclear(&reltime);
        }

        waittime = &reltime;
    }


    FD_ZERO(&readfds);
    FD_SET(cntrl->sockfd,&readfds);
    FD_ZERO(&exceptfds);
    FD_SET(cntrl->sockfd,&exceptfds);
    rc = select(cntrl->sockfd+1,&readfds,NULL,&exceptfds,waittime);

    if(rc < 0){
        if(errno != EINTR){
            BWLError(cntrl->ctx,BWLErrFATAL,BWLErrUNKNOWN,
                    "select():%M");
            *err_ret = BWLErrFATAL;
            return -1;
        }
        if(waittime || *intr){
            return 2;
        }

        /*
         * If there are tests still happening, and no tests have
         * ended in error - go back to select and wait for the
         * rest of the tests to complete.
         */
        if(BWLSessionsActive(cntrl,acceptval) && !*acceptval){
            goto AGAIN;
        }

        /*
         * Sessions are complete - send StopSession message.
         */
        *err_ret = BWLStopSession(cntrl,intr,acceptval);

        return 0;
    }
    if(rc == 0)
        return 1;

    if(!FD_ISSET(cntrl->sockfd,&readfds) &&
            !FD_ISSET(cntrl->sockfd,&exceptfds)){
        BWLError(cntrl->ctx,BWLErrFATAL,BWLErrUNKNOWN,
                "select():cntrl fd not ready?:%M");
        *err_ret = _BWLFailControlSession(cntrl,BWLErrFATAL);
        return -1;
    }

    msgtype = BWLReadRequestType(cntrl,intr);
    if(msgtype == 0){
        BWLError(cntrl->ctx,BWLErrFATAL,errno,
                "BWLStopSessionWait: Control socket closed: %M");
        *err_ret = _BWLFailControlSession(cntrl,BWLErrFATAL);
        return -1;
    }
    if(msgtype != 3){
        BWLError(cntrl->ctx,BWLErrFATAL,BWLErrINVALID,
                "Invalid protocol message received...");
        *err_ret = _BWLFailControlSession(cntrl,BWLErrFATAL);
        return -1;
    }

    *err_ret = _BWLReadStopSession(cntrl,intr,acceptval,
            cntrl->tests->remotefp);
    if(*err_ret != BWLErrOK){
        *err_ret = _BWLFailControlSession(cntrl,(int)*err_ret);
        return -1;
    }

    /*
     * If StopSessions was sent with accept==0, then make EndpointStop
     * wait for the local endpoint to exit before continuing.
     */
    if(*acceptval == 0){
        cntrl->tests->endpoint->dont_kill = 1;
    }

    /*
     * Stop the local endpoint. This should not return until
     * the datafile is "flushed" into "localfp".
     */
    (void)_BWLEndpointStop(cntrl->ctx,cntrl->tests,*acceptval,&err2);
    if(err2 < BWLErrWARNING){
        *acceptval = BWL_CNTRL_FAILURE;
        fp = NULL;
    }
    else{
        fp = cntrl->tests->localfp;
    }
    *err_ret = MIN(*err_ret,err2);

    if( (err2 = _BWLWriteStopSession(cntrl,intr,*acceptval,fp)) !=
            BWLErrOK){
        (void)_BWLFailControlSession(cntrl,(int)err2);
    }
    *err_ret = MIN(*err_ret,err2);

    err2 = _BWLCallProcessResults(cntrl->tests);
    *err_ret = MIN(*err_ret,err2);

    while(cntrl->tests){
        err2 = _BWLTestSessionFree(cntrl->ctx,cntrl->tests,*acceptval);
        *err_ret = MIN(*err_ret,err2);
    }

    cntrl->state &= ~_BWLStateTest;

    *err_ret = MIN(*err_ret, err2);
    return 0;
}
