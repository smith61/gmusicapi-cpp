
#ifndef _MOBILECLIENT_H_
#define _MOBILECLIENT_H_

#include <string>
#include <vector>

#include "cpprest/http_client.h"

#include "gmusicapi/types.h"
#include "gmusicapi/song.h"

namespace gmusicapi {

	class MobileClient {
	private:

		bool isAuthenticated;
		string_t oauthToken;

		web::http::client::http_client sjClient;

	public:

		MobileClient( );

		bool login( const string_t& email, const string_t& password, const string_t& androidID );

		std::vector< gmusicapi::Song > get_all_songs( );

	};

}

#endif