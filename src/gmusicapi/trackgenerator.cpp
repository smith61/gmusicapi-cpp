
#include "gmusicapi/mobileclient.h"
#include "gmusicapi/protocol/mc_calls.h"

using namespace std;

using namespace web;
using namespace web::http::client;

using namespace gmusicapi;
using namespace gmusicapi::protocol;

TrackGenerator::TrackGenerator( http_client& client, unsigned int page_size )
	: client( client ), page_size( page_size ), current_page( ) {
	this->next_page( );
}

bool TrackGenerator::has_next( ) {
	return !this->next_page_token.empty( ) || !this->current_page.empty( );
}

TrackGenerator::result_type TrackGenerator::operator( )( ) {
	if( !this->has_next( ) ) {
		throw "Illegal state.";
	}

	if( this->current_page.empty( ) ) {
		this->next_page( );
	}
	
	Song s = this->current_page[ this->current_page.size( ) - 1 ];
	this->current_page.pop_back( );
	return s;
}

void TrackGenerator::next_page( ) {
	this->current_page.clear( );
	json::value val = ListTracksCall( this->page_size, this->next_page_token ).make_call( this->client ).get( );

	json::value& next_page_token_js = val[ U( "nextPageToken" ) ];
	if( next_page_token_js.is_string( ) ) {
		this->next_page_token = next_page_token_js.as_string( );
	}
	else {
		this->next_page_token = U( "" );
	}

	json::value& songs_json = val[ U( "data" ) ][ U( "items" ) ];
	if( songs_json.is_array( ) ) {
		json::array& songs_array = songs_json.as_array( );
		for( auto itr = songs_array.rbegin( ); itr != songs_array.rend( ); itr++ ) {
			this->current_page.push_back( Song( *itr ) );
		}
	}
}