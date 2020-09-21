#include <stdio.h>
#include <string.h>
#include <slapi-plugin.h>
#include <prnetdb.h>

Slapi_PluginDesc bindpdesc = { "LDAPS only", "Jan Tomasek", "0.3",
			       "Pre-Bind plugin to prevent users from binding over nonSSL lines." };

/* our plug-in identity . set in init function */
static Slapi_ComponentId *my_plugin_identity;

#define LDAPSONLY_MAX_ADDRESS 1024
#define LDAPSONLY_USE_SSL     "Please use SSL connection for non anonymous bind."


int ldapsonly_bind(Slapi_PBlock *pb) {
  char *dn;
  int isSSL;
  PRNetAddr clientAddress;
  char clientAddressString[LDAPSONLY_MAX_ADDRESS];
  char **argv;
  int  argc; 
  int  i;
  int  permit = 0;
  char *permited_by;
  
  /* Log a message to the server error log. */
  slapi_log_error( SLAPI_LOG_PLUGIN, "ldapsonly_bind", "Pre-operation bind function called.\n" );

  /* Gets parameters available when processing an LDAP bind operation. */
  if (slapi_pblock_get(pb, SLAPI_BIND_TARGET, &dn)!=0 ||
      slapi_pblock_get(pb, SLAPI_CONN_IS_SSL_SESSION, &isSSL)!=0 ||
      slapi_pblock_get(pb, SLAPI_CONN_CLIENTNETADDR, &clientAddress)) {
    slapi_log_error(SLAPI_LOG_PLUGIN, "ldapsonly_bind" ,"Could not get parameters for bind operation.\n");
    slapi_send_ldap_result(pb, LDAP_OPERATIONS_ERROR,
			   NULL, NULL, 0, NULL );
    return(1);
  }

  if (isSSL==0) {

    if (slapi_pblock_get(pb, SLAPI_PLUGIN_ARGV, &argv)!=0 ||
	slapi_pblock_get(pb, SLAPI_PLUGIN_ARGC, &argc)!=0 ) {
      /* tohle NEselze kdyz neni definovan zadny argument v konfiguraci */
      slapi_log_error(SLAPI_LOG_PLUGIN,
		      "ldapsonly_bind", "Could not get argc or argv\n");
      slapi_send_ldap_result(pb, LDAP_OPERATIONS_ERROR,
			     NULL, NULL, 0, NULL);
      return(1);
    } 

    PR_NetAddrToString(&clientAddress, clientAddressString, LDAPSONLY_MAX_ADDRESS);

    if (argc>0) {
      for (i = 0; i<argc; i++) {
	if (strlen(clientAddressString) == strlen(argv[i])
	    && strcmp(clientAddressString, argv[i])==0) {
	  permit = 1;
	  permited_by = argv[i];
	  i = argc;
	};
      };
    };

    if (permit) {
      slapi_log_error(SLAPI_LOG_PLUGIN, "ldapsonly_bind",
		      "User '%s' from IP '%s' is not using SSL(%d). Bind is permitted because he comes from configured IP '%s'.\n",
		      dn, clientAddressString, isSSL, permited_by);
    } else {
      slapi_log_error(SLAPI_LOG_FATAL, "ldapsonly_bind", 
		      "User '%s' from IP '%s' is not using SSL(%d). Bind refused.\n",
		      dn, clientAddressString, isSSL);
      slapi_send_ldap_result(pb, LDAP_STRONG_AUTH_REQUIRED,
			     NULL, LDAPSONLY_USE_SSL, 0, NULL );
      return(1);
    };
  };
  
  return(0);
}

int ldapsonly_init(Slapi_PBlock *pb) {
  /* get our plug-in identity . we will need this to perform
     any internal operations (search, modify, etc.) */
  slapi_pblock_get (pb, SLAPI_PLUGIN_IDENTITY, &my_plugin_identity);

  /* Register the pre-operation bind function and specify
  the server plug-in version. */
  if (slapi_pblock_set( pb, SLAPI_PLUGIN_VERSION,SLAPI_PLUGIN_VERSION_03 )!=0 ||
      slapi_pblock_set( pb, SLAPI_PLUGIN_DESCRIPTION,(void *)&bindpdesc )!=0 ||
      slapi_pblock_set( pb, SLAPI_PLUGIN_PRE_BIND_FN,(void *) ldapsonly_bind )!=0 ) {
    slapi_log_error( SLAPI_LOG_PLUGIN, "ldapsonly_init" , "Failed to set version and function.\n" );
    return(-1);
  }

  return(0);
}
