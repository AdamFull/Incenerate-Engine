#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace sjson
{
	enum class esjson_object_type
	{
		e_object,
		e_integer,
		e_double,
		e_boolean,
		e_string,
		e_array
	};

	struct sjson_object
	{
		int64_t _integer{ 0ll };
		double _double{ 0.0 };
		bool _boolean{ false };
		std::string _string;
		std::vector<sjson_object> _array;
		std::unordered_map<std::string, sjson_object> _objects;
		esjson_object_type _type{ esjson_object_type::e_object };

		template<class _Ty>
		void parse_to(_Ty& value) const
		{
			deserialize(*this, value);
		}

		template<class _Ty>
		void try_parse_to(const std::string& key, _Ty& value, bool expect = false) const
		{
			auto found = _objects.find(key);
			if (found != _objects.end())
			{
				deserialize(found->second, value);
				return;
			}

			if (expect)
				throw std::runtime_error("Expected value not exists.");
		}
	};

	namespace parsing
	{
		enum class esjson_token_type
		{
			eUnknown,
			eIdentifier,
			eString,
			eInteger,
			eDouble,
			eBoolean,
			eLBrace,
			eRBrace,
			eLBracket,
			eRBracket,
			eColon,
			eSemicolon,
			eComma,
			eEnd
		};

		struct sjson_token
		{
			esjson_token_type type;
			std::string value;
		};

		inline sjson_token make_token(esjson_token_type type, const std::string& value)
		{
			return sjson_token{ type, value };
		}

		class sjson_tokenizer
		{
		public:
			sjson_tokenizer(const std::string& input) : content(input) {}

			std::vector<sjson_token> tokenize()
			{
				std::vector<sjson_token> tokens;

				while (pos < content.size())
				{
					skip_whitespaces_and_comments();

					if (pos >= content.size())
						break;

					if (isalpha(content[pos]) || content[pos] == '_')
					{
						std::string lookahead = content.substr(pos, 5);
						if (lookahead.find("true") != std::string::npos || lookahead == "false")
							tokens.emplace_back(parse_boolean());
						else
							tokens.emplace_back(make_token(esjson_token_type::eIdentifier, parse_identifier()));
					}
					else if (content[pos] == '"')
					{
						if (pos + 2 < content.size() && content.substr(pos, 3) == "\"\"\"")
							tokens.emplace_back(make_token(esjson_token_type::eString, parse_multiline_string()));
						else
							tokens.emplace_back(make_token(esjson_token_type::eString, parse_string()));
					}
					else if (isdigit(content[pos]) || (content[pos] == '-' && isdigit(content[pos + 1])))
						tokens.emplace_back(parse_number());
					else
					{
						switch (content[pos]) {
						case '{': tokens.emplace_back(make_token(esjson_token_type::eLBrace, "{")); pos++; break;
						case '}': tokens.emplace_back(make_token(esjson_token_type::eRBrace, "}")); pos++; break;
						case '[': tokens.emplace_back(make_token(esjson_token_type::eLBracket, "[")); pos++; break;
						case ']': tokens.emplace_back(make_token(esjson_token_type::eRBracket, "]")); pos++; break;
						case ':': tokens.emplace_back(make_token(esjson_token_type::eColon, ":")); pos++; break;
						case ';': tokens.emplace_back(make_token(esjson_token_type::eSemicolon, ";")); pos++; break;
						case ',': tokens.emplace_back(make_token(esjson_token_type::eComma, ",")); pos++; break;
						default: tokens.emplace_back(make_token(esjson_token_type::eUnknown, std::string(1, content[pos]))); pos++; break;
						}
					}
				}

				tokens.emplace_back(make_token(esjson_token_type::eEnd, ""));
				return tokens;
			}
		private:
			void skip_single_line_comment()
			{
				while (pos < content.size() && content[pos] != '\n')
					pos++;
			}

			void skip_multi_line_comment()
			{
				while (pos + 1 < content.size() && (content[pos] != '*' || content[pos + 1] != '/'))
					pos++;
				pos += 2;  // Skip */
			}

			void skip_comments()
			{
				while (pos + 1 < content.size())
				{
					if (content[pos] == '/' && content[pos + 1] == '/')
						skip_single_line_comment();
					else if (content[pos] == '/' && content[pos + 1] == '*')
						skip_multi_line_comment();
					else
						break;
				}
			}

			void skip_whitespaces_and_comments()
			{
				while (pos < content.size())
				{
					if (isspace(content[pos]))
						pos++;
					else if (pos + 1 < content.size() && (content[pos] == '/' && (content[pos + 1] == '/' || content[pos + 1] == '*')))
						skip_comments();
					else
						break;
				}
			}

			std::string parse_identifier()
			{
				size_t start = pos;
				while (pos < content.size() && (isalnum(content[pos]) || content[pos] == '_'))
					pos++;

				return content.substr(start, pos - start);
			}

			std::string parse_string()
			{
				std::ostringstream out;
				pos++;  // Skip initial "
				while (pos < content.size() && content[pos] != '"')
				{
					out << content[pos];
					pos++;
				}
				pos++;  // Skip final "
				return out.str();
			}

			std::string parse_multiline_string()
			{
				std::ostringstream out;
				pos += 3;  // Skip initial """

				while (pos < content.size() && (pos + 2 < content.size() && (content.substr(pos, 3) != "\"\"\"")))
				{
					out << content[pos];
					pos++;
				}

				if (pos + 2 < content.size() && content.substr(pos, 3) == "\"\"\"")
					pos += 3;  // Skip final """

				return out.str();
			}

			sjson_token parse_number()
			{
				size_t start = pos;
				while (pos < content.size() && (isdigit(content[pos]) || content[pos] == '.'))
					pos++;

				std::string number = content.substr(start, pos - start);
				if (number.find('.') != std::string::npos)
					return make_token(esjson_token_type::eDouble, number);

				return make_token(esjson_token_type::eInteger, number);
			}

			sjson_token parse_boolean()
			{
				if (content.substr(pos, 4) == "true")
				{
					pos += 4;
					return make_token(esjson_token_type::eBoolean, "true");
				}
				else if (content.substr(pos, 5) == "false")
				{
					pos += 5;
					return make_token(esjson_token_type::eBoolean, "false");
				}

				return make_token(esjson_token_type::eUnknown, "");
			}
		private:
			std::string content;
			size_t pos{ 0ull };
		};

		class sjson_parser
		{
		public:
			sjson_parser(const std::string& input)
			{
				sjson_tokenizer tokenizer(input);
				tokens = tokenizer.tokenize();
			}

			sjson_object parse()
			{
				return parse_value();
			}
		private:
			sjson_object parse_value()
			{
				sjson_object object{};

				auto& current = peek();
				switch (current.type)
				{
				case esjson_token_type::eLBrace:
				{
					object = parse_object();
					object._type = esjson_object_type::e_object;
				} break;
				case esjson_token_type::eLBracket:
				{
					object = parse_array();
					object._type = esjson_object_type::e_array;
				} break;
				case esjson_token_type::eString:
				{
					object._string = expect(esjson_token_type::eString).value;
					object._type = esjson_object_type::e_string;
				} break;
				case esjson_token_type::eInteger:
				{
					object._integer = std::stoll(expect(esjson_token_type::eInteger).value);
					object._type = esjson_object_type::e_integer;
				} break;
				case esjson_token_type::eDouble:
				{
					object._double = std::stod(expect(esjson_token_type::eDouble).value);
					object._type = esjson_object_type::e_double;
				} break;
				case esjson_token_type::eBoolean:
				{
					object._boolean = expect(esjson_token_type::eBoolean).value == "true";
					object._type = esjson_object_type::e_boolean;
				} break;
				default:
					throw std::runtime_error("Unexpected token found.");
					break;
				}

				return object;
			}

			sjson_object parse_object()
			{
				sjson_object object;

				expect(esjson_token_type::eLBrace);
				while (peek().type != esjson_token_type::eRBrace)
				{
					std::string key = expect(esjson_token_type::eIdentifier).value;
					expect(esjson_token_type::eColon);
					object._objects[key] = parse_value();

					if (peek().type != esjson_token_type::eRBrace)
						expect(esjson_token_type::eSemicolon);
				}

				expect(esjson_token_type::eRBrace);
				return object;
			}

			sjson_object parse_array()
			{
				sjson_object object;

				expect(esjson_token_type::eLBracket);
				while (peek().type != esjson_token_type::eRBracket)
				{
					object._array.emplace_back(parse_value());

					if (peek().type != esjson_token_type::eRBracket)
						expect(esjson_token_type::eComma);
				}

				expect(esjson_token_type::eRBracket);
				return object;
			}

			sjson_token& peek()
			{
				return tokens[pos];
			}

			sjson_token& consume()
			{
				return tokens[pos++];
			}

			sjson_token& expect(esjson_token_type token)
			{
				auto& current = consume();
				if (current.type != token)
					std::runtime_error("Unexpected token type.");

				return current;
			}
		private:
			std::vector<sjson_token> tokens;
			size_t pos{ 0ull };
		};
	}

	inline sjson_object parse(const std::string& data)
	{
		return parsing::sjson_parser(data).parse();
	}

	inline void serialize(sjson_object& sjson, const int& value);

	// Default integer deserialization
	inline void deserialize(const sjson_object& sjson, int8_t& value) { value = static_cast<int8_t>(sjson._integer); }
	inline void deserialize(const sjson_object& sjson, uint8_t& value) { value = static_cast<uint8_t>(sjson._integer); }
	inline void deserialize(const sjson_object& sjson, int16_t& value) { value = static_cast<int16_t>(sjson._integer); }
	inline void deserialize(const sjson_object& sjson, uint16_t& value) { value = static_cast<uint16_t>(sjson._integer); }
	inline void deserialize(const sjson_object& sjson, int32_t& value) { value = static_cast<int32_t>(sjson._integer); }
	inline void deserialize(const sjson_object& sjson, uint32_t& value) { value = static_cast<uint32_t>(sjson._integer); }
	inline void deserialize(const sjson_object& sjson, int64_t& value) { value = static_cast<int64_t>(sjson._integer); }
	inline void deserialize(const sjson_object& sjson, uint64_t& value) { value = static_cast<uint64_t>(sjson._integer); }

	// Default floating point deserialization
	inline void deserialize(const sjson_object& sjson, float& value) { value = static_cast<float>(sjson._double); }
	inline void deserialize(const sjson_object& sjson, double& value) { value = sjson._double; }

	// Default bool serialization
	inline void deserialize(const sjson_object& sjson, bool& value) { value = sjson._boolean; }

	// Default std types deserialization
	inline void deserialize(const sjson_object& sjson, std::string& value) { value = sjson._string; }

	template<class _Ty>
	inline void deserialize(const sjson_object& sjson, std::vector<_Ty>& value)
	{ 
		for (auto& object : sjson._array)
		{
			_Ty data;
			deserialize(object, data);
			value.emplace_back(data);
		}
	}
}