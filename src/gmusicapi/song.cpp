#include "gmusicapi/song.h"

using namespace std;

using namespace web;

using namespace gmusicapi;

static string_t get_string( const json::value& obj, const string_t& key, const string_t& def ) {
	if( !obj.has_field( key ) ) {
		return def;
	}

	json::value val = obj.at( key );
	if( val.is_null( ) ) {
		return def;
	}
	return val.as_string( );
}

Song::Song( const json::value& value ) {
	this->album = get_string( value, U( "album" ), U( "Unknown" ) );
	this->artist = get_string( value, U( "artist" ), U( "Unknown" ) );
	this->genre = get_string( value, U( "genre" ), U( "Unknown" ) );
	this->title = get_string( value, U( "title" ), U( "Unknown" ) );
}