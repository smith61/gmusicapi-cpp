#pragma once

#include <string>
#include <vector>

#include "cpprest/http_client.h"

#include "song.h"

namespace gmusicapi {

	class MobileClient {
	private:

		bool isAuthenticated;
		std::string oauthToken;

		web::http::client::http_client sjClient;

	public:

		MobileClient( );

		bool login( const std::string& email, const std::string& password, const std::string& androidID );

		std::vector< gmusicapi::Song > get_all_songs( );

	};

}