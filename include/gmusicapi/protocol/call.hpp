
#ifndef _CALL_H_
#define _CALL_H_

#include <map>

#include "cpprest/http_client.h"
#include "cpprest/http_headers.h"
#include "cpprest/uri.h"
#include "cpprest/uri_builder.h"

#include "gmusicapi/types.h"

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

				CALL_TYPE* self = this->as_call_type( );

				uri_builder builder;
				map< string_t, string_t > query_params;
				
				builder.set_path( self->get_endpoint( ) );

				self->get_query_params( query_params );

				for( const auto& itr : query_params ) {
					builder.append_query( itr.first, itr.second );
				}

				return builder.to_uri( );
			}

		public:

			decltype( auto ) make_call( web::http::client::http_client& client ) {
				using namespace std;
				using namespace pplx;
				using namespace web;
				using namespace web::http;

				CALL_TYPE* self = this->as_call_type( );

				http_request request;

				request.set_method( self->method( ) );
				request.set_request_uri( this->make_uri( ) );
				self->get_headers( request.headers( ) );
				self->set_body( request );

				return client.request( request ).then( [self]( task< http_response > res ) {
					return self->parse_response( res.get( ) );
				} );
			}

			void get_query_params( std::map< string_t, string_t >& query_params ) { }

			void get_headers( web::http::http_headers& headers ) { }

			void set_body( web::http::http_request& request ) { }

		};

	}
}

#endif