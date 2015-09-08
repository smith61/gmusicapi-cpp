#pragma once

#include "call.hpp"

namespace gmusicapi {
	namespace protocol {
		
		class LoginCall : public Call< LoginCall > {
		private:

			const utility::string_t email;
			const utility::string_t password;
			const utility::string_t androidID;

		public:

			LoginCall( const std::string& email, const std::string& password, const std::string& androidID );

			web::http::method method( );
			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			std::map< std::string, std::string > parse_response( const web::http::http_response& res );

		};

		class OAuthCall : public Call< OAuthCall > {
		private:

			const utility::string_t masterToken;
			const utility::string_t androidID;

		public:

			OAuthCall( const std::string& masterToken, const std::string& androidID );

			web::http::method method( );
			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			std::map< std::string, std::string > parse_response( const web::http::http_response& res );
		};

		template< typename CALL_TYPE >
		class AuthenticatedCall : public Call< CALL_TYPE > {
		private:

			const utility::string_t oauthToken;

		public:

			AuthenticatedCall( const std::string& oauthToken )
				: oauthToken( utility::conversions::to_string_t( oauthToken ) ) { }

			void get_headers( web::http::http_headers& headers ) {
				using namespace utility;

				static string_t header_name = U( "Authorization" );

				stringstream_t ss;
				ss << "GoogleLogin auth=" << this->oauthToken;

				headers.add( header_name, ss.str( ) );
			}

		};

		class ListTracksCall : public AuthenticatedCall< ListTracksCall > {
		public:

			ListTracksCall( const std::string& oauthToken );

			web::http::method method( );
			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			web::json::value parse_response( const web::http::http_response& res );

		};

	}
}