#pragma once

#include <map>

#include "cpprest/http_client.h"
#include "cpprest/http_headers.h"
#include "cpprest/uri.h"
#include "cpprest/uri_builder.h"

namespace gmusicapi {
	namespace protocol {

		template< typename CALL_TYPE >
		class Call {
		private:

			CALL_TYPE* as_call_type( ) {
				return static_cast< CALL_TYPE* >( this );
			}

			web::uri make_uri( ) {
				using namespace std;
				using namespace web;
				using namespace utility;

				CALL_TYPE* self = this->as_call_type( );

				uri_builder builder;
				map< string_t, string_t > query_params;
				
				builder.set_path( self->get_endpoint( ) );

				self->get_query_params( query_params );

				for( auto itr : query_params ) {
					builder.append_query( itr.first, itr.second );
				}

				return builder.to_uri( );
			}

		public:

			decltype( auto ) make_call( web::http::client::http_client& client ) {
				using namespace std;
				using namespace web;
				using namespace web::http;
				using namespace utility;

				CALL_TYPE* self = this->as_call_type( );

				http_request request;

				request.set_method( self->method( ) );
				request.set_request_uri( this->make_uri( ) );
				self->get_headers( request.headers( ) );
				self->set_body( request );

				const http_response response = client.request( request ).get( );

				return self->parse_response( response );
			}

			void get_query_params( std::map< utility::string_t, utility::string_t > query_params ) { }

			void get_headers( web::http::http_headers& headers ) { }

			void set_body( web::http::http_request& request ) { }

		};

	}
}