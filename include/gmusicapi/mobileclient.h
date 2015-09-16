
#ifndef _MOBILECLIENT_H_
#define _MOBILECLIENT_H_

#include <string>
#include <vector>

#include "cpprest/http_client.h"

#include "gmusicapi/types.h"
#include "gmusicapi/song.h"
#include "gmusicapi/generator.hpp"

namespace gmusicapi {

	class TrackGenerator {
	private:

		web::http::client::http_client& client;
		unsigned int page_size;

		std::vector< Song > current_page;

		string_t next_page_token;

		void next_page( );

	public:

		typedef Song result_type;

		TrackGenerator( web::http::client::http_client& client, unsigned int page_size );

		TrackGenerator( const TrackGenerator& ) = delete;
		TrackGenerator& operator=( const TrackGenerator& ) = delete;

		TrackGenerator( TrackGenerator&& o );
		TrackGenerator& operator=( TrackGenerator&& o );

		result_type operator( )( );

		bool has_next( );

	};

	class MobileClient {
	private:

		bool isAuthenticated;

		web::http::client::http_client sjClient;
		web::http::client::http_client androidClient;

	public:

		MobileClient( );

		bool login( const string_t& email, const string_t& password, const string_t& androidID );

		std::vector< gmusicapi::Song > get_all_songs( );

		Generator< TrackGenerator > get_all_tracks( unsigned int page_size = 10000 );

		std::vector< unsigned char > get_song_bytes( const string_t& song_id );

		web::json::value get_registered_devices( );

	};

}

#endif