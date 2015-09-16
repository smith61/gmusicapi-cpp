
#ifndef _TYPES_H_
#define _TYPES_H_

#include <vector>

#include "cpprest/details/basic_types.h"

#include "cpprest/streams.h"
#include "cpprest/astreambuf.h"
#include "cpprest/containerstream.h"

namespace gmusicapi {

	// Import types from utility namespace
	using utility::char_t;
	using utility::ifstream_t;
	using utility::istream_t;
	using utility::istringstream_t;
	using utility::ofstream_t;
	using utility::ostream_t;
	using utility::ostringstream_t;
	using utility::size64_t;
	using utility::stringstream_t;
	using utility::string_t;

	typedef concurrency::streams::istream cibytestream;
	typedef concurrency::streams::container_buffer< std::vector< uint8_t > > cbytebuf;

}

#endif