#include "gmusicapi/song.h"

using namespace std;

using namespace web;
using namespace utility::conversions;

using namespace gmusicapi;

static string get_string( const json::value& obj, const string& key, const string& def ) {
	if( !obj.has_field( to_string_t( key ) ) ) { 
		return def;
	}

	json::value val = obj.at( to_string_t( key ) );
	if( val.is_null( ) ) {
		return def;
	}
	return to_utf8string( val.as_string( ) );
}

Song::Song( const json::value& value ) {
	this->album = get_string( value, "album", "Unknown" );
	this->artist = get_string( value, "artist", "Unknown" );
	this->genre = get_string( value, "genre", "Unknown" );
	this->title = get_string( value, "title", "Unknown" );
}