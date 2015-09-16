#include "gmusicapi/mobileclient.h"
#include "gmusicapi/protocol/mc_calls.h"

#include <map>
#include <iostream>

#include "cpprest/http_client.h"

using namespace std;

using namespace web;
using namespace web::http;
using namespace web::http::client;

using namespace boost;

using namespace gmusicapi;
using namespace gmusicapi::protocol;

MobileClient::MobileClient( )
	: isAuthenticated( false ),
	  sjClient( U( "https://www.googleapis.com/sj/v1.11" ) ),
	  androidClient( U( "https://android.clients.google.com/" ) )  {
}

bool MobileClient::login( const string_t& email, const string_t& password, const string_t& androidID ) {
	if( this->isAuthenticated ) {
		return false;
	}

	http_client client( U( "https://android.clients.google.com/" ) );
	map< string_t, string_t > res;

	res = LoginCall( email, password, androidID ).make_call( client ).get( );
	auto itr = res.find( U( "Token" ) );
	if( itr == res.end( ) ) {
		return false;
	}

	
	res = OAuthCall( itr->second, androidID ).make_call( client ).get( );
	itr = res.find( U( "Auth" ) );
	if( itr == res.end( ) ) {
		return false;
	}
	
	string_t oauthToken = itr->second;

	auto handler = [ androidID, oauthToken ]( http_request request, std::shared_ptr< http_pipeline_stage >& handler ) {
		static string_t auth_header_name = U( "Authorization" );
		static string_t dvid_header_name = U( "X-Device-ID" );

		stringstream_t ss;
		ss << "GoogleLogin auth=" << oauthToken;

		request.headers( ).add( auth_header_name, ss.str( ) );
		request.headers( ).add( dvid_header_name, androidID );

		return handler->propagate( request );
	};
	sjClient.add_handler( handler );
	androidClient.add_handler( handler );

	this->isAuthenticated = true;

	return true;
}

vector< Song > MobileClient::get_all_songs( ) {
	vector< Song > res;

	Generator< TrackGenerator > gen = this->get_all_tracks( );
	while( gen ) {
		res.push_back( *gen++ );
	}

	return res;
}


Generator< TrackGenerator > MobileClient::get_all_tracks( unsigned int page_size ) {
	return Generator< TrackGenerator >( this->sjClient, page_size );
}

vector< unsigned char > MobileClient::get_song_bytes( const string_t& song_id ) {
	return GetSongBytesCall( song_id ).make_call( this->androidClient ).get( );
}

json::value MobileClient::get_registered_devices( ) {
	return GetDeviceInfoCall( ).make_call( this->sjClient ).get( );
}