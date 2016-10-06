/*	Copyright 2016 (c) Michael Thomas (malinka) <malinka@entropy-development.com>
	Distributed under the terms of the GNU Affero General Public License v3
*/

#if !defined ENTROPY_ASIO_STREAMBUFFER_INC
#	define ENTROPY_ASIO_STREAMBUFFER_INC

#	include <streambuf>
#	include <list>

#	include "Exception.hh"
#	include "Buffer.hh"

#	ifndef ENTROPY_ASIO_STREAMBUFFER_WRITE_BUFFER_SIZE
#		define ENTROPY_ASIO_STREAMBUFFER_WRITE_BUFFER_SIZE 4096
#	endif

	namespace Entropy
	{
		namespace Asio
		{
			template<
				typename Stream,
				typename charT = char,
				typename traits = std::char_traits<charT>
			>
			class StreamBuffer :
				public std::basic_streambuf<charT, traits>
			{
				public:
					StreamBuffer(Stream &);
					~StreamBuffer();
					std::streamsize Available() const;
				private:
					typename traits::pos_type seekpos(typename traits::pos_type, std::ios_base::openmode = std::ios_base::in | std::ios_base::out);
					typename traits::pos_type seekoff(typename traits::off_type, std::ios_base::seekdir, std::ios_base::openmode = std::ios_base::in | std::ios_base::out);
					typename traits::int_type underflow();
					std::streamsize showmanyc();
				private:
					Stream &_stream;
					std::list<Buffer<charT>> _buffers;
					typename std::list<Buffer<charT>>::iterator _current;
					Buffer<charT> *_write;
					bool _is_seek;
					static constexpr std::size_t write_buffer_size = ENTROPY_ASIO_STREAMBUFFER_WRITE_BUFFER_SIZE;
			};
		}
	}

#	include "StreamBuffer.impl.hh"

#endif