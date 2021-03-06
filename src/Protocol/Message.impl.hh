/*	Copyright 2018 (c) Michael Thomas (malinka) <malinka@entropy-development.com>
	Distributed under the terms of the GNU Affero General Public License v3
*/

#if !defined ENTROPY_TETHYS_MESSAGE_PROTOCOL_IMPL
#	define ENTROPY_TETHYS_MESSAGE_PROTOCOL_IMPL

#	include "Message.hh"
#	include <boost/algorithm/string.hpp>
#	include <boost/lexical_cast.hpp>

	namespace Entropy
	{
		namespace Tethys
		{
			namespace Protocol
			{
				template<typename charT, typename traits>
				Message<charT, traits>::Message()
					: _headers(), _body(), _has_body(false)
				{}

				template<typename charT, typename traits> Message<charT, traits>::Message(const Message<charT, traits> &) = default;
				template<typename charT, typename traits> Message<charT, traits>::Message(Message<charT, traits> &&) = default;
				template<typename charT, typename traits> Message<charT, traits>::~Message() = default;

				template<typename charT, typename traits>
				void Message<charT, traits>::addHeader(string_type &&line)
				{
					// 2018-02-16 AMR TODO: optimization
					std::vector<string_type> pieces;
					boost::split(pieces, line, boost::is_any_of(":"));

					if(
						pieces.size() < 2 ||
						pieces[0] == "" || pieces[1] == "" ||
						std::isspace(pieces[0][0])
					) {
						ENTROPY_THROW(Exception("Malformed Header in Message") <<
							HeaderLineInfo(line)
						);
					} else if(pieces.size() > 2) {
						auto i = pieces.begin();
						i += 2;
						while(i != pieces.end()) {
							pieces[1] += ":" + std::move(*i++);
						}

						pieces.resize(2);
					}

					boost::trim(pieces[1]);

					try
					{
						// 2018-02-19 AMR NOTE: some include the \r\n and some don't
						// 2018-02-19 AMR TODO: check standard
						// 2018-02-21 AMR TODO: should this be here or in HttpMessage overload
						if(pieces[0] == "Content-Length") {
							_body.reserve(boost::lexical_cast<std::size_t>(pieces[1]));
							ENTROPY_LOG(Log, Severity::Debug) << "body capacity set to " << _body.capacity();
						}
					}
					catch(boost::bad_lexical_cast &)
					{
						ENTROPY_LOG(Log, Severity::Warning) << "failed to convert Content-Length to size_t (" << pieces[1] << ")";
					}

					Headers()[std::move(pieces[0])] = std::move(pieces[1]);
				}

				template<typename charT, typename traits>
				void Message<charT, traits>::addBody(string_type &&s)
				{
					_has_body = true;
					_body = std::move(s);
				}

				template<typename charT, typename traits>
				void Message<charT, traits>::addBody(std::basic_istream<charT, traits> &s)
				{
					_has_body = true;

					auto start = s.tellg();
					auto sz = s.rdbuf()->in_avail();

					s.seekg(-2, std::ios_base::end);
					if(s.peek() == '\r') {
						s.seekg(1, std::ios_base::cur);
						if(s.peek() == '\n')
							sz -= 2;
					}

					s.seekg(start);
					auto osz = Body().size();
					Body().resize(sz + osz);
					ENTROPY_LOG(Log, Severity::Debug) << "body size set to " << _body.size();
					s.read(Body().data() + osz, sz);
				}

				template<typename charT, typename traits>
				bool Message<charT, traits>::hasBody() const
				{
					return _has_body;
				}

				template<typename charT, typename traits>
				typename Message<charT, traits>::string_type &Message<charT, traits>::Body()
				{
					return _body;
				}

				template<typename charT, typename traits>
				const typename Message<charT, traits>::string_type &Message<charT, traits>::Body() const
				{
					return _body;
				}

				template<typename charT, typename traits>
				typename Message<charT, traits>::container_type &Message<charT, traits>::Headers()
				{
					return _headers;
				}

				template<typename charT, typename traits>
				const typename Message<charT, traits>::container_type &Message<charT, traits>::Headers() const
				{
					return _headers;
				}
			}
		}
	}

	template<typename charT, typename traits>
	std::basic_ostream<charT, traits> &operator << (std::basic_ostream<charT, traits> &o, const Entropy::Tethys::Protocol::Message<charT, traits> &m)
	{
		for(auto &&p : m.Headers())
			o << p.first << ": " << p.second << "\r\n";

		o << "\r\n";
		if(m.hasBody())
			o << m.Body() << "\r\n";

		return o;
	}

#endif
