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
	vector< Song > ret;
	json::value val = ListTracksCall( 10000 ).make_call( this->sjClient ).get( );
	val = val.at( U( "data" ) ).at( U( "items" ) );

	if( val.is_array( ) ) {
		json::array songs = val.as_array( );
		for( const auto& song : songs ) {
			ret.push_back( Song( song ) );
		}
	}

	return ret;
}


Generator< TrackGenerator > MobileClient::get_all_tracks( unsigned int page_size ) {
	shared_ptr< TrackGenerator > gen( new TrackGenerator( this->sjClient, page_size ) );
	return Generator< TrackGenerator >( gen );
}

vector< unsigned char > MobileClient::get_song_bytes( const string_t& song_id ) {
	return GetSongBytesCall( song_id ).make_call( this->androidClient ).get( );
}