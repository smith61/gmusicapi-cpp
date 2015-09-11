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
	: isAuthenticated( false ), sjClient( U( "https://www.googleapis.com/sj/v1.11" ) ) {
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
	sjClient.add_handler( [ oauthToken ]( http_request request, std::shared_ptr< http_pipeline_stage >& handler ) {
		static string_t header_name = U( "Authorization" );

		stringstream_t ss;
		ss << "GoogleLogin auth=" << oauthToken;

		request.headers( ).add( header_name, ss.str( ) );

		return handler->propagate( request );
	} );
	this->isAuthenticated = true;

	return true;
}

vector< Song > MobileClient::get_all_songs( ) {
	vector< Song > ret;
	json::value val = ListTracksCall( ).make_call( this->sjClient ).get( );
	val = val.at( U( "data" ) ).at( U( "items" ) );

	if( val.is_array( ) ) {
		json::array songs = val.as_array( );
		for( auto song : songs ) {
			ret.push_back( Song( song ) );
		}
	}

	return ret;
}


namespace {
	class TrackGenerator : public Generator< Song* > {
	private:

		http_client& client;
		const unsigned int page_size;

		vector< Song > current_page;
		unsigned int page_offset;

	public:

		TrackGenerator( http_client& client, unsigned int page_size )
			: client( client ), page_size( page_size ), current_page( ), page_offset( 0 ) {
			
			json::value val = ListTracksCall( ).make_call( client ).get( );
			val = val[ U( "data" ) ][ U( "items" ) ];

			if( val.is_array( ) ) {
				for( auto song : val.as_array( ) ) {
					this->current_page.push_back( Song( song ) );
				}
			}
		}

		virtual ~TrackGenerator( ) { }

		virtual bool hasNext( ) override {
			return this->page_offset != this->current_page.size( );
		}

		virtual Song* next( ) override {
			return &current_page[ page_offset++ ];
		}

		virtual Generator< Song* >* clone( ) override {
			TrackGenerator* n = new TrackGenerator( client, page_size );
			n->current_page = this->current_page;
			n->page_offset = this->page_offset;

			return n;
		}

	};
}

GeneratorIterator< Song* > MobileClient::get_all_tracks( unsigned int page_size ) {
	return GeneratorIterator< Song* >( new TrackGenerator( this->sjClient, page_size ) );
}