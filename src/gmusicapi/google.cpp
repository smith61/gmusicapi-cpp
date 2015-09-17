
#include <vector>

#include "boost/endian/conversion.hpp"

#include "openssl/bn.h"
#include "openssl/rsa.h"
#include "openssl/sha.h"

#include "gmusicapi/google.h"
#include "gmusicapi/b64.h"

using namespace std;

using namespace boost::endian;

using namespace utility::conversions;

using namespace gmusicapi;

static RSA* decode_key( const vector< unsigned char >& encoded_key );
static void vector_append( vector< unsigned char >& to, const vector< unsigned char >& from );
static void vector_append( vector< unsigned char >& to, const string_t& from );

static string_t b64_key_7_3_29 = U( "AAAAgMom/1a/v0lblO2Ubrt60J2gcuXSljGFQXgcyZWveWLEwo6prwgi3iJIZdodyhKZQrNWp5nKJ3srRXcUW+F1BD3baEVGcmEgqaLZUNBjm057pKRI16kB0YppeGx5qIQ5QjKzsR8ETQbKLNWgRY0QRNVz34kMJR3P/LgHax/6rmf5AAAAAwEAAQ==" );
static vector< unsigned char > android_key_7_3_29_encoded = base64_decode( b64_key_7_3_29 );
static RSA* android_key_7_3_29 = decode_key( android_key_7_3_29_encoded );

string_t gmusicapi::gpsoauth_signature( const string_t& email, const string_t& pass ) {
	vector< unsigned char > sig;
	vector< unsigned char > buf;
	vector< unsigned char > email_pass;

	sig.push_back( 0 );


	buf.resize( 20 );
	SHA1( &android_key_7_3_29_encoded[ 0 ], android_key_7_3_29_encoded.size( ), &buf[ 0 ] );
	buf.resize( 4 );
	vector_append( sig, buf );

	vector_append( email_pass, email );
	email_pass.push_back( 0 );
	vector_append( email_pass, pass );
	buf.resize( RSA_size( android_key_7_3_29 ) );
	buf.resize( RSA_public_encrypt( email_pass.size( ), &email_pass[ 0 ], &buf[ 0 ], android_key_7_3_29, RSA_PKCS1_OAEP_PADDING ) );


	vector_append( sig, buf );

	return base64_encode_urlsafe( &sig[ 0 ], sig.size( ) );
}

static RSA* decode_key( const vector< unsigned char >& encoded_key ) {
	uint32_t i = *( ( const uint32_t* ) &encoded_key[ 0 ] );
	big_to_native_inplace( i );
	const unsigned char* mod = &encoded_key[ 4 ];

	uint32_t j = *( ( const uint32_t* ) &encoded_key[ i + 4 ] );
	big_to_native_inplace( j );
	const unsigned char* exp = &encoded_key[ i + 8 ];

	RSA* key = RSA_new( );
	key->n = BN_bin2bn( mod, i, BN_new( ) );
	key->e = BN_bin2bn( exp, j, BN_new( ) );

	return key;
}

static void vector_append( vector< unsigned char >& to, const vector< unsigned char >& from ) {
	size_t size = to.size( );
	to.resize( size + from.size( ) );

	copy( from.begin( ), from.end( ), to.begin( ) + size );
}

static void vector_append( vector< unsigned char >& to, const string_t& from ) {
	string utf8_from = to_utf8string( from );

	size_t size = to.size( );
	to.resize( size + utf8_from.length( ) );
	for( size_t i = 0; i < utf8_from.size( ); i++ ) {
		to[ size + i ] = utf8_from[ i ];
	}
}