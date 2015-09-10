
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
		string_t oauthToken;

		web::http::client::http_client sjClient;

	public:

		MobileClient( );

		bool login( const string_t& email, const string_t& password, const string_t& androidID );

		template< typename CALL_TYPE, typename... ARG_TYPES >
		decltype( auto ) make_authed_call( ARG_TYPES&&... args ) {
			return CALL_TYPE( this->oauthToken, std::move( args )... ).make_call( this->sjClient );
		}

		std::vector< gmusicapi::Song > get_all_songs( );

		GeneratorIterator< Song* > get_all_tracks( unsigned int page_size = 10000 );

	};

}

#endif