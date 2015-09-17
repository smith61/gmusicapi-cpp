
#ifndef _MC_CALLS_H_
#define _MC_CALLS_H_

#include <vector>

#include "gmusicapi/protocol/call.hpp"
#include "gmusicapi/types.h"

namespace gmusicapi {
	namespace protocol {
		
		class LoginCall : public PostCall< LoginCall > {
		private:

			const string_t email;
			const string_t password;
			const string_t androidID;

		public:

			LoginCall( const string_t& email, const string_t& password, const string_t& androidID );

			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			std::map< string_t, string_t > parse_response( const web::http::http_response& res );

		};

		class OAuthCall : public PostCall< OAuthCall > {
		private:

			const string_t masterToken;
			const string_t androidID;

		public:

			OAuthCall( const string_t& masterToken, const string_t& androidID );

			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			std::map< string_t, string_t > parse_response( const web::http::http_response& res );
		};

		class ListTracksCall : public PostCall< ListTracksCall > {
		private:

			size_t max_results;
			const string_t page_token;

		public:

			ListTracksCall( size_t max_results, const string_t& page_token = U( "" ) );

			utility::string_t get_endpoint( );

			void set_body( web::http::http_request& req );
			web::json::value parse_response( const web::http::http_response& res );

		};

		class GetSongStreamCall : public GetCall< GetSongStreamCall > {
		private:

			const string_t song_id;

		public:

			GetSongStreamCall( const string_t& song_id );

			utility::string_t get_endpoint( );

			void get_query_params( std::map< string_t, string_t >& query_params );
			cibytestream parse_response( const web::http::http_response& res );

		};

		class GetDeviceInfoCall : public GetCall< GetDeviceInfoCall > {
		public:

			utility::string_t get_endpoint( );

			web::json::value parse_response( const web::http::http_response& res );

		};
	}
}

#endif