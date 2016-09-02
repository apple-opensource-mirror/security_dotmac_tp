/*
 * Copyright (c) 2004 Apple Computer, Inc. All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/*
 * dotMacTp.h - private SPI for .mac TP
 */
 
#ifndef	_DOT_MAC_TP_H_
#define _DOT_MAC_TP_H_

#include <Security/cssmtype.h>
#include <Security/x509defs.h>
#include <Security/oidsalg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Some hard-coded constants related to .mac Cert acquisition.
 */
#define DOT_MAC_KEY_ALG					CSSM_ALGID_RSA			/* alg of the key */
#define DOT_MAC_KEY_SIZE				1024					/* key size in bits */
#define DOT_MAC_CSR_SIGNATURE_ALGID		CSSM_ALGID_SHA1WithRSA  /* sig alg of the CSR */
#define DOT_MAC_CSR_SIGNATURE_ALGOID	CSSMOID_SHA1WithRSA		/* ditto */

/* By default, sign requests go to http://certmgmt.mac.com/sign */
#define DOT_MAC_SIGN_SCHEMA				"http://"
#define DOT_MAC_SIGN_HOST_NAME			"certmgmt.mac.com"		/* default server */
#define DOT_MAC_SIGN_PATH				"/sign"

/* By default, archive requests go to http://certmgmt.mac.com/archive */
#define DOT_MAC_ARCHIVE_SCHEMA			"http://"
#define DOT_MAC_ARCHIVE_HOST_NAME		"certmgmt.mac.com"		/* default server */
#define DOT_MAC_ARCHIVE_PATH			"/archive"

/* 
 * lookup requests go to http://certinfo.mac.com/ 
 * Note that lookup via RetrieveCredResult() has no mechanism for specifying an 
 * alternate lookup host
 */
#define DOT_MAC_LOOKUP_SCHEMA			"http://"
#ifdef	NDEBUG
#define DOT_MAC_LOOKUP_HOST				"certinfo.mac.com"
#else
#define DOT_MAC_LOOKUP_HOST				"certinfo.mac.com"
#endif

/* the path to use when fetching all of a user's certs */
#define DOT_MAC_LOOKUP_PATH				"/lookup?"			/* followed by user name */

/* paths for per-cert-type lookups (all followed name) */

#define DOT_MAC_LOOKUP_ID_PATH			"/lookup/ichat?"
#define DOT_MAC_LOOKUP_SIGN_PATH		"/lookup/email?"
#define DOT_MAC_LOOKUP_ENCRYPT_PATH		"/lookup/emailencrypt?"

/*
 * Contents of CSSM_APPLE_DOTMAC_TP_CERT_REQUEST.flags.
 */
enum {
	/* 
	 * Do not post the actual request. Generally used in conjunction with 
	 * CSSM_DOTMAC_TP_RETURN_CSR.
	 */
	CSSM_DOTMAC_TP_DO_NOT_POST  = 0x00000001,
	
	/* 
	 *  Return the generated CSR in CSSM_APPLE_DOTMAC_TP_CERT_REQUEST.Csr.
	 */
	CSSM_DOTMAC_TP_RETURN_CSR   = 0x00000002,
	
	/* 
	 * Post a renew request instead of new.
	 */
	CSSM_DOTMAC_TP_SIGN_RENEW   = 0x00000004,
	
	/*
	 * Use existing CSR from CSSM_APPLE_DOTMAC_TP_CERT_REQUEST.Csr.
	 */
	CSSM_DOTMAC_TP_EXIST_CSR	= 0x00000008,
	
	/*
	 * Ask the server if a request is pending; for lookup only.
	 * When set, just does a query, doesn't return any data. 
	 */
	CSSM_DOTMAC_TP_IS_REQ_PENDING	= 0x00000010
};

/* version of CSSM_APPLE_DOTMAC_TP_CERT_REQUEST */
#define CSSM_DOT_MAC_TP_REQ_VERSION		0

/* 
 * Cert request passed to CSSM_TP_SubmitCredRequest() in the
 * CSSM_TP_AUTHORITY_REQUEST_TYPE.Requests field.
 */
typedef struct {
	uint32							version;
	CSSM_CSP_HANDLE					cspHand;			// sign with this CSP
	CSSM_CL_HANDLE					clHand;				// and this CL
	uint32							numTypeValuePairs;  // size of typeValuePairs[]
	CSSM_X509_TYPE_VALUE_PAIR_PTR	typeValuePairs;		// user name, etc. 
	CSSM_KEY						*publicKey;			// included in CSR
	CSSM_KEY						*privateKey;		// signs the CSR
	CSSM_DATA						userName;			// UTF8 encoded user name
	CSSM_DATA						password;			// UTF8 encoded password
	uint32							flags;
	CSSM_DATA						csr;				//�optional in/out
} CSSM_APPLE_DOTMAC_TP_CERT_REQUEST;

/*
 * Additional CSSM_TP_AUTHORITY_REQUEST_TYPE values
 */
#define CSSM_TP_AUTHORITY_REQUEST_PRIVATE		0x80000000
enum
{
	/* cert lookup, using userName in a CSSM_APPLE_DOTMAC_TP_CERT_REQUEST */
	CSSM_TP_AUTHORITY_REQUEST_CERTLOOKUP		= CSSM_TP_AUTHORITY_REQUEST_PRIVATE + 0
};

/* 
 * An archive List operation returns an array of these in the 
 * CSSM_APPLE_DOTMAC_TP_ARCHIVE_REQUEST.list field. Caller must
 * free all contents, including the array itself, with its
 * CSSM-registered free() callback.
 */
typedef struct {
	CSSM_DATA	archiveName;		// UTF8 encoded archive name
	CSSM_DATA	timeString;			// UNIX timestring
} DotMacArchive;

/* 
 * Archive request passed to CSSM_TP_SubmitCredRequest() in the
 * CSSM_TP_AUTHORITY_REQUEST_TYPE.Requests field.
 */

#define CSSM_DOT_MAC_TP_ARCHIVE_REQ_VERSION		0

typedef struct {
	uint32			version;		// CSSM_DOT_MAC_TP_ARCHIVE_REQ_VERSION
	CSSM_DATA		userName;		// UTF8 encoded user name required for all
	CSSM_DATA		password;		// UTF8 encoded password required for all
	CSSM_DATA		archiveName;	// UTF8 encoded archive name: store, fetch, remove
	CSSM_DATA		timeString;		// UNIX timestring, store only
	
	/*
	 * On archive store, caller places the PKCS12 PFX to store here.
	 * On archive fetch, the fetched PKCS12 PFX is returned here. App must free
	 *   the referent data via the CSSM-registered free() callback.
	 */
	CSSM_DATA		pfx;
	
	/* archive list only: see comments above for DotMacArchive */
	unsigned		numArchives;
	DotMacArchive	*archives;

} CSSM_APPLE_DOTMAC_TP_ARCHIVE_REQUEST;

#ifdef __cplusplus
}
#endif

#endif	/* _DOT_MAC_TP_H_ */
