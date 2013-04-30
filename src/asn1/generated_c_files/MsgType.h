/*
 * Generated by asn1c-0.9.24 (http://lionet.info/asn1c)
 * From ASN.1 module "Module"
 * 	found in "Module.asn1"
 */

#ifndef	_MsgType_H_
#define	_MsgType_H_


#include <asn_application.h>

/* Including external dependencies */
#include <ENUMERATED.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum MsgType {
	MsgType_clientToServerInt	= 0,
	MsgType_serverToClientAnswer	= 1,
	MsgType_clientToServerString	= 2,
	MsgType_serverToClientResult	= 3
} e_MsgType;

/* MsgType */
typedef ENUMERATED_t	 MsgType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MsgType;
asn_struct_free_f MsgType_free;
asn_struct_print_f MsgType_print;
asn_constr_check_f MsgType_constraint;
ber_type_decoder_f MsgType_decode_ber;
der_type_encoder_f MsgType_encode_der;
xer_type_decoder_f MsgType_decode_xer;
xer_type_encoder_f MsgType_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _MsgType_H_ */
#include <asn_internal.h>