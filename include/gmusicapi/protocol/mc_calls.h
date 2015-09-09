
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

		template< typename CALL_TYPE >
		class AuthenticatedCall : public Call< CALL_TYPE > {
		private:

			const utility::string_t oauthToken;

		public:

			AuthenticatedCall( const string_t& oauthToken )
				: oauthToken( oauthToken) { }

			void get_headers( web::http::http_headers& headers ) {
				static string_t header_name = U( "Authorization" );

				stringstream_t ss;
				ss << "GoogleLogin auth=" << this->oauthToken;

				headers.add( header_name, ss.str( ) );
			}

		};

		class ListTracksCall : public AuthenticatedCall< ListTracksCall > {
		public:

			ListTracksCall( const string_t& oauthToken );

			web::http::method method( );
			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			web::json::value parse_response( const web::http::http_response& res );

		};

	}
}

#endif