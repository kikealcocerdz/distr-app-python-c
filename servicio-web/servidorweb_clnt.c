/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "servidorweb.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

enum clnt_stat 
terminal_rpc_1(char *user, int request, char *timestamp, char *filename, int *clnt_res,  CLIENT *clnt)
{
	terminal_rpc_1_argument arg;
	arg.user = user;
	arg.request = request;
	arg.timestamp = timestamp;
	arg.filename = filename;
	return (clnt_call (clnt, terminal_rpc, (xdrproc_t) xdr_terminal_rpc_1_argument, (caddr_t) &arg,
		(xdrproc_t) xdr_int, (caddr_t) clnt_res,
		TIMEOUT));
}
