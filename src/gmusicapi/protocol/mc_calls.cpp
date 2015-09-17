#include "gmusicapi/protocol/mc_calls.h"
#include "gmusicapi/b64.h"
#include "gmusicapi/google.h"

#include "cpprest/json.h"
#include "cpprest/interopstream.h"

#include "openssl/hmac.h"

#include <ctime>
#include <iostream>

using namespace std;

using namespace web;
using namespace web::http;

using namespace gmusicapi;
using namespace gmusicapi::protocol;

static map< string_t, string_t > parse_key_value( const http_response& res ) {
	map< string_t, string_t > v;
	
	stringstream_t ss( res.extract_string( ).get( ) );
	string_t line;
	while( getline( ss, line, ( char_t ) '\n' ) ) {
		size_t i = line.find_first_of( '=' );

		v[ line.substr( 0, i ) ] = line.substr( i + 1 );
	}

	return v;
}


LoginCall::LoginCall( const string_t& email, const string_t& password, const string_t& androidID ) 
	: email( email ), password( password ), androidID( androidID ) { }

string_t LoginCall::get_endpoint( ) {
	static const string_t endpoint = U( "auth" );
	return endpoint;
}

void LoginCall::set_body( http_request& req ) {
	static const string_t content_type = U( "application/x-www-form-urlencoded" );

	stringstream_t ss;

	ss << "accountType=HOSTED_OR_GOOGLE";
	ss << "&has_permission=1";
	ss << "&add_account=1";
	ss << "&service=ac2dm";
	ss << "&source=android";
	ss << "&device_country=us";
	ss << "&operatorCountry=us";
	ss << "&lang=en";
	ss << "&sdk_version=17";
	ss << "&Email=" << this->email;
	ss << "&EncryptedPasswd=" << gpsoauth_signature( this->email, this->password );
	ss << "&androidId=" << this->androidID;
	
	req.set_body( ss.str( ), content_type );
}

map< string_t, string_t > LoginCall::parse_response( const http_response& res ) {
	return parse_key_value( res );
}

OAuthCall::OAuthCall( const string_t& masterToken, const string_t& androidID )
	: masterToken( masterToken ), androidID( androidID ) { }

string_t OAuthCall::get_endpoint( ) {
	static const string_t endpoint = U( "auth" );
	return endpoint;
}

void OAuthCall::set_body( http_request& req ) {
	static const string_t content_type = U( "application/x-www-form-urlencoded" );

	stringstream_t ss;

	ss << "accountType=HOSTED_OR_GOOGLE";
	ss << "&has_permission=1";
	ss << "&service=sj";
	ss << "&source=android";
	ss << "&app=com.google.android.music";
	ss << "&device_country=us";
	ss << "&operatorCountry=us";
	ss << "&lang=en";
	ss << "&sdk_version=17";
	ss << "&client_sig=38918a453d07199354f8b19af05ec6562ced5788";
	ss << "&androidId=" << this->androidID;
	ss << "&EncryptedPasswd=" << this->masterToken;

	req.set_body( ss.str( ), content_type );
}

map< string_t, string_t > OAuthCall::parse_response( const http_response& res ) {
	return parse_key_value( res );
}

ListTracksCall::ListTracksCall( size_t max_results, const string_t& page_token )
	: max_results( max_results ), page_token( page_token ) { }

string_t ListTracksCall::get_endpoint( ) {
	const string_t endpoint = U( "trackfeed" );
	return endpoint;
}

void ListTracksCall::set_body( http_request& req ) {
	json::value body = json::value::object( );

	body[ U( "max-results" ) ] = json::value::number( this->max_results );
	if( this->page_token.size( ) != 0 ) {
		body[ U( "start-token" ) ] = json::value::string( this->page_token );
	}

	req.set_body( body );
}

json::value ListTracksCall::parse_response( const http_response& res ) {
	return res.extract_json( ).get( );
}

GetSongStreamCall::GetSongStreamCall( const string_t& song_id )
	: song_id( song_id ) { }

string_t GetSongStreamCall::get_endpoint( ) {
	static const string_t endpoint = U( "music/mplay" );
	return endpoint;
}

void GetSongStreamCall::get_query_params( map< string_t, string_t >& query_params ) {
	static vector< unsigned char > key = base64_decode( U( "MzRlZTc5ODMtNWVlNi00MTQ3LWFhODYtNDQzZWEwNjJhYmY3NzQ0OTNkNmEtMmExNS00M2ZlLWFhY2UtZTc4NTY2OTI3NTg1Cg==" ) );

	stringstream ss;
	ss << ( time( 0 ) * 1000 );
	
	string sID = utility::conversions::to_utf8string( song_id );
	string salt = ss.str( );

	unsigned char result[ 20 ];
	unsigned int len = 20;

	HMAC_CTX ctx;
	HMAC_CTX_init( &ctx );

	HMAC_Init_ex( &ctx, &key[ 0 ], key.size( ), EVP_sha1( ), NULL );
	HMAC_Update( &ctx, ( unsigned char * ) &sID[ 0 ], sID.size( ) );
	HMAC_Update( &ctx, ( unsigned char * ) &salt[ 0 ], salt.size( ) );
	HMAC_Final( &ctx, result, &len );
	HMAC_CTX_cleanup( &ctx );

	string_t sig = base64_encode_urlsafe( result, len );
	sig = sig.substr( 0, sig.size( ) - 1 );

	query_params[ U( "opt" ) ] = U( "hi" );
	query_params[ U( "net" ) ] = U( "mob" );
	query_params[ U( "pt" ) ] = U( "e" );
	query_params[ U( "slt" ) ] = utility::conversions::to_string_t( salt );
	query_params[ U( "sig" ) ] = sig;

	if( song_id[ 0 ] == 'T' ) {
		query_params[ U( "mjck" ) ] = song_id;
	}
	else {
		query_params[ U( "songid" ) ] = song_id;
	}
}

cibytestream GetSongStreamCall::parse_response( const http_response& res ) {
	return res.body( );
}

string_t GetDeviceInfoCall::get_endpoint( ) {
	static const string_t endpoint = U( "devicemanagementinfo" );
	return endpoint;
}

json::value GetDeviceInfoCall::parse_response( const http_response& res ) {
	return res.extract_json( ).get( );
}