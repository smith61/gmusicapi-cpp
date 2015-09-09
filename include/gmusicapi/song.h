#pragma once

#include "gmusicapi/types.h"

#include "cpprest/json.h"

#define GETTER( prop ) decltype( prop ) get_##prop( ) { return this->##prop; }

namespace gmusicapi {

	class Song {

		string_t album;
		string_t title;
		string_t genre;
		string_t artist;

	public:

		Song( const web::json::value& json );

		GETTER( album );
		GETTER( title );
		GETTER( genre );
		GETTER( artist );

	};

}