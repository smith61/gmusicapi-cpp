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
	: isAuthenticated( false ), oauthToken( "" ), sjClient( U( "https://www.googleapis.com/sj/v1.11" ) ) { }

bool MobileClient::login( const string& email, const string& password, const string& androidID ) {
	if( this->isAuthenticated ) {
		return false;
	}

	http_client client( U( "https://android.clients.google.com/" ) );
	map< string, string > res;

	res = LoginCall( email, password, androidID ).make_call( client );
	auto itr = res.find( "Token" );
	if( itr == res.end( ) ) {
		return false;
	}

	
	res = OAuthCall( itr->second, androidID ).make_call( client );
	itr = res.find( "Auth" );
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

	ofstream out( "C:/Users/Jacob/Desktop/out.json" );
	val.serialize( out );
	out.close( );

	if( val.is_array( ) ) {
		json::array songs = val.as_array( );
		for( auto song : songs ) {
			ret.push_back( Song( song ) );
		}
	}

	return ret;
}