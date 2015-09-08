#pragma once

#include <string>

#include "cpprest/json.h"

#define GETTER( prop ) decltype( prop ) get_##prop( ) { return this->##prop; }

namespace gmusicapi {

	class Song {

		std::string album;
		std::string title;
		std::string genre;
		std::string artist;

	public:

		Song( const web::json::value& json );

		GETTER( album );
		GETTER( title );
		GETTER( genre );
		GETTER( artist );

	};

}