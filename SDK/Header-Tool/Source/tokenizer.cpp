/*
Copyright © from 2024 to present, UNKNOWN STRYKER (Hojin Lee / Joey). All Rights Reserved.

Licensed under the Frogman Engine License (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	https://github.com/UnknownStryker-Interactive-Technologies/Frogman-Engine-License/blob/release/LICENSE.md

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "header_tool.hpp"
#include "context.hpp"
#include "file_io.hpp"
#include <preprocessor.hpp>
#include "tokenizer.hpp"




namespace FHT::tokenizer
{
	FE::boolean is_a_valid_letter_for_identifiers(FE::UTF8 char_p) noexcept
	{
		if (char_p == '_' ||
			(char_p >= 'a' && char_p <= 'z') ||
			(char_p >= 'A' && char_p <= 'Z') ||
			(char_p >= '0' && char_p <= '9')
			)
		{
			return true;
		}

		return false;
	}


	_FE_NODISCARD_ std::pmr::list<token> tokenize_header(file_buffer_t& file_p, const directory_t& path_p)
	{
		if (file_p.empty() == true)
		{
			throw FE::pair<FrogmanEngineHeaderToolError, FE::ASCII*>{FrogmanEngineHeaderToolError::_EmptyHeaderFile, "Warning, The header file is empty."};
		}

		context_stack_t l_context_stack{ framework::get_framework().get_memory_resource() };
		l_context_stack.reserve(64);
		l_context_stack.emplace_back(FHT::Context::_Global);

		std::pmr::list<token> l_list{ framework::get_framework().get_memory_resource() };

		auto l_end = file_p.c_str() + file_p.size();
		var::uint32 l_token_number = 1;

		FE_LOG_IF(path_p.empty(), FE::log::Severity::_Info, "The header file path is empty, transitioning to scope lexer mode.");
		for (FE::UTF8* iterator = path_p.empty() ? file_p.c_str() : FE::algorithm::string::skip_BOM(file_p.c_str()); iterator < l_end;)
		{
			if (*iterator <= ' ')
			{
				++iterator;
				continue;
			}


			token l_token = tokenize_identifiable(iterator, l_context_stack);
			if (l_token._code.size() == 0)
			{
				continue;
			}

			if (l_token._vocabulary == Vocabulary::_Macro)
			{
				auto l_macro_pos = iterator - file_p.c_str();
				file_p.replace(l_macro_pos, l_token._macro_identifier_length, l_token._code);
				iterator = file_p.c_str() + l_macro_pos;
				l_end = file_p.c_str() + file_p.size();
			}

			l_token._header_file_path = path_p.c_str();
			l_token._token_line_number = l_token_number;
			if (l_token._vocabulary != Vocabulary::_Undefined)
			{
				iterator += l_token._code.size(); // move to the next.
				l_list.push_back(std::move(l_token)); // push_back the defined vocab.

				if (iterator < l_end) _FE_LIKELY_
				{
					if (*iterator == '\n')
					{
						l_token._vocabulary = Vocabulary::_LineEnd;
						l_token._code = file_buffer_t(1, *iterator, framework::get_framework().get_memory_resource());
						l_token._header_file_path = path_p.c_str();
						l_token._token_line_number = l_token_number;

						l_list.push_back(std::move(l_token));
						++l_token_number; // Increment the line number.
						++iterator; // move to the next.
					}
				}
				continue;
			}


			l_token = tokenize_unidentifiable(iterator, l_context_stack);
			l_token._header_file_path = path_p.c_str();
			l_token._token_line_number = l_token_number;
			iterator += l_token._code.size(); // move to the next.
			l_list.push_back(std::move(l_token));

			if (iterator < l_end) _FE_LIKELY_
			{
				if (*iterator == '\n')
				{
					l_token._vocabulary = Vocabulary::_LineEnd;
					l_token._code = file_buffer_t(1, *iterator, framework::get_framework().get_memory_resource());
					l_token._header_file_path = path_p.c_str();
					l_token._token_line_number = l_token_number;

					l_list.push_back(std::move(l_token));
					++l_token_number; // Increment the line number.
					++iterator; // move to the next.
				}
			}
			continue;
		}

		l_list.emplace_back(Vocabulary::_EndOfCode, FE::null, l_token_number, u8"\0");
		return l_list;
	}


	// const char* p = "/* text */", f = "//text"; the 'text' is recognized as comments by FHT are purged from the token list.
	void purge_comments(std::pmr::list<token>& out_list_p) noexcept
	{
		for (auto it = out_list_p.begin(); it != out_list_p.end();) 
		{
			switch (it->_vocabulary)
			{
			case Vocabulary::_CommentBegin:
				_FE_FALLTHROUGH_;
			case Vocabulary::_CommentEnd:
				_FE_FALLTHROUGH_;
			case Vocabulary::_LineComment:
				_FE_FALLTHROUGH_;
			case Vocabulary::_CommentBody:
				{
					auto l_to_erase = it;
					++it;
					out_list_p.erase(l_to_erase);
				}
				continue;

			default:
				++it;
				continue;
			}
		}
	}

	void purge_preprocessor(std::pmr::list<token>& out_list_p) noexcept
	{
		for (auto it = out_list_p.begin(); it != out_list_p.end();)
		{
			switch (it->_vocabulary)
			{
			case Vocabulary::_PreprocessorDirective:
				_FE_FALLTHROUGH_;
			case Vocabulary::_Preprocessor:
				_FE_FALLTHROUGH_;
			case Vocabulary::_PreprocessorNextLine:
			{
				auto l_to_erase = it;
				++it;
				out_list_p.erase(l_to_erase);
			}
			continue;

			default:
				++it;
				continue;
			}
		}
	}

	void purge_string_literals_and_backslashes(std::pmr::list<token>& out_list_p) noexcept
	{
		for (auto it = out_list_p.begin(); it != out_list_p.end();)
		{
			switch (it->_vocabulary)
			{
			case Vocabulary::_TextLiteralPrefix:
				_FE_FALLTHROUGH_;
			case Vocabulary::_StringLiteral:
				_FE_FALLTHROUGH_;
			case Vocabulary::_CharLiteral:
				_FE_FALLTHROUGH_;
			case Vocabulary::_BackSlash:
			{
				auto l_to_erase = it;
				++it;
				out_list_p.erase(l_to_erase);
			}
			continue;

			default:
				++it;
				continue;
			}
		}
	}

	void purge_template(std::pmr::list<token>& out_list_p) noexcept
	{
		for (auto it = out_list_p.begin(); it != out_list_p.end();)
		{
			switch (it->_vocabulary)
			{
			case Vocabulary::_Template:
				_FE_FALLTHROUGH_;
			case Vocabulary::_BeginTemplateArgs:
				_FE_FALLTHROUGH_;
			case Vocabulary::_Typename:
				_FE_FALLTHROUGH_;
			case Vocabulary::_TemplateArg:
				_FE_FALLTHROUGH_;
			case Vocabulary::_EndTemplateArgs:
				_FE_FALLTHROUGH_;
			case Vocabulary::_TemplateBody:
			{
				auto l_to_erase = it;
				++it;
				out_list_p.erase(l_to_erase);
			}
			continue;

			default:
				++it;
				continue;
			}
		}
	}

	void purge_forward_declaration(std::pmr::list<token>& out_list_p) noexcept
	{
		for (auto it = out_list_p.begin(); it != out_list_p.end();)
		{
			switch (it->_vocabulary)
			{
			case Vocabulary::_ClassStructEnumMethodForwardDeclaration:
			{
				auto l_to_erase = it;
				++it;
				out_list_p.erase(l_to_erase);
			}
			continue;

			default:
				++it;
				continue;
			}
		}
	}


	_FE_NODISCARD_ token tokenize_identifiable(typename file_buffer_t::const_pointer code_iterator_p, context_stack_t& context_stack_p)
	{
		token l_token = 
		{ 
			._vocabulary = Vocabulary::_Undefined,
			._code = file_buffer_t(u8"\0", framework::get_framework().get_memory_resource())
		};


		// The top priority is marking out the comments.
		tokenize_comment(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token; // return if the text is a comment.
		}

		tokenize_preprocessor(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token; // return if the text is a preprocessor directive.
		}

		if (context_stack_p.back() != FHT::Context::_EnumStructFieldValue)
		{
			tokenize_string_literal(l_token, code_iterator_p, context_stack_p);
			if (l_token._vocabulary != Vocabulary::_Undefined)
			{
				return l_token; // return if the text is a string literal.
			}
		}

		tokenize_template(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token; // return if the text is a template declaration.
		}

		FHT::preprocessor::preprocess_macros(l_token, code_iterator_p, header_tool::get_program_options().get_macro_map()); // substitutes macro defines with their values.
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token;
		}

		// tokenize operators.
		tokenize_other(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token; // return if the text is an operator.
		}

		tokenize_reflection_macros(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token;
		}

		tokenize_namespace(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token;
		}

		tokenize_enum_struct(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token;
		}

		tokenize_struct(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token;
		}

		tokenize_class(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token;
		}

		tokenize_class_struct_enum_forward_decl_and_using_namespace(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token; // return if the text is a forward declaration.
		}


		tokenize_template_body(l_token, code_iterator_p, context_stack_p);




		l_token._code.reserve(100);
		while (*code_iterator_p != '{')
		{
			if (*code_iterator_p == ';')
			{
				l_token._vocabulary = Vocabulary::_AnyDecl;
				return l_token;
			}
			l_token._code += *code_iterator_p;
			++code_iterator_p;
		}




		file_buffer_t l_brace_stack(framework::get_framework().get_memory_resource());
		token l_tmp = { ._code{ framework::get_framework().get_memory_resource()} };
		do
		{
			switch (*code_iterator_p)
			{
			case '(':
				if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
					&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
				{
					l_brace_stack.push_back('(');
				}
				break;

			case ')':
				if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
					&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
				{
					if (l_brace_stack.back() == '(')
					{
						l_brace_stack.pop_back();
					}
				}
				break;


			case '{':
				if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
					&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
				{
					l_brace_stack.push_back('{');
				}
				break;

			case '}':
				if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
					&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
				{
					if (l_brace_stack.back() == '{')
					{
						l_brace_stack.pop_back();
					}

					switch (context_stack_p.back())
					{
					case FHT::Context::_AnyDecl:
						_FE_FALLTHROUGH_;
					case FHT::Context::_Class:
						context_stack_p.pop_back();
						break;

					default:
						break;
					}
				}
				break;

			default:
				break;
			}
			l_token._code += *code_iterator_p;
			++code_iterator_p;
			THROW_CPP_SYNTAX_ERROR(*code_iterator_p == '\0', "C++ Code Syntax Error C1075: missing '}' in class declaration, or found an explicit null terminator \0");
		} 
		while (l_brace_stack.size() > 0);
		l_token._vocabulary = Vocabulary::_AnyDecl;

		return l_token; 
	}

	_FE_NODISCARD_ token tokenize_unidentifiable(typename file_buffer_t::const_pointer code_iterator_p, context_stack_t& context_stack_p)
	{
		token l_token = 
		{
			._vocabulary = Vocabulary::_Undefined,
			._code = file_buffer_t(framework::get_framework().get_memory_resource())
		};
	
		for (;(tokenize_identifiable(code_iterator_p, context_stack_p)._vocabulary == Vocabulary::_Undefined) &&
			(*code_iterator_p > ' '); ++code_iterator_p)
		{
			l_token._code += *code_iterator_p;
		}
		return l_token;
	}


	_FE_NODISCARD_ std::pmr::list<token> tokenize_any_decl(file_buffer_t& file_p)
	{
		if (file_p.empty() == true)
		{
			throw FE::pair<FrogmanEngineHeaderToolError, FE::ASCII*>{FrogmanEngineHeaderToolError::_EmptyHeaderFile, "Warning, code buffer is empty."};
		}

		context_stack_t l_context_stack{ framework::get_framework().get_memory_resource() };
		l_context_stack.reserve(64);
		l_context_stack.emplace_back(FHT::Context::_Global);

		std::pmr::list<token> l_list{ framework::get_framework().get_memory_resource() };

		auto l_end = file_p.c_str() + file_p.size();
		var::uint32 l_token_number = 1;
		for (FE::UTF8* iterator = file_p.c_str(); iterator < l_end;)
		{
			if (*iterator <= ' ')
			{
				++iterator;
				continue;
			}


			token l_token = tokenize_identifiable_any_decl(iterator, l_context_stack);
			if (l_token._code.size() == 0)
			{
				continue;
			}

			if (l_token._vocabulary == Vocabulary::_Macro)
			{
				auto l_macro_pos = iterator - file_p.c_str();
				file_p.replace(l_macro_pos, l_token._macro_identifier_length, l_token._code);
				iterator = file_p.c_str() + l_macro_pos;
				l_end = file_p.c_str() + file_p.size();
			}

			l_token._token_line_number = l_token_number;
			if (l_token._vocabulary != Vocabulary::_Undefined)
			{
				iterator += l_token._code.size(); // move to the next.
				l_list.push_back(std::move(l_token)); // push_back the defined vocab.

				if (iterator < l_end) _FE_LIKELY_
				{
					if (*iterator == '\n')
					{
						l_token._vocabulary = Vocabulary::_LineEnd;
						l_token._code = file_buffer_t(1, *iterator, framework::get_framework().get_memory_resource());
						l_token._token_line_number = l_token_number;

						l_list.push_back(std::move(l_token));
						++l_token_number; // Increment the line number.
						++iterator; // move to the next.
					}
				}
				continue;
			}


			l_token = tokenize_unidentifiable_any_decl(iterator, l_context_stack);
			l_token._token_line_number = l_token_number;
			iterator += l_token._code.size(); // move to the next.
			l_list.push_back(std::move(l_token));

			if (iterator < l_end) _FE_LIKELY_
			{
				if (*iterator == '\n')
				{
					l_token._vocabulary = Vocabulary::_LineEnd;
					l_token._code = file_buffer_t(1, *iterator, framework::get_framework().get_memory_resource());
					l_token._token_line_number = l_token_number;

					l_list.push_back(std::move(l_token));
					++l_token_number; // Increment the line number.
					++iterator; // move to the next.
				}
			}
			continue;
		}

		l_list.emplace_back(Vocabulary::_EndOfCode, FE::null, l_token_number, u8"\0");
		return l_list;
	}

	_FE_NODISCARD_ token tokenize_identifiable_any_decl(typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		token l_token =
		{
			._vocabulary = Vocabulary::_Undefined,
			._code = file_buffer_t(u8"\0", framework::get_framework().get_memory_resource())
		};


		// The top priority is marking out the comments.
		tokenize_comment(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token; // return if the text is a comment.
		}

		tokenize_preprocessor(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token; // return if the text is a preprocessor directive.
		}

		tokenize_string_literal(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token; // return if the text is a string literal.
		}

		FHT::preprocessor::preprocess_macros(l_token, code_iterator_p, header_tool::get_program_options().get_macro_map()); // substitutes macro defines with their values.
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token;
		}

		// tokenize operators.
		tokenize_other(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined &&
			l_token._vocabulary != Vocabulary::_RightCurlyBracket &&
			l_token._vocabulary != Vocabulary::_LeftCurlyBracket
			)
		{
			return l_token; // return if the text is an operator.
		}

		tokenize_reflection_macros(l_token, code_iterator_p, context_stack_p);
		if (l_token._vocabulary != Vocabulary::_Undefined)
		{
			return l_token;
		}




		l_token._code.reserve(100);
		while (*code_iterator_p != '\0')
		{
			if (is_a_valid_letter_for_identifiers(*code_iterator_p) == false)
			{
				l_token._vocabulary = Vocabulary::_Identifier;
				return l_token;
			}
			l_token._code += *code_iterator_p;
			++code_iterator_p;
		}




		file_buffer_t l_brace_stack(framework::get_framework().get_memory_resource());
		token l_tmp = { ._code{ framework::get_framework().get_memory_resource()} };
		do
		{
			switch (*code_iterator_p)
			{
			case '(':
				if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
					&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
				{
					l_brace_stack.push_back('(');
				}
				break;

			case ')':
				if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
					&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
				{
					if (l_brace_stack.back() == '(')
					{
						l_brace_stack.pop_back();
					}
				}
				break;


			case '{':
				if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
					&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
				{
					l_brace_stack.push_back('{');
				}
				break;

			case '}':
				if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
					&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
				{
					if (l_brace_stack.back() == '{')
					{
						l_brace_stack.pop_back();
					}
				}
				break;


			case '\0':
				return l_token;

			default:
				break;
			}
			l_token._code += *code_iterator_p;
			++code_iterator_p;
			THROW_CPP_SYNTAX_ERROR(*code_iterator_p == '\0', "C++ Code Syntax Error C1075: missing '}' in class declaration, or found an explicit null terminator \0");
		} while (l_brace_stack.size() > 0);
		l_token._code += *code_iterator_p;
		l_token._vocabulary = Vocabulary::_AnyDecl;

		return l_token;
	}

	_FE_NODISCARD_ token tokenize_unidentifiable_any_decl(typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		token l_token =
		{
			._vocabulary = Vocabulary::_Undefined,
			._code = file_buffer_t(framework::get_framework().get_memory_resource())
		};

		for (;(tokenize_identifiable_any_decl(code_iterator_p, context_stack_p)._vocabulary == Vocabulary::_Undefined) &&
			(*code_iterator_p > ' '); ++code_iterator_p)
		{
			l_token._code += *code_iterator_p;
		}
		return l_token;
	}


	void tokenize_comment(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p) noexcept
	{
		thread_local static std::string tl_s_key_buffer;

		auto l_prefix_iterators = g_vocabulary.equal_prefix_range_ks(FE::iterator_cast<FE::ASCII*>(code_iterator_p), 2);

		for (auto it = l_prefix_iterators.first; it != l_prefix_iterators.second; ++it) // iterate all candidates.
		{
			it.key(tl_s_key_buffer); // populate the key buffer.
			switch (it.value()) // Check if it matches the Vocabulary enum value.
			{
			case Vocabulary::_CommentBegin:
				if (FE::algorithm::string::compare_ranged(	(FE::ASCII*)code_iterator_p, FE::algorithm::string::range{ 0, tl_s_key_buffer.length() },
																tl_s_key_buffer.c_str(), FE::algorithm::string::range{ 0, tl_s_key_buffer.length() }) == true)
				{
					context_stack_p.emplace_back(FHT::Context::_CommentBlock);
					out_token_p._vocabulary = it.value();
					out_token_p._code = reinterpret_cast<FE::UTF8*>(tl_s_key_buffer.c_str());
					return;
				}
				break;


			case Vocabulary::_CommentEnd:
				if (FE::algorithm::string::compare_ranged((FE::ASCII*)code_iterator_p, FE::algorithm::string::range{ 0, tl_s_key_buffer.length() },
					tl_s_key_buffer.c_str(), FE::algorithm::string::range{ 0, tl_s_key_buffer.length() }) == true)
				{
					if (context_stack_p.back() == FHT::Context::_CommentBlock)
					{
						context_stack_p.pop_back();
						out_token_p._vocabulary = it.value();
						out_token_p._code = reinterpret_cast<FE::UTF8*>(tl_s_key_buffer.c_str());
						return;
					}
				}
				break;


			case Vocabulary::_LineComment:
				if (FE::algorithm::string::compare_ranged((FE::ASCII*)code_iterator_p, FE::algorithm::string::range{ 0, tl_s_key_buffer.length() },
					tl_s_key_buffer.c_str(), FE::algorithm::string::range{ 0, tl_s_key_buffer.length() }) == true)
				{
					out_token_p._vocabulary = it.value();
					auto l_rng = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, '\n');

					if (l_rng != std::nullopt) _FE_LIKELY_
					{
						out_token_p._code.assign(code_iterator_p, l_rng->_begin);
					}
					else
					{
						out_token_p._code = code_iterator_p; // EOF
					}
					return;
				}
				break;


			default:
				if (context_stack_p.back() == FHT::Context::_CommentBlock)
				{
					goto MarkAsComment;
				}
				break;
			}
		}

		if (context_stack_p.back() == FHT::Context::_CommentBlock)
		{
		MarkAsComment:
			out_token_p._vocabulary = Vocabulary::_CommentBody;
			auto l_rng = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, '\n');

			if (l_rng != std::nullopt) _FE_LIKELY_
			{
				out_token_p._code.assign(code_iterator_p, l_rng->_begin);
			}
			else
			{
				out_token_p._code = code_iterator_p; // EOF
			}

			auto l_pos = out_token_p._code.find(u8"*/");
			if (l_pos != std::string::npos)
			{
				out_token_p._code.erase(l_pos, out_token_p._code.size() - l_pos);
			}
			return;
		}
	}

	void tokenize_preprocessor(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p) noexcept
	{
		switch (context_stack_p.back())
		{
		case FHT::Context::_StringLiteral:
			_FE_FALLTHROUGH_;
		case FHT::Context::_CharLiteral:
			return; // Preprocessor directives are not recognized inside string literals or char literals.

		default:
			break;
		}

		switch (*code_iterator_p)
		{
		case '#':
			if (context_stack_p.back() != FHT::Context::_Preprocessor)
			{
				context_stack_p.emplace_back(FHT::Context::_Preprocessor);
			}
			out_token_p._vocabulary = Vocabulary::_PreprocessorDirective;
			out_token_p._code = *code_iterator_p;
			break;


		case '\\':
			if (context_stack_p.back() == FHT::Context::_Preprocessor)
			{
				out_token_p._vocabulary = Vocabulary::_PreprocessorNextLine;
				out_token_p._code = *code_iterator_p;
			}
			break;


		default:
			if (context_stack_p.back() == FHT::Context::_Preprocessor)
			{
				out_token_p._vocabulary = Vocabulary::_Preprocessor;
				auto l_rng = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, '\n');

				if (l_rng != std::nullopt) _FE_LIKELY_
				{
					out_token_p._code.assign(code_iterator_p, l_rng->_begin);
				}
				else
				{
					out_token_p._code = code_iterator_p; // EOF
				}
				
				if (out_token_p._code.back() == '\\')
				{
					out_token_p._code.pop_back();
				}
				else
				{
					context_stack_p.pop_back();
				}
			}
			break;
		}
	}


	void tokenize_string_literal(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		thread_local static file_buffer_t tl_s_delimiter;

		// tokenize _TextLiteralPrefix
		switch (*code_iterator_p)
		{
		case 'L':
			_FE_FALLTHROUGH_;
		case 'u':
			_FE_FALLTHROUGH_;
		case 'U': 
			_FE_FALLTHROUGH_;

		case 'R':
			{
				auto l_quote = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, '\"');
				auto l_line_end = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, '\n');


				if ((l_quote == std::nullopt) || (l_line_end == std::nullopt)) // NAh!
				{
					return;
				}


				if (!(l_quote->_begin < l_line_end->_begin)) // doesn't the first " comes before \n in the current line?
				{
					break;
				}


				out_token_p._code.assign(code_iterator_p, l_quote->_begin); // copy until ".

				while (out_token_p._code.back() == ' ') // the first character is the case value. Purge all trailing spaces between the prefix and the quote.
				{
					out_token_p._code.pop_back();
				}


				switch (out_token_p._code.length()) // test the length of the purged string.
				{
				case 1:
					out_token_p._vocabulary = Vocabulary::_TextLiteralPrefix;

					if (out_token_p._code.back() == 'R') // is R
					{
						context_stack_p.push_back(FHT::Context::_RawTextLiteral);
						extract_raw_text_delimiter_from_the_left_quote(tl_s_delimiter, code_iterator_p);
						return;
					}
					tl_s_delimiter.clear();
					return;


				case 2:
					if (out_token_p._code.back() == '8') // is u8
					{
						out_token_p._vocabulary = Vocabulary::_TextLiteralPrefix;
						tl_s_delimiter.clear();
						return;
					}

					if (out_token_p._code.back() == 'R') // is LR, uR, or UR
					{
						out_token_p._vocabulary = Vocabulary::_TextLiteralPrefix;
						context_stack_p.push_back(FHT::Context::_RawTextLiteral);
						extract_raw_text_delimiter_from_the_left_quote(tl_s_delimiter, code_iterator_p);
						return;
					}

					tl_s_delimiter.clear();
					out_token_p._code.clear(); // is not a valid prefix.
					break;


				case 3:
					if (out_token_p._code == u8"u8R")
					{
						out_token_p._vocabulary = Vocabulary::_TextLiteralPrefix;
						context_stack_p.push_back(FHT::Context::_RawTextLiteral);
						extract_raw_text_delimiter_from_the_left_quote(tl_s_delimiter, code_iterator_p);
						return;
					}

					tl_s_delimiter.clear();
					out_token_p._code.clear(); // is not a valid prefix.
					break;
					

				default:
					tl_s_delimiter.clear();
					out_token_p._code.clear(); // is not a valid prefix.
					break;
				}
			}
			break;


		default:
			break; 
		}




		switch (*code_iterator_p)
		{
		case '\"':
			switch (context_stack_p.back())
			{
			case FHT::Context::_StringLiteral:
				if (code_iterator_p[-1] != '\\')
				{
					context_stack_p.pop_back(); // is accessible when "".
				}
				break;

			case FHT::Context::_RawTextLiteral:
				if (tl_s_delimiter.length() > 0)
				{
					auto l_pos = code_iterator_p - tl_s_delimiter.length();
					if (FE::algorithm::string::compare_ranged(
						l_pos,
						FE::algorithm::string::range{ ._begin = 0, ._end = tl_s_delimiter.length() },

						tl_s_delimiter.c_str(),
						FE::algorithm::string::range{ ._begin = 0, ._end = tl_s_delimiter.length() }
					) == true)
					{
						context_stack_p.pop_back(); // is accessible when R"delimiter()delimiter"
						tl_s_delimiter.clear();
						break;
					}
				}

				if (code_iterator_p[-1] == ')') // does not have any delimiters; is the previous character ')'?
				{
					context_stack_p.pop_back(); // is accessible when R"()"
					break;
				}
				break;

			default:
				context_stack_p.emplace_back(FHT::Context::_StringLiteral);
				break;
			}

			out_token_p._vocabulary = Vocabulary::_StringLiteral;
			out_token_p._code = *code_iterator_p;
			break;


		case '\'':
			if (context_stack_p.back() == FHT::Context::_CharLiteral)
			{
				if (code_iterator_p[-1] != '\\') // is accessible when '' or '\''.
				{
					context_stack_p.pop_back();
				}
			}
			else if (context_stack_p.back() != FHT::Context::_RawTextLiteral)
			{
				context_stack_p.emplace_back(FHT::Context::_CharLiteral);
			}
			out_token_p._vocabulary = Vocabulary::_CharLiteral;
			out_token_p._code = *code_iterator_p;
			break;


		default:
			switch (context_stack_p.back())
			{
			case FHT::Context::_CharLiteral:
				out_token_p._vocabulary = Vocabulary::_CharLiteral;
				out_token_p._code = *code_iterator_p;
				break;

			case FHT::Context::_StringLiteral:
				_FE_FALLTHROUGH_;
			case FHT::Context::_RawTextLiteral:
				out_token_p._vocabulary = Vocabulary::_StringLiteral;
				{
					auto l_rng = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, '\"');

					THROW_CPP_SYNTAX_ERROR(l_rng == std::nullopt, "FHT C++ Code Syntax Error C2001: the string literal is incomplete.")
					out_token_p._code.assign(code_iterator_p, l_rng->_begin);
				}
				break;

			default:
				break;
			}
			break;
		}
	}

	void extract_raw_text_delimiter_from_the_left_quote(file_buffer_t& out_return_p, typename file_buffer_t::const_pointer code_iterator_p)
	{
		auto l_start = code_iterator_p;
		while (*l_start != '\"')
		{
			THROW_CPP_SYNTAX_ERROR(*l_start == '\0', "C++ code syntax Error C2001: the raw text literal delimiter is incomplete.");
			++l_start;
		}
		++l_start; // skip "

		auto l_end = l_start;
		while (*l_end != '(')
		{
			THROW_CPP_SYNTAX_ERROR(*l_end == '\0', "C++ code syntax Error C2001: the raw text literal delimiter is incomplete.");
			++l_end;
		}

		out_return_p.assign(l_start, l_end);
	}


	void tokenize_template(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		constexpr FE::UTF8* l_template_keyword = u8"template";
		FE::algorithm::string::range l_template = { 0, FE::algorithm::string::length(l_template_keyword) };
		if (FE::algorithm::string::compare_ranged<FE::UTF8>(code_iterator_p, l_template,
			l_template_keyword, l_template)
			== true)
		{
			auto l_code_line_end = FE::algorithm::string::find_the_first(code_iterator_p, u8';');
			THROW_CPP_SYNTAX_ERROR(l_code_line_end == std::nullopt, "C++ code syntax Error C2143: the template declaration is incomplete; ; is missing.");

			if (FE::algorithm::string::space_insensitive_contains(code_iterator_p, l_code_line_end->_begin, u8"template<") == false)
			{
				// is a template class forward declaration: template class BasicTemplateTypename<int>;
				out_token_p._vocabulary = Vocabulary::_ClassStructEnumMethodForwardDeclaration;
				out_token_p._code.assign(code_iterator_p, l_code_line_end->_end);
				return;
			}

			out_token_p._vocabulary = Vocabulary::_Template;
			out_token_p._code = l_template_keyword;
			context_stack_p.emplace_back(FHT::Context::_Template);
			return;
		}


		switch (context_stack_p.back())
		{
		case FHT::Context::_Template:
			if (*code_iterator_p == '<')
			{
				out_token_p._vocabulary = Vocabulary::_BeginTemplateArgs;
				out_token_p._code = *code_iterator_p;
				context_stack_p.emplace_back(FHT::Context::_TemplateArgs);
				return;
			}
			break;


		case FHT::Context::_TemplateArgs:
			if (*code_iterator_p == '>')
			{
				out_token_p._vocabulary = Vocabulary::_EndTemplateArgs;
				out_token_p._code = *code_iterator_p;
				context_stack_p.pop_back();

				if (context_stack_p.size() >= 3)
				{
					auto l_right_before_back = context_stack_p.begin() + (context_stack_p.size() - 3);
					if (*l_right_before_back == FHT::Context::_Template) // is the C++ 17 nested template template argument: template <template <typename T> class C> class C {};
					{
						context_stack_p.pop_back(); // pop the template template arg
					}
				}
				return;
			}
			else
			{
				auto l_end_args = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, '>');
				auto l_nested_begin_args = FE::algorithm::string::find_the_first_within_range<FE::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0, l_end_args->_begin }, '<');

				THROW_CPP_SYNTAX_ERROR(l_end_args == std::nullopt, "C++ code syntax Error C2988: the template argument list is incomplete.");

				if (l_nested_begin_args == std::nullopt) // not found '<'
				{
					// found '>'; copy until '>'
					out_token_p._vocabulary = Vocabulary::_TemplateArg;
					out_token_p._code.assign(code_iterator_p, l_end_args->_begin);
					return;
				}

				out_token_p._vocabulary = Vocabulary::_TemplateArg;
				out_token_p._code.assign(code_iterator_p, l_nested_begin_args->_end); // found '<'; copy until '<'
				return;
			}


		default:
			break;
		}
	}


	void tokenize_other(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, _FE_MAYBE_UNUSED_ FHT::context_stack_t& context_stack_p)
	{
		switch (*code_iterator_p)
		{
		case '{':
			switch (context_stack_p.back())
			{
			case FHT::Context::_AnyDecl:
				_FE_FALLTHROUGH_;
			case FHT::Context::_ClassBody:
				return;

			case FHT::Context::_ClassExtension:
				context_stack_p.pop_back();
				return;

			case FHT::Context::_Namespace:
				context_stack_p.pop_back();
				break;

			default:
				break;
			}
			out_token_p._vocabulary = Vocabulary::_LeftCurlyBracket;
			out_token_p._code = *code_iterator_p;
			break;
			
		case '}':
			switch (context_stack_p.back())
			{
			case FHT::Context::_AnyDecl:
				break;

			case FHT::Context::_ClassBody:
				while (context_stack_p.back() != FHT::Context::_Class)
				{
					context_stack_p.pop_back();
				}
				context_stack_p.pop_back();
				break;

			case FHT::Context::_EnumStructBody:
				while (context_stack_p.back() != FHT::Context::_EnumStruct)
				{
					context_stack_p.pop_back();
				}
				context_stack_p.pop_back();
				break;

			default:
				break;
			}
			out_token_p._vocabulary = Vocabulary::_RightCurlyBracket;
			out_token_p._code = *code_iterator_p;
			break;


		case '(':
			out_token_p._vocabulary = Vocabulary::_LeftParen;
			out_token_p._code = *code_iterator_p;
			break;

		case ')':
			if (context_stack_p.back() == FHT::Context::_BeginNamespace)
			{
				context_stack_p.pop_back();
			}
			out_token_p._vocabulary = Vocabulary::_RightParen;
			out_token_p._code = *code_iterator_p;
			break;


		case '[':
			if (context_stack_p.back() == FHT::Context::_ProbablyAttribute)
			{
				context_stack_p.push_back(FHT::Context::_Attribute);
			}
			else
			{
				context_stack_p.push_back(FHT::Context::_ProbablyAttribute);
			}
			out_token_p._vocabulary = Vocabulary::_LeftBracket;
			out_token_p._code = *code_iterator_p;
			break;

		case ']':
			if (context_stack_p.back() == FHT::Context::_Attribute)
			{
				context_stack_p.pop_back();
				if (context_stack_p.back() == FHT::Context::_ProbablyAttribute)
				{
					context_stack_p.pop_back();
				}
			}
			out_token_p._vocabulary = Vocabulary::_RightBracket;
			out_token_p._code = *code_iterator_p;
			break;


		case ';':
			switch (context_stack_p.back())
			{
			case FHT::Context::_AnyDecl:
				context_stack_p.pop_back();
				break;

			case FHT::Context::_ClassBody:
				while (context_stack_p.back() != FHT::Context::_Class)
				{
					context_stack_p.pop_back();
				}
				context_stack_p.pop_back();
				break;

			case FHT::Context::_EnumStructBody:
				while (context_stack_p.back() != FHT::Context::_EnumStruct)
				{
					context_stack_p.pop_back();
				}
				context_stack_p.pop_back();
				break;

			default:
				break;
			}
			out_token_p._vocabulary = Vocabulary::_Semicolon;
			out_token_p._code = *code_iterator_p;
			break;

		case ',':
			THROW_CPP_SYNTAX_ERROR(context_stack_p.back() == FHT::Context::_ClassExtension, "Frogman C++ does not allow multiple inheritance.");
			out_token_p._vocabulary = Vocabulary::_Comma;
			out_token_p._code = *code_iterator_p;
			break;

		case '=':
			if (context_stack_p.back() == FHT::Context::_EnumStructBody)
			{
				context_stack_p.push_back(FHT::Context::_EnumStructFieldValue);
			}
			out_token_p._vocabulary = Vocabulary::_AssignmentOperator;
			out_token_p._code = *code_iterator_p;
			break;


		case '.':
			_FE_FALLTHROUGH_;
		case '+':
			_FE_FALLTHROUGH_;
		case '-':
			_FE_FALLTHROUGH_;
		case '*':
			_FE_FALLTHROUGH_;
		case '/':
			_FE_FALLTHROUGH_;
		case '%':
			_FE_FALLTHROUGH_;
		case '&':
			_FE_FALLTHROUGH_;
		case '|':
			_FE_FALLTHROUGH_;
		case '^':
			_FE_FALLTHROUGH_;
		case '!':
			_FE_FALLTHROUGH_;
		case '<':
			_FE_FALLTHROUGH_;
		case '>':
			out_token_p._vocabulary = Vocabulary::_Operator;
			out_token_p._code = *code_iterator_p;
			break;

		case '~':
			out_token_p._vocabulary = Vocabulary::_BitwiseNot;
			out_token_p._code = *code_iterator_p;
			break;


		case 'a':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"alignas") }, u8"alignas")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Alignas;
				out_token_p._code = u8"alignas";
			}
			break;

		case 'c':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"constexpr") }, u8"constexpr")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Constexpr;
				out_token_p._code = u8"constexpr";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"consteval") }, u8"consteval")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Consteval;
				out_token_p._code = u8"consteval";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"constinit") }, u8"constinit")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Constinit;
				out_token_p._code = u8"constinit";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"const") }, u8"const")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Const;
				out_token_p._code = u8"const";
			}
			break;


		case 'e':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"extern") }, u8"extern")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Extern;
				out_token_p._code = u8"extern";
			}
			break;


		case 'f':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"friend") }, u8"friend")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Friend;
				out_token_p._code = u8"friend";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"final") }, u8"final")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Final;
				out_token_p._code = u8"final";
			}
			break;


		case 'i':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"inline") }, u8"inline")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Inline;
				out_token_p._code = u8"inline";
			}
			break;


		case 'm':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"mutable") }, u8"mutable")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Mutable;
				out_token_p._code = u8"mutable";
			}
			break;


		case 'n':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"noexcept") }, u8"noexcept")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Noexcept;
				out_token_p._code = u8"noexcept";
			}
			break;


		case 'o':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"override") }, u8"override")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Override;
				out_token_p._code = u8"override";
			}
			break;


		case 'p':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"public") }, u8"public")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Public;
				out_token_p._code = u8"public";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"private") }, u8"private")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Private;
				out_token_p._code = u8"private";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"protected") }, u8"protected")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Protected;
				out_token_p._code = u8"protected";
			}
			break;


		case 's':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"static") }, u8"static")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Static;
				out_token_p._code = u8"static";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"static_assert") }, u8"static_assert")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_StaticAssert;
				out_token_p._code = u8"static_assert";
			}
			break;


		case 't':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"this") }, u8"this")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_This;
				out_token_p._code = u8"this";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"thread_local") }, u8"thread_local")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_ThreadLocal;
				out_token_p._code = u8"thread_local";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"typedef") }, u8"typedef")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Typedef;
				out_token_p._code = u8"typedef";
			}
			break;


		case 'u':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"using") }, u8"using")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Using;
				out_token_p._code = u8"using";
			}
			break;


		case 'v':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"volatile") }, u8"volatile")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Volatile;
				out_token_p._code = u8"volatile";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"virtual") }, u8"virtual")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_Virtual;
				out_token_p._code = u8"virtual";
			}
			break;


		case ':':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"::") }, u8"::")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_NamespaceConcatenator;
				out_token_p._code = u8"::";
			}
			else if (*code_iterator_p == ':')
			{
				out_token_p._vocabulary = Vocabulary::_Colon;
				out_token_p._code = *code_iterator_p;

				switch (context_stack_p.back())
				{
				case FHT::Context::_EnumStructBody:
					context_stack_p.push_back(FHT::Context::_EnumStructExtension);
					break;

				case FHT::Context::_ClassBody:
					context_stack_p.push_back(FHT::Context::_ClassExtension);
					break;

				default:
					break;
				} 
			}
			break;


		case '_':
			if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"__forceinline") }, u8"__forceinline")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_ForceInline;
				out_token_p._code = u8"__forceinline";
			}
			else if (FE::algorithm::string::find_the_first_within_range<var::UTF8>(code_iterator_p, FE::algorithm::string::range{ 0,FE::algorithm::string::compiletime::length(u8"_FE_FORCE_INLINE_") }, u8"_FE_FORCE_INLINE_")
				!= std::nullopt)
			{
				out_token_p._vocabulary = Vocabulary::_FrogmanEngineForceInline;
				out_token_p._code = u8"_FE_FORCE_INLINE_";
			}
		break;


		default:
			break;
		}
	}


	void tokenize_reflection_macros(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		auto l_prefix_iterators = g_vocabulary.equal_prefix_range_ks("FE_", 3);
		thread_local static std::string tl_s_key_buffer;

		for (auto it = l_prefix_iterators.first; it != l_prefix_iterators.second; ++it) // iterate all candidates.
		{
			it.key(tl_s_key_buffer); // populate the key buffer.
			switch (it.value())
			{
			case Vocabulary::_FrogmanEngineBaseClassReflectionMacro:
				_FE_FALLTHROUGH_;

			case Vocabulary::_FrogmanEnginePropertyReflectionMacro:
				_FE_FALLTHROUGH_;

			case Vocabulary::_FrogmanEngineStaticMethodReflectionMacro:
				_FE_FALLTHROUGH_;

			case Vocabulary::_FrogmanEngineMethodReflectionMacro:
				_FE_FALLTHROUGH_;

			case Vocabulary::_FrogmanEngineClassReflectionMacro:
				_FE_FALLTHROUGH_;

			case Vocabulary::_FrogmanEngineStructReflectionMacro:
				_FE_FALLTHROUGH_;

			case Vocabulary::_FrogmanEngineEnumStructReflectionMacro:
				_FE_FALLTHROUGH_;

			case Vocabulary::_FrogmanEngineSystemMacro:
				if (FE::algorithm::string::space_insensitive_contains((FE::ASCII*)code_iterator_p, tl_s_key_buffer.length(), tl_s_key_buffer.c_str()))
				{
					out_token_p._vocabulary = it.value();
					out_token_p._code = reinterpret_cast<FE::UTF8*>(tl_s_key_buffer.c_str());

					if (out_token_p._vocabulary == Vocabulary::_FrogmanEngineSystemMacro)
					{
						context_stack_p.push_back(FHT::Context::_FrogmanEngineSystemMacro);
					}
					return;
				}
				break;

			default:
				break;
			}
			
		}

		if (FE::algorithm::string::space_insensitive_contains((FE::ASCII*)code_iterator_p, FE::algorithm::string::compiletime::length("ENABLE_SERIALIZATION"), "ENABLE_SERIALIZATION"))
		{
			out_token_p._vocabulary = Vocabulary::_FrogmanEngineEnableSerialization;
			out_token_p._code = u8"ENABLE_SERIALIZATION";
			return;
		}

		if (context_stack_p.back() == FHT::Context::_FrogmanEngineSystemMacro)
		{
			thread_local static var::int32 tl_s_arg_index = 0;
			auto l_comma = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, ',');

			switch (tl_s_arg_index)
			{
			case 0:
				THROW_CPP_SYNTAX_ERROR(l_comma == std::nullopt, "FHT C++ Error: the FE_SYSTEM macro is ill-formed.");
				out_token_p._vocabulary = Vocabulary::_FrogmanEngineSystemArgSysCallPhase;
				out_token_p._code.assign(code_iterator_p, l_comma->_begin);
				++tl_s_arg_index;
				return;

			case 1:
				l_comma = FE::algorithm::string::find_the_first<FE::UTF8>(code_iterator_p, ')');
				THROW_CPP_SYNTAX_ERROR(l_comma == std::nullopt, "FHT C++ Error: the FE_SYSTEM macro is ill-formed.");
				out_token_p._code.assign(code_iterator_p, l_comma->_begin);
				out_token_p._vocabulary = Vocabulary::_FrogmanEngineSystemArgWorldTagEnum;
				++tl_s_arg_index;
				return;

			default: // EOR; reset.
				tl_s_arg_index = 0;
				break;
			}
			context_stack_p.pop_back();
		}
	}


	void tokenize_namespace(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		switch (context_stack_p.back())
		{
		case FHT::Context::_BeginNamespace:
			{
				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_identifier_end_pos = 0;
				for (auto it = code_iterator_p; (*it != ')'); ++it)
				{
					if ((is_a_valid_letter_for_identifiers(*it) == false))
					{
						break;
					}
					++l_identifier_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_identifier_end_pos);
				out_token_p._vocabulary = Vocabulary::_NamespaceIdentifier;
				return;
			}
			break;

		case FHT::Context::_Namespace:
			{
				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_identifier_end_pos = 0;
				for (auto it = code_iterator_p; (*it != '{'); ++it)
				{
					if ((is_a_valid_letter_for_identifiers(*it) == false))
					{
						break;
					}
					++l_identifier_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_identifier_end_pos);
				out_token_p._vocabulary = Vocabulary::_NamespaceIdentifier;
				return;
			}
			break;


		case FHT::Context::_EnumStructBody:
			_FE_FALLTHROUGH_;
		case FHT::Context::_EnumStructFieldValue:
			_FE_FALLTHROUGH_;
		case FHT::Context::_StructIdentifier:
			_FE_FALLTHROUGH_;
		case FHT::Context::_Class:
			_FE_FALLTHROUGH_;
		case FHT::Context::_Template:
			return;


		default:
			{
				//tokenize_class_struct_enum_forward_decl_and_using_namespace(out_token_p, code_iterator_p, context_stack_p);
				//if (out_token_p._vocabulary != Vocabulary::_Undefined)
				//{
				//	return; // return if the text is a forward declaration.
				//}

				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_namespace_keyword_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_namespace_keyword_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_namespace_keyword_end_pos);


				if (out_token_p._code.starts_with(u8"END_NAMESPACE"))
				{
					out_token_p._vocabulary = Vocabulary::_EndNamespace;
					out_token_p._code.resize(FE::algorithm::string::compiletime::length(u8"END_NAMESPACE"));
					return;
				}

				if (out_token_p._code.starts_with(u8"BEGIN_NAMESPACE"))
				{
					out_token_p._vocabulary = Vocabulary::_BeginNamespace;
					context_stack_p.emplace_back(FHT::Context::_BeginNamespace);
					out_token_p._code.resize(FE::algorithm::string::compiletime::length(u8"BEGIN_NAMESPACE"));
					return;
				}

				if (out_token_p._code.starts_with(u8"namespace"))
				{
					out_token_p._vocabulary = Vocabulary::_Namespace;
					context_stack_p.emplace_back(FHT::Context::_Namespace);
					out_token_p._code.resize(FE::algorithm::string::compiletime::length(u8"namespace"));
					return;
				}
			}
			break;
		}
	}

	void tokenize_enum_struct(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		switch (context_stack_p.back())
		{
		case FHT::Context::_EnumStruct:
			{
				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_struct_keyword_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_struct_keyword_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_struct_keyword_end_pos);


				if (out_token_p._code.starts_with(u8"struct"))
				{
					out_token_p._vocabulary = Vocabulary::_StructKeywordOfEnumStruct;
					context_stack_p.emplace_back(FHT::Context::_EnumStructIdentifier);
					out_token_p._code.resize(FE::algorithm::string::compiletime::length(u8"struct"));
					return;
				}

				THROW_CPP_SYNTAX_ERROR(true, "Frogman C++ Error: 'enum' or 'enum class' is unsupported; please use 'enum struct' instead.");
			}
		break;


		case FHT::Context::_EnumStructIdentifier:
			{
				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_identifier_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_identifier_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_identifier_end_pos);
				out_token_p._vocabulary = Vocabulary::_EnumStructIdentifier;
				context_stack_p.emplace_back(FHT::Context::_EnumStructBody);
				return;
			}
		break;


		case FHT::Context::_EnumStructExtension:
			{
				auto l_result = FE::algorithm::string::find_the_first(code_iterator_p, u8'{');
				THROW_CPP_SYNTAX_ERROR(l_result == std::nullopt, "Frogman C++ Syntax Error: the enum struct body is missing.");

				out_token_p._code.assign(code_iterator_p, l_result->_begin);
				out_token_p._vocabulary = Vocabulary::_EnumStructExtension;

				while (out_token_p._code.length() > 0)
				{
					if (out_token_p._code.back() <= ' ')
					{
						out_token_p._code.pop_back();
						continue;
					}
					break;
				}
				context_stack_p.pop_back();
				return;
			}
		break;


		case FHT::Context::_EnumStructBody:
			_FE_FALLTHROUGH_;
		case FHT::Context::_EnumStructFieldValue:
			while (*code_iterator_p != '}')
			{
				if (*code_iterator_p == ',')
				{
					out_token_p._vocabulary = Vocabulary::_EnumStructField;
					return;
				}

				if (*code_iterator_p == '=')
				{
					context_stack_p.emplace_back(FHT::Context::_EnumStructFieldValue);
					out_token_p._vocabulary = Vocabulary::_EnumStructField;
					return;
				}


				if (context_stack_p.back() == FHT::Context::_EnumStructFieldValue)
				{
					while (*code_iterator_p != '\n')
					{
						if (*code_iterator_p == ',')
						{
							context_stack_p.pop_back();
							out_token_p._vocabulary = Vocabulary::_EnumStructFieldValue;
							return;
						}

						token l_tmp;
						tokenize_string_literal(l_tmp, code_iterator_p, context_stack_p);

						if (l_tmp._vocabulary != Vocabulary::_CharLiteral)
						{
							if (*code_iterator_p == '}')
							{
								while (context_stack_p.back() != FHT::Context::_EnumStruct)
								{
									context_stack_p.pop_back();
								}
								context_stack_p.pop_back();
								out_token_p._vocabulary = Vocabulary::_EnumStructFieldValue;
								return;
							}

							out_token_p._code += *code_iterator_p;
							++code_iterator_p;
							continue;
						}

						while (l_tmp._vocabulary == Vocabulary::_CharLiteral)
						{
							out_token_p._code += l_tmp._code;
							l_tmp._code.clear();
							l_tmp._vocabulary = Vocabulary::_Undefined;
							++code_iterator_p;
							tokenize_string_literal(l_tmp, code_iterator_p, context_stack_p);
						}

						++code_iterator_p;
					}
					while (context_stack_p.back() != FHT::Context::_EnumStructBody)
					{
						context_stack_p.pop_back();
					}
					out_token_p._vocabulary = Vocabulary::_EnumStructFieldValue;
					return;
				}

				if (is_a_valid_letter_for_identifiers(*code_iterator_p) == false)
				{
					if (*code_iterator_p == '=')
					{
						context_stack_p.emplace_back(FHT::Context::_EnumStructFieldValue);
						out_token_p._vocabulary = Vocabulary::_EnumStructField;
						return;
					}

					out_token_p._vocabulary = Vocabulary::_EnumStructField;
					return;
				}
				out_token_p._code += *code_iterator_p;
				++code_iterator_p;
			}
			out_token_p._vocabulary = Vocabulary::_EnumStructField;

			while (context_stack_p.back() != FHT::Context::_EnumStruct)
			{
				context_stack_p.pop_back();
			}
			context_stack_p.pop_back();
			break;


		case FHT::Context::_StructIdentifier:
			_FE_FALLTHROUGH_;
		case FHT::Context::_Class:
			_FE_FALLTHROUGH_;
		case FHT::Context::_Template:
			return;


		default:
			{
				//tokenize_class_struct_enum_forward_decl_and_using_namespace(out_token_p, code_iterator_p, context_stack_p);
				//if (out_token_p._vocabulary != Vocabulary::_Undefined)
				//{
				//	return; // return if the text is a forward declaration.
				//}

				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_enum_keyword_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_enum_keyword_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_enum_keyword_end_pos);

				if (out_token_p._code.starts_with(u8"enum"))
				{
					out_token_p._vocabulary = Vocabulary::_EnumStruct;
					context_stack_p.emplace_back(FHT::Context::_EnumStruct);
					out_token_p._code.resize(FE::algorithm::string::compiletime::length(u8"enum"));
					return;
				}

				if (context_stack_p.back() == FHT::Context::_Attribute)
				{
					context_stack_p.pop_back();
					if (context_stack_p.back() == FHT::Context::_ProbablyAttribute)
					{
						context_stack_p.pop_back();
					}
					out_token_p._vocabulary = Vocabulary::_Attribute;
					return;
				}
				
				out_token_p._code.clear();
				return;
			}
			break;
		}
	}

	void tokenize_struct(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		switch (context_stack_p.back())
		{
		case FHT::Context::_StructIdentifier:
			{
				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_identifier_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_identifier_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_identifier_end_pos);
				out_token_p._vocabulary = Vocabulary::_StructIdentifier;
				context_stack_p.pop_back();
				context_stack_p.push_back(FHT::Context::_AnyDecl);
				return;
			}
			break;


		case FHT::Context::_Class:
			_FE_FALLTHROUGH_;
		case FHT::Context::_Template:
			return;


		default:
			{
				//tokenize_class_struct_enum_forward_decl_and_using_namespace(out_token_p, code_iterator_p, context_stack_p);
				//if (out_token_p._vocabulary != Vocabulary::_Undefined)
				//{
				//	return; // return if the text is a forward declaration.
				//}

				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_struct_keyword_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_struct_keyword_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_struct_keyword_end_pos);

				if (out_token_p._code.starts_with(u8"struct"))
				{
					out_token_p._vocabulary = Vocabulary::_Struct;
					context_stack_p.emplace_back(FHT::Context::_StructIdentifier);
					out_token_p._code.resize(FE::algorithm::string::compiletime::length(u8"struct"));
					return;
				}

				if (context_stack_p.back() == FHT::Context::_Attribute)
				{
					context_stack_p.pop_back();
					if (context_stack_p.back() == FHT::Context::_ProbablyAttribute)
					{
						context_stack_p.pop_back();
					}
					out_token_p._vocabulary = Vocabulary::_Attribute;
					return;
				}

				out_token_p._code.clear();
				return;
			}
			break;
		}
	}

	void tokenize_class(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		switch (context_stack_p.back())
		{
		case FHT::Context::_Class:
			{
				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_identifier_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_identifier_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_identifier_end_pos);
				out_token_p._vocabulary = Vocabulary::_ClassIdentifier;
				context_stack_p.emplace_back(FHT::Context::_ClassBody);
				return;
			}
			break;


		case FHT::Context::_ClassExtension:
			{
				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_identifier_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_identifier_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_identifier_end_pos);
				out_token_p._vocabulary = Vocabulary::_BaseClassIdentifier;
				return;
			}
			return;


		case FHT::Context::_Template:
			return;


		default:
			{
				//tokenize_class_struct_enum_forward_decl_and_using_namespace(out_token_p, code_iterator_p, context_stack_p);
				//if (out_token_p._vocabulary != Vocabulary::_Undefined)
				//{
				//	return; // return if the text is a forward declaration.
				//}

				while (*code_iterator_p <= ' ')
				{
					++code_iterator_p;
				}

				var::uint64 l_struct_keyword_end_pos = 0;
				for (auto it = code_iterator_p; is_a_valid_letter_for_identifiers(*it); ++it)
				{
					++l_struct_keyword_end_pos;
				}
				out_token_p._code.assign(code_iterator_p, l_struct_keyword_end_pos);

				if (out_token_p._code.starts_with(u8"class"))
				{
					out_token_p._vocabulary = Vocabulary::_Class;
					context_stack_p.emplace_back(FHT::Context::_Class);
					out_token_p._code.resize(FE::algorithm::string::compiletime::length(u8"class"));
					return;
				}

				if (context_stack_p.back() == FHT::Context::_Attribute)
				{
					context_stack_p.pop_back();
					if (context_stack_p.back() == FHT::Context::_ProbablyAttribute)
					{
						context_stack_p.pop_back();
					}
					out_token_p._vocabulary = Vocabulary::_Attribute;
					return;
				}

				out_token_p._code.clear();
				return;
			}
			break;
		}
	}

	void tokenize_class_struct_enum_forward_decl_and_using_namespace(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		while (*code_iterator_p != '{')
		{
			if (*code_iterator_p == '\0')
			{
				return;
			}

			if (*code_iterator_p == ';')
			{
				out_token_p._vocabulary = Vocabulary::_ClassStructEnumMethodForwardDeclaration;
				return;
			}

			out_token_p._code += *code_iterator_p;
			++code_iterator_p;
		}
		out_token_p._code.clear();
	}

	void tokenize_template_body(token& out_token_p, typename file_buffer_t::const_pointer code_iterator_p, FHT::context_stack_t& context_stack_p)
	{
		if (context_stack_p.back() == FHT::Context::_Template)
		{
			out_token_p._vocabulary = Vocabulary::_TemplateBody;
			out_token_p._code.reserve(100);

			while (*code_iterator_p != '{')
			{
				out_token_p._code += *code_iterator_p;
				++code_iterator_p;
			}

			file_buffer_t l_brace_stack(framework::get_framework().get_memory_resource());

			token l_tmp = { ._code{ framework::get_framework().get_memory_resource()} };

			do
			{
				switch (*code_iterator_p)
				{
				case '{':
					if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
						&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
					{
						l_brace_stack.push_back('{');
					}
					break;

				case '}':
					if ((l_tmp._vocabulary != Vocabulary::_CharLiteral)
						&& (l_tmp._vocabulary != Vocabulary::_StringLiteral))
					{
						l_brace_stack.pop_back();
					}
					break;

				default:
					l_tmp._vocabulary = Vocabulary::_Undefined;
					tokenize_string_literal(l_tmp, code_iterator_p, context_stack_p);
					switch (l_tmp._vocabulary)
					{
					case Vocabulary::_CharLiteral:
						out_token_p._code += *code_iterator_p;
						++code_iterator_p;
						continue;

					case Vocabulary::_StringLiteral:
						out_token_p._code += l_tmp._code;
						code_iterator_p += l_tmp._code.length();
						l_tmp._code.clear();
						continue;

					default:
						break;
					}
					break;
				}
				out_token_p._code += *code_iterator_p;
				++code_iterator_p;
				THROW_CPP_SYNTAX_ERROR(*code_iterator_p == '\0', "C++ Code Syntax Error C1075: missing '}' in class declaration, or found an explicit null terminator \0");
			} while (l_brace_stack.size() > 0);
			context_stack_p.pop_back(); // pop the template context.
			THROW_CPP_SYNTAX_ERROR(*code_iterator_p != ';', "C++ Code Syntax Error C2143: missing ';' after class declaration");
			out_token_p._code += *code_iterator_p;
			return;
		}
	}
}