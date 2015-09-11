
#ifndef _MC_CALLS_H_
#define _MC_CALLS_H_

#include "gmusicapi/protocol/call.hpp"
#include "gmusicapi/types.h"

namespace gmusicapi {
	namespace protocol {
		
		class LoginCall : public Call< LoginCall > {
		private:

			const string_t email;
			const string_t password;
			const string_t androidID;

		public:

			LoginCall( const string_t& email, const string_t& password, const string_t& androidID );

			web::http::method method( );
			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			std::map< string_t, string_t > parse_response( const web::http::http_response& res );

		};

		class OAuthCall : public Call< OAuthCall > {
		private:

			const string_t masterToken;
			const string_t androidID;

		public:

			OAuthCall( const string_t& masterToken, const string_t& androidID );

			web::http::method method( );
			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			std::map< string_t, string_t > parse_response( const web::http::http_response& res );
		};

		class ListTracksCall : public Call< ListTracksCall > {
		private:

			size_t max_results;
			const string_t& page_token;

		public:

			ListTracksCall( size_t max_results, const string_t& page_token = U( "" ) );

			web::http::method method( );
			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			web::json::value parse_response( const web::http::http_response& res );

		};

	}
}

#endif