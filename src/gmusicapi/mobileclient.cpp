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
	auto useragent_handler = [ ]( http_request request, std::shared_ptr< http_pipeline_stage >& handler ) {
		request.headers( ).add( U( "User-Agent" ), U( "gmusicapi-cpp/mobileclient" ) );

		return handler->propagate( request );
	};
	sjClient.add_handler( useragent_handler );
	androidClient.add_handler( useragent_handler );
}

bool MobileClient::login( const string_t& email, const string_t& password, const string_t& androidID ) {
	if( this->isAuthenticated ) {
		return false;
	}

	map< string_t, string_t > res;

	res = LoginCall( email, password, androidID ).make_call( this->androidClient ).get( );
	auto itr = res.find( U( "Token" ) );
	if( itr == res.end( ) ) {
		return false;
	}

	
	res = OAuthCall( itr->second, androidID ).make_call( this->androidClient ).get( );
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


TrackIterator MobileClient::get_all_tracks( unsigned int page_size ) {
	return TrackIterator( this->sjClient, page_size );
}

pplx::task< cibytestream > MobileClient::get_song_stream( const string_t& song_id ) {
	return GetSongStreamCall( song_id ).make_call( this->androidClient );
}

pplx::task< vector< uint8_t > > MobileClient::get_song_bytes( const string_t& song_id ) {
	return this->get_song_stream( song_id ).then( [ ]( cibytestream& is ) {
		cbytebuf buffer;
		is.read_to_end( buffer ).wait( );

		return buffer.collection( );
	} );
}

json::value MobileClient::get_registered_devices( ) {
	return GetDeviceInfoCall( ).make_call( this->sjClient ).get( );
}