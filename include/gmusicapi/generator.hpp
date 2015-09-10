
#ifndef _GENERATOR_HPP_
#define _GENERATOR_HPP_

#include "boost/optional.hpp"

namespace gmusicapi {

	template< typename VALUE_TYPE >
	class Generator {
	public:

		virtual ~Generator( ) { }

		virtual bool hasNext( ) = 0;
		virtual VALUE_TYPE next( ) = 0;

		virtual Generator< VALUE_TYPE >* clone( ) = 0;

	};

	template< typename VALUE_TYPE >
	class GeneratorIterator {
	private:

		Generator< VALUE_TYPE > * generator;

		void checkNext( ) {
			if( !( *this ) ) {
				throw "Invalid generator state.";
			}
		}

	public:

		GeneratorIterator( Generator< VALUE_TYPE >* generator )
			: generator( generator ) { }

		GeneratorIterator( const GeneratorIterator< VALUE_TYPE >& rhs )
			: generator( rhs.generator->clone( ) ) {
		}

		GeneratorIterator( GeneratorIterator< VALUE_TYPE >&& rhs )
			: generator( rhs.generator ) {
			rhs.generator = nullptr;
		}

		~GeneratorIterator( ) {
			delete generator;
		}

		GeneratorIterator< VALUE_TYPE >& operator=( const GeneratorIterator< VALUE_TYPE >& rhs ) {
			delete generator;

			this->generator = rhs.generator->clone( );
			return *this;
		}

		operator bool( ){
			return this->generator->hasNext( );
		}

		VALUE_TYPE operator*( ) {
			this->checkNext( );

			return this->generator->next( );
		}

	};

}

#endif