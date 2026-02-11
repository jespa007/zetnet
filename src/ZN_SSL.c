#include "zetnet.h"

static SSL_CTX *sctx = NULL;

void ZN_SSL_Init(void){
  SSL_library_init ();
  SSL_load_error_strings ();
  OpenSSL_add_all_algorithms ();

  printf (
	"OpenSSL version \"%s\"\n",
	SSLeay_version (SSLEAY_VERSION)
  );

  // create ssl context
  sctx = SSL_CTX_new (SSLv23_server_method ());
  SSL_CTX_set_options (ctx,
                       SSL_OP_SINGLE_DH_USE |
                       SSL_OP_SINGLE_ECDH_USE |
                       SSL_OP_NO_SSLv2);

  /* Cheesily pick an elliptic curve to use with elliptic curve ciphersuites.
   * We just hardcode a single curve which is reasonably decent.
   * See http://www.mail-archive.com/openssl-dev@openssl.org/msg30957.html */
  EC_KEY *ecdh = EC_KEY_new_by_curve_name (NID_X9_62_prime256v1);
  if (! ecdh)
    die_most_horribly_from_openssl_error ("EC_KEY_new_by_curve_name");
  if (1 != SSL_CTX_set_tmp_ecdh (ctx, ecdh))
    die_most_horribly_from_openssl_error ("SSL_CTX_set_tmp_ecdh");
}

static void ZN_SSL_SetupCerts (SSL_CTX *ctx,
                                const char *certificate_chain,
                                const char *private_key)
{

 info_report ("Loading certificate chain from '%s'\n"
               "and private key from '%s'\n",
               certificate_chain, private_key);

  if (1 != SSL_CTX_use_certificate_chain_file (ctx, certificate_chain))
    die_most_horribly_from_openssl_error ("SSL_CTX_use_certificate_chain_file");

  if (1 != SSL_CTX_use_PrivateKey_file (ctx, private_key, SSL_FILETYPE_PEM))
    die_most_horribly_from_openssl_error ("SSL_CTX_use_PrivateKey_file");

  if (1 != SSL_CTX_check_private_key (ctx))
    die_most_horribly_from_openssl_error ("SSL_CTX_check_private_key");
}

void ZN_SSL_Deinit(void){
	SSL_CTX_free (sctx);

}
