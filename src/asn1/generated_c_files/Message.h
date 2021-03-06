/*
 * Generated by asn1c-0.9.24 (http://lionet.info/asn1c)
 * From ASN.1 module "Module"
 * 	found in "Module.asn1"
 */

#ifndef	_Message_H_
#define	_Message_H_


#include <asn_application.h>

/* Including external dependencies */
#include "MsgType.h"
#include <INTEGER.h>
#include <IA5String.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Message */
typedef struct Message {
	MsgType_t	 msgType;
	INTEGER_t	 number;
	IA5String_t	 string;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Message_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Message;

#ifdef __cplusplus
}
#endif

#endif	/* _Message_H_ */
#include <asn_internal.h>
