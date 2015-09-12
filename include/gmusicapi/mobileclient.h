
#ifndef _MOBILECLIENT_H_
#define _MOBILECLIENT_H_

#include <string>
#include <vector>

#include "cpprest/http_client.h"

#include "boost/generator_iterator.hpp"

#include "gmusicapi/types.h"
#include "gmusicapi/song.h"
#include "gmusicapi/generator.hpp"

namespace gmusicapi {

	class MobileClient {
	private:

		bool isAuthenticated;

		web::http::client::http_client sjClient;
		web::http::client::http_client androidClient;

	public:

		MobileClient( );

		bool login( const string_t& email, const string_t& password, const string_t& androidID );

		std::vector< gmusicapi::Song > get_all_songs( );

		GeneratorIterator< Song > get_all_tracks( unsigned int page_size = 10000 );

		std::vector< unsigned char > get_song_bytes( const string_t& song_id );

	};

}

#endif