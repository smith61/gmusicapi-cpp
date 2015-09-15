
#ifndef _GENERATOR_HPP_
#define _GENERATOR_HPP_

#include <memory>
#include "boost/iterator_adaptors.hpp"

namespace gmusicapi {

	// Similar to boost::generator_iterator but provides GC of generators
	//  and has_next support
	template< typename GENERATOR_TYPE >
	class Generator
		: public boost::iterator_facade<
					Generator< GENERATOR_TYPE >,
					typename GENERATOR_TYPE::result_type,
					boost::single_pass_traversal_tag,
					typename GENERATOR_TYPE::result_type const& > {
	private:
		GENERATOR_TYPE instance;

		bool valid;
		typename GENERATOR_TYPE::result_type cur_val;

	public:

		template< typename... ARGS >
		Generator( ARGS&&... args )
			: instance( std::forward< ARGS >( args )... ), valid( true ) {
			( *this )++;
		}

		Generator( const Generator& ) = delete;
		Generator& operator=( const Generator& ) = delete;

		Generator( Generator&& o )
			: instance( std::move( o.instance ) ), valid( o.valid ), cur_val( std::move( o.cur_val ) ) {
			o.instance = nullptr;
			o.valid = false;
		}

		Generator& operator=( Generator&& o ) {
			std::swap( this->instance, o.instance );
			std::swap( this->valid, o.valid );
			std::swap( this->cur_val, o.cur_val );

			return *this;
		}

		void increment( ) {
			if( instance.has_next( ) ) {
				this->cur_val = instance( );
			}
			else {
				this->valid = false;
			}
		}

		const typename GENERATOR_TYPE::result_type& dereference( ) const {
			return this->cur_val;
		}

		bool equal( Generator const& y ) const {
			return this->instance == y.instance && this->cur_val == y.cur_val;
		}

		bool has_next( ) {
			return this->valid;
		}

		operator bool( ) {
			return this->has_next( );
		}

	};

}

#endif