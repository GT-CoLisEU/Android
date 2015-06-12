/*
 *      $Id$
 */
/************************************************************************
*									*
*			     Copyright (C)  2002			*
*				Internet2				*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		util.h
 *
 *	Author:		Jeff Boote
 *			Internet2
 *
 *	Date:		Tue Apr 23 10:11:16  2002
 *
 *	Description:	
 *			I2 Utility library. Currently contains:
 *				* error logging
 *				* command-line parsing
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
#ifndef	_I2_util_h_
#define	_I2_util_h_

#include <inttypes.h>

typedef intptr_t    I2Boolean;

#ifndef	False
#define	False	(0)
#endif
#ifndef	True
#define	True	!False
#endif

#ifndef MIN
#define MIN(a,b)    ((a<b)?a:b)
#endif
#ifndef MAX
#define MAX(a,b)    ((a>b)?a:b)
#endif

#ifdef	__cplusplus
#define	BEGIN_C_DECLS	extern "C" {
#define	END_C_DECLS	}
#else
#define	BEGIN_C_DECLS
#define	END_C_DECLS
#endif

#define	I2Number(arr)	(sizeof(arr)/sizeof(arr[0]))

#include "Pthread.h"
#include "errlog.h"
#include "table.h"
#include "random.h"
#include "io.h"
#include "saddr.h"
#include "md5.h"
#include "readpassphrase.h"
#include "hex.h"
#include "conf.h"
#include "addr.h"

#endif	/*	_I2_util_h_	*/
