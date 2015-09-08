#include "gmusicapi/protocol/mc_calls.h"

#include "cpprest/json.h"

using namespace std;

using namespace web;
using namespace web::http;
using namespace utility;
using namespace utility::conversions;

using namespace gmusicapi::protocol;

static map< string, string > parse_key_value( const http_response& res ) {
	map< string, string > v;
	
	stringstream ss( res.extract_utf8string( ).get( ) );
	string line;
	while( getline( ss, line, '\n' ) ) {
		size_t i = line.find_first_of( '=' );

		v[ line.substr( 0, i ) ] = line.substr( i + 1 );
	}

	return v;
}


LoginCall::LoginCall( const std::string& email, const std::string& password, const std::string& androidID ) 
	: email( to_string_t( email ) ), password( to_string_t( password ) ), androidID( to_string_t( androidID ) ) { }

method LoginCall::method( ) {
	return methods::POST;
}

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
	ss << "&Passwd=" << this->password;
	ss << "&androidId=" << this->androidID;
	
	req.set_body( ss.str( ), content_type );
}

map< string, string > LoginCall::parse_response( const http_response& res ) {
	return parse_key_value( res );
}

OAuthCall::OAuthCall( const std::string& masterToken, const std::string& androidID )
	: masterToken( to_string_t( masterToken ) ), androidID( to_string_t( androidID ) ) { }

method OAuthCall::method( ) {
	return methods::POST;
}

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
	ss << "&androidId=" << this->androidID;
	ss << "&Token=" << this->masterToken;

	req.set_body( ss.str( ), content_type );
}

map< string, string > OAuthCall::parse_response( const http_response& res ) {
	return parse_key_value( res );
}

ListTracksCall::ListTracksCall( const std::string& oauthToken )
	: AuthenticatedCall< ListTracksCall >( oauthToken ) { }

method ListTracksCall::method( ) {
	return methods::POST;
}

string_t ListTracksCall::get_endpoint( ) {
	const string_t endpoint = U( "trackfeed" );
	return endpoint;
}

void ListTracksCall::set_body( http_request& req ) {
	json::value body = json::value::object( );

	body[ U( "max-results" ) ] = json::value::number( 20000 );

	req.set_body( body );
}

json::value ListTracksCall::parse_response( const http_response& res ) {
	return res.extract_json( ).get( );
}