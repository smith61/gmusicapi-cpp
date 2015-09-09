#include "gmusicapi/mobileclient.h"
#include "gmusicapi/protocol/mc_calls.h"

#include <map>
#include <iostream>

#include "cpprest/http_client.h"

using namespace std;

using namespace web;
using namespace web::http::client;

using namespace gmusicapi;
using namespace gmusicapi::protocol;

MobileClient::MobileClient( )
	: isAuthenticated( false ), oauthToken( ), sjClient( U( "https://www.googleapis.com/sj/v1.11" ) ) { }

bool MobileClient::login( const string_t& email, const string_t& password, const string_t& androidID ) {
	if( this->isAuthenticated ) {
		return false;
	}

	http_client client( U( "https://android.clients.google.com/" ) );
	map< string_t, string_t > res;

	res = LoginCall( email, password, androidID ).make_call( client );
	auto itr = res.find( U( "Token" ) );
	if( itr == res.end( ) ) {
		return false;
	}

	
	res = OAuthCall( itr->second, androidID ).make_call( client );
	itr = res.find( U( "Auth" ) );
	if( itr == res.end( ) ) {
		return false;
	}
	
	this->oauthToken = itr->second;
	this->isAuthenticated = true;

	return true;
}

vector< Song > MobileClient::get_all_songs( ) {
	vector< Song > ret;
	json::value val = ListTracksCall( this->oauthToken ).make_call( this->sjClient );
	val = val.at( U( "data" ) ).at( U( "items" ) );

	if( val.is_array( ) ) {
		json::array songs = val.as_array( );
		for( auto song : songs ) {
			ret.push_back( Song( song ) );
		}
	}

	return ret;
}