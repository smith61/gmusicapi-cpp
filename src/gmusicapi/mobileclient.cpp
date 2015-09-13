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


namespace {
	class TrackGenerator : public Generator< Song > {
	private:

		http_client& client;
		const unsigned int page_size;

		bool has_next_page;
		string_t page_token;
		vector< Song > current_page;

		void get_next_page( ) {
			this->current_page.clear( );

			json::value val = ListTracksCall( this->page_size, page_token ).make_call( client ).get( );

			json::value nextPageToken = val[ U( "nextPageToken" ) ];
			if( nextPageToken.is_string( ) ) {
				this->page_token = nextPageToken.as_string( );
			}
			else {
				this->has_next_page = false;
			}

			val = val[ U( "data" ) ][ U( "items" ) ];

			if( val.is_array( ) ) {
				json::array& songs = val.as_array( );

				for( auto itr = songs.rbegin( ); itr != songs.rend( ); ++itr ) {
					this->current_page.push_back( Song( *itr ) );
				}
			}
		}

		void check_needs_next_page( ) {
			if( current_page.empty( ) && has_next_page ) {
				get_next_page( );
			}
		}

	public:

		TrackGenerator( http_client& client, unsigned int page_size )
			: client( client ), page_size( page_size ), page_token( ), current_page( ) {
			get_next_page( );
		}

		virtual ~TrackGenerator( ) { }

		virtual bool hasNext( ) override {
			return this->has_next_page || !this->current_page.empty( );
		}

		virtual Song next( ) override {
			this->check_needs_next_page( );

			Song s = current_page[ current_page.size( ) - 1 ];
			current_page.pop_back( );
			return s;
		}

		virtual Generator< Song >* clone( ) override {
			TrackGenerator* n = new TrackGenerator( client, page_size );
			n->current_page = this->current_page;

			return n;
		}

	};
}

GeneratorIterator< Song > MobileClient::get_all_tracks( unsigned int page_size ) {
	return GeneratorIterator< Song >( new TrackGenerator( this->sjClient, page_size ) );
}

vector< unsigned char > MobileClient::get_song_bytes( const string_t& song_id ) {
	return GetSongBytesCall( song_id ).make_call( this->androidClient ).get( );
}