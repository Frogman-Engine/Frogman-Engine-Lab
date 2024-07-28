#ifndef _FE_HXX_VOCABULARY_H_
// Copyright © 2023~ UNKNOWN STRYKER. All Rights Reserved.

// FE core
#include <FE/core/prerequisites.h>

// robin hood
#include <robin_hood.h>




BEGIN_NAMESPACE(FE/fxx)

	
enum struct VOCABULARY
{
	_UNDEFINED, _END_OF_TOKEN,
	
	_LITERAL_TRUE, _LITERAL_FALSE, _LITERAL_NULL, _LITERAL_BINARY, _LITERAL_HEXADECIMAL, 
	_LITERAL_ASCII_CHAR, _LITERAL_UTF8_CHAR, _LITERAL_UTF16_CHAR, _LITERAL_UTF32_CHAR,
	_LITERAL_ASCII_STRING, _LITERAL_UTF8_STRING, _LITERAL_UTF16_STRING, _LITERAL_UTF32_STRING,
	_LITERAL_UI8, _LITERAL_I8, _LITERAL_UI16, _LITERAL_I16, _LITERAL_UI32, _LITERAL_I32, _LITERAL_UI64, _LITERAL_I64, _LITERAL_UI128, _LITERAL_I128,
	_LITERAL_F8, _LITERAL_F16, _LITERAL_F32, _LITERAL_F64, _LITERAL_F128,
	
	_TYPE_BYTE, _TYPE_WORD, _TYPE_DWORD, _TYPE_QWORD, _TYPE_OWORD, _TYPE_YMMWORD, _TYPE_ZMMWORD,
	_TYPE_QUALIFIER_UNSAFE, _TYPE_PTR64, _TYPE_TASK, _TYPE_THREAD, 
	_TYPE_QUALIFIER_ATOMIC, _TYPE_QUALIFIER_VOLATILE, 
	_TYPE_QUALIFIER_VAR, _TYPE_QUALIFIER_TRACKABLE,
	_SYMBOL_PTR, _SYMBOL_REF,
	_SPECIFIER_CONSTEXPR,
	_SPECIFIER_DECLTYPE,
	_SPECIFIER_ALIGN_AS,
	_STORAGE_CLASS_SPECIFIER_STATIC, _STORAGE_CLASS_SPECIFIER_THREAD_LOCAL,

	_ATTRIBUTE_MAYBE_UNUSED, _ATTRIBUTE_NODISCARD, _ATTRIBUTE_LIKELY, _ATTRIBUTE_UNLIKELY,
	_METHOD_ATTRIBUTE_CONST, 
	
	_DIRECTIVE_INLINE, _DIRECTIVE_FORCE_INLINE, 

	_KEYWORD_DELETE,
	_KEYWORD_EXTERN, _KEYWORD_FRIEND, _KEYWORD_NAMESPACE,
	_KEYWORD_IMPORT, _KEYWORD_EXPORT, _KEYWORD_USING,
	
	_ACCESS_MODIFIER_PUBLIC, _ACCESS_MODIFIER_PROTECTED, _ACCESS_MODIFIER_PRIVATE,
	
	_KEYWORD_IF, _KEYWORD_ELSE, _KEYWORD_ELIF, 
	_KEYWORD_SWITCH, _KEYWORD_CASE, _KEYWORD_DEFAULT, _KEYWORD_NODEFAULT, _KEYWORD_FALLTHROUGH,
	_KEYWORD_FOR, _KEYWORD_FOREACH,
	_KEYWORD_DO, _KEYWORD_WHILE, _KEYWORD_LOOP,
	_KEYWORD_BREAK, _KEYWORD_BREAK_ALL, 
	_KEYWORD_CONTINUE,
	_KEYWORD_RETURN,
	
	_KEYWORD_THIS, _KEYWORD_STATIC_ASSERT,
	_KEYWORD_CLASS, _KEYWORD_STRUCT, _KEYWORD_UNION, _KEYWORD_ENUM,
	
	_KEYWORD_STATIC_CAST, _KEYWORD_REINTERPRET_CAST, _KEYWORD_CONST_CAST, 
	
	_BUILT_IN_FUNCTION_LOG, _BUILT_IN_FUNCTION_VERIFY, _BUILT_IN_FUNCTION_ASSERT,

	_OPERATOR_MOVE, _OPERATOR_DYNAMIC_CAST, _OPERATOR_TYPEID, _OPERATOR_SIZEOF,
	_TERNARY_OPERATOR,
	_OPERATOR_ARROW, _OPERATOR_DOT, 
	_OPERATOR_ADD, _OPERATOR_SUBTRACT, _OPERATOR_MULTIPLY, _OPERATOR_DIVIDE, _OPERATOR_MODULO, _OPERATOR_INCREMENT, _OPERATOR_DECREMENT,
	_OPERATOR_ASSIGN, _OPERATOR_ADD_ASSIGN, _OPERATOR_SUBTRACT_ASSIGN, _OPERATOR_MULTIPLY_ASSIGN, _OPERATOR_DIVIDE_ASSIGN, _OPERATOR_MODULO_ASSIGN,
	
	_BITWISE_OPERATOR_AND, _BITWISE_OPERATOR_NAND, _BITWISE_OPERATOR_OR, _BITWISE_OPERATOR_XOR, _BITWISE_OPERATOR_RSHIFT, _BITWISE_OPERATOR_LSHIFT,
	_BITWISE_OPERATOR_AND_ASSIGN, _BITWISE_OPERATOR_NAND_ASSIGN, _BITWISE_OPERATOR_OR_ASSIGN, _BITWISE_OPERATOR_XOR_ASSIGN, _BITWISE_OPERATOR_RSHIFT_ASSIGN, _BITWISE_OPERATOR_LSHIFT_ASSIGN,
	
	_LOGICAL_NOT, _LOGICAL_AND, _LOGICAL_NAND, _LOGICAL_OR, _LOGICAL_XOR,
	
	_COMPARATOR_L_IS_GREATER_THAN_R, _COMPARATOR_L_IS_GREATER_THAN_OR_EQUAL_TO_R, _COMPARATOR_L_IS_LESS_THAN_R, _COMPARATOR_L_IS_LESS_THAN_OR_EQUAL_TO_R,
	_COMPARATOR_EQUAL, _COMPARATOR_NOT_EQUAL,
	
	_SEPERATOR_COMMA, _SEPERATOR_COLON, _SEPERATOR_SEMICOLON, _SEPERATOR_DOUBLE_COLON, 
	_SEPERATOR_LEFT_PAREN, _SEPERATOR_RIGHT_PAREN, 
	_SEPERATOR_LEFT_BRACE, _SEPERATOR_RIGHT_BRACE, 
	_SEPERATOR_LEFT_BRACKET, _SEPERATOR_RIGHT_BRACKET, 
	
	_COMMENT_BEGIN, _COMMENT_END,
	_LINE_COMMENT,
	
	_PRE_PROCESSOR, 
	_PRE_PROCESSOR_ENDIF, 
	_PRE_PROCESSOR_DEFINED, 
	_PRE_PROCESSOR_DEFINE, _PRE_PROCESSOR_UNDEF,
	_PRE_PROCESSOR_VA_ARGS,
	_PRE_PROCESSOR_MESSAGE, _PRE_PROCESSOR_WARN, _PRE_PROCESSOR_ERROR, _PRE_PROCESSOR_IGNORE,
	
	_IDENTIFIER
};


const robin_hood::unordered_map<FE::ASCII*, VOCABULARY, FE::hash<FE::ASCII*>> g_fxx_language_vocabulary_map = {
	
	{"$Undefined$", VOCABULARY::_UNDEFINED}, {"$EndOfToken$", VOCABULARY::_END_OF_TOKEN},
	{"true", VOCABULARY::_LITERAL_TRUE}, {"false", VOCABULARY::_LITERAL_FALSE}, 
	{"null", VOCABULARY::_LITERAL_NULL}, 
	{"0b", VOCABULARY::_LITERAL_BINARY}, {"0x", VOCABULARY::_LITERAL_HEXADECIMAL}, 
	{"\'\'", VOCABULARY::_LITERAL_ASCII_CHAR}, 	{"utf8\'\'", VOCABULARY::_LITERAL_UTF8_CHAR}, 	{"utf16\'\'", VOCABULARY::_LITERAL_UTF16_CHAR},		{"utf32\'\'", VOCABULARY::_LITERAL_UTF32_CHAR},
	{"\"\"", VOCABULARY::_LITERAL_ASCII_STRING},{"utf8\"\"", VOCABULARY::_LITERAL_UTF8_STRING}, {"utf16\"\"", VOCABULARY::_LITERAL_UTF16_STRING},	{"utf32\"\"", VOCABULARY::_LITERAL_UTF32_STRING},
	{"ui8", VOCABULARY::_LITERAL_UI8}, {"i8", VOCABULARY::_LITERAL_I8}, {"ui16", VOCABULARY::_LITERAL_UI16}, {"i16", VOCABULARY::_LITERAL_I16}, {"ui32", VOCABULARY::_LITERAL_UI32}, {"i32", VOCABULARY::_LITERAL_I32}, {"ui64", VOCABULARY::_LITERAL_UI64}, {"i64", VOCABULARY::_LITERAL_I64}, {"ui128", VOCABULARY::_LITERAL_UI128}, {"i128", VOCABULARY::_LITERAL_I128},
	{"f8", VOCABULARY::_LITERAL_F8}, {"f16", VOCABULARY::_LITERAL_F16}, {"f32", VOCABULARY::_LITERAL_F32}, {"f64", VOCABULARY::_LITERAL_F64}, {"f128", VOCABULARY::_LITERAL_F128},
	
	{"byte", VOCABULARY::_TYPE_BYTE}, {"word", VOCABULARY::_TYPE_WORD}, {"dword", VOCABULARY::_TYPE_DWORD}, {"qword", VOCABULARY::_TYPE_QWORD}, {"oword", VOCABULARY::_TYPE_OWORD}, {"ymmword", VOCABULARY::_TYPE_YMMWORD}, {"zmmword", VOCABULARY::_TYPE_ZMMWORD},
	{"unsafe", VOCABULARY::_TYPE_QUALIFIER_UNSAFE}, {"__ptr64", VOCABULARY::_TYPE_PTR64}, {"task", VOCABULARY::_TYPE_TASK}, {"thread", VOCABULARY::_TYPE_THREAD}, 
	
	{"atomic", VOCABULARY::_TYPE_QUALIFIER_ATOMIC}, {"trackable", VOCABULARY::_TYPE_QUALIFIER_TRACKABLE},
	{"var", VOCABULARY::_TYPE_QUALIFIER_VAR}, 		{"volatile", VOCABULARY::_TYPE_QUALIFIER_VOLATILE}, 	
	
	{"*", VOCABULARY::_SYMBOL_PTR}, {"&", VOCABULARY::_SYMBOL_REF},
	
	{"constexpr", VOCABULARY::_SPECIFIER_CONSTEXPR}, {"decltype", VOCABULARY::_SPECIFIER_DECLTYPE}, {"align_as", VOCABULARY::_SPECIFIER_ALIGN_AS},

	{"static", VOCABULARY::_STORAGE_CLASS_SPECIFIER_STATIC}, {"thread_local", VOCABULARY::_STORAGE_CLASS_SPECIFIER_THREAD_LOCAL},

	{"maybe_unused", VOCABULARY::_ATTRIBUTE_MAYBE_UNUSED}, {"nodiscard", VOCABULARY::_ATTRIBUTE_NODISCARD}, {"likely", VOCABULARY::_ATTRIBUTE_LIKELY}, {"unlikely", VOCABULARY::_ATTRIBUTE_UNLIKELY},
	{"", VOCABULARY::_METHOD_ATTRIBUTE_CONST}, 
	
	{"inline", VOCABULARY::_DIRECTIVE_INLINE}, {"force_inline", VOCABULARY::_DIRECTIVE_FORCE_INLINE}, 

	{"delete", VOCABULARY::_KEYWORD_DELETE},
	{"extern", VOCABULARY::_KEYWORD_EXTERN}, {"friend", VOCABULARY::_KEYWORD_FRIEND}, {"namespace", VOCABULARY::_KEYWORD_NAMESPACE},
	{"import", VOCABULARY::_KEYWORD_IMPORT}, {"export", VOCABULARY::_KEYWORD_EXPORT}, {"using", VOCABULARY::_KEYWORD_USING},
	
	{"public", VOCABULARY::_ACCESS_MODIFIER_PUBLIC}, {"protected", VOCABULARY::_ACCESS_MODIFIER_PROTECTED}, {"private", VOCABULARY::_ACCESS_MODIFIER_PRIVATE},
	
	{"if", VOCABULARY::_KEYWORD_IF}, 			{"else", VOCABULARY::_KEYWORD_ELSE},		{"elif", VOCABULARY::_KEYWORD_ELIF}, 
	{"switch", VOCABULARY::_KEYWORD_SWITCH},	{"case", VOCABULARY::_KEYWORD_CASE}, 		{"default", VOCABULARY::_KEYWORD_DEFAULT}, {"nodefault", VOCABULARY::_KEYWORD_NODEFAULT}, {"fallthrough", VOCABULARY::_KEYWORD_FALLTHROUGH},
	{"for", VOCABULARY::_KEYWORD_FOR}, 			{"foreach", VOCABULARY::_KEYWORD_FOREACH},
	{"do", VOCABULARY::_KEYWORD_DO}, 			{"while", VOCABULARY::_KEYWORD_WHILE}, 		{"loop", VOCABULARY::_KEYWORD_LOOP},
	{"break", VOCABULARY::_KEYWORD_BREAK}, 		{"break_all", VOCABULARY::_KEYWORD_BREAK_ALL}, 
	{"continue", VOCABULARY::_KEYWORD_CONTINUE},
	{"return", VOCABULARY::_KEYWORD_RETURN},
	
	{"this", VOCABULARY::_KEYWORD_THIS},	{"static_assert", VOCABULARY::_KEYWORD_STATIC_ASSERT},
	{"class", VOCABULARY::_KEYWORD_CLASS},	{"struct", VOCABULARY::_KEYWORD_STRUCT}, {"union", VOCABULARY::_KEYWORD_UNION}, {"enum", VOCABULARY::_KEYWORD_ENUM},
	
	{"static_cast", VOCABULARY::_KEYWORD_STATIC_CAST}, {"reinterpret_cast", VOCABULARY::_KEYWORD_REINTERPRET_CAST}, {"const_cast", VOCABULARY::_KEYWORD_CONST_CAST}, 
	
	{"LOG", VOCABULARY::_BUILT_IN_FUNCTION_LOG}, {"VERIFY", VOCABULARY::_BUILT_IN_FUNCTION_VERIFY}, {"ASSERT", VOCABULARY::_BUILT_IN_FUNCTION_ASSERT},

	{"move", VOCABULARY::_OPERATOR_MOVE}, {"dynamic_cast", VOCABULARY::_OPERATOR_DYNAMIC_CAST}, {"typeid", VOCABULARY::_OPERATOR_TYPEID}, {"sizeof", VOCABULARY::_OPERATOR_SIZEOF},
	{"()?:", VOCABULARY::_TERNARY_OPERATOR},
	{"->", VOCABULARY::_OPERATOR_ARROW}, 	{".", VOCABULARY::_OPERATOR_DOT}, 
	{"+", VOCABULARY::_OPERATOR_ADD}, 		{"-", VOCABULARY::_OPERATOR_SUBTRACT},		{"*", VOCABULARY::_OPERATOR_MULTIPLY}, 			{"/", VOCABULARY::_OPERATOR_DIVIDE}, 		 	{"%", VOCABULARY::_OPERATOR_MODULO},			{"++", VOCABULARY::_OPERATOR_INCREMENT},	{"--", VOCABULARY::_OPERATOR_DECREMENT},
	{"=", VOCABULARY::_OPERATOR_ASSIGN}, 	{"+=", VOCABULARY::_OPERATOR_ADD_ASSIGN},	{"-=", VOCABULARY::_OPERATOR_SUBTRACT_ASSIGN}, 	{"*=", VOCABULARY::_OPERATOR_MULTIPLY_ASSIGN},	{"/=", VOCABULARY::_OPERATOR_DIVIDE_ASSIGN},	{"%=", VOCABULARY::_OPERATOR_MODULO_ASSIGN},
	
	{"&", VOCABULARY::_BITWISE_OPERATOR_AND},			{"|", VOCABULARY::_BITWISE_OPERATOR_OR},			{"^", VOCABULARY::_BITWISE_OPERATOR_XOR},			{">>", VOCABULARY::_BITWISE_OPERATOR_RSHIFT},			{"<<", VOCABULARY::_BITWISE_OPERATOR_LSHIFT},
	{"&=", VOCABULARY::_BITWISE_OPERATOR_AND_ASSIGN}, 	{"|=", VOCABULARY::_BITWISE_OPERATOR_OR_ASSIGN}, 	{"^=", VOCABULARY::_BITWISE_OPERATOR_XOR_ASSIGN}, 	{">>=", VOCABULARY::_BITWISE_OPERATOR_RSHIFT_ASSIGN}, 	{"<<=", VOCABULARY::_BITWISE_OPERATOR_LSHIFT_ASSIGN},
	
	{"not", VOCABULARY::_LOGICAL_NOT}, {"and", VOCABULARY::_LOGICAL_AND}, {"nand", VOCABULARY::_LOGICAL_NAND}, {"or", VOCABULARY::_LOGICAL_OR}, {"xor", VOCABULARY::_LOGICAL_XOR},
	
	{">", VOCABULARY::_COMPARATOR_L_IS_GREATER_THAN_R}, {">=", VOCABULARY::_COMPARATOR_L_IS_GREATER_THAN_OR_EQUAL_TO_R}, {"<", VOCABULARY::_COMPARATOR_L_IS_LESS_THAN_R}, {"<=", VOCABULARY::_COMPARATOR_L_IS_LESS_THAN_OR_EQUAL_TO_R},
	{"==", VOCABULARY::_COMPARATOR_EQUAL},				{"!=", VOCABULARY::_COMPARATOR_NOT_EQUAL},
	
	{",", VOCABULARY::_SEPERATOR_COMMA}, 		{":", VOCABULARY::_SEPERATOR_COLON}, 		{";", VOCABULARY::_SEPERATOR_SEMICOLON}, {"::", VOCABULARY::_SEPERATOR_DOUBLE_COLON}, 
	{"(", VOCABULARY::_SEPERATOR_LEFT_PAREN},   {")", VOCABULARY::_SEPERATOR_RIGHT_PAREN}, 
	{"{", VOCABULARY::_SEPERATOR_LEFT_BRACE},   {"}", VOCABULARY::_SEPERATOR_RIGHT_BRACE}, 
	{"[", VOCABULARY::_SEPERATOR_LEFT_BRACKET},	{"]", VOCABULARY::_SEPERATOR_RIGHT_BRACKET}, 
	
	{"/*", VOCABULARY::_COMMENT_BEGIN}, {"*/", VOCABULARY::_COMMENT_END},
	{"//", VOCABULARY::_LINE_COMMENT},
	
	{"#", VOCABULARY::_PRE_PROCESSOR}, 
	{"endif", VOCABULARY::_PRE_PROCESSOR_ENDIF}, 
	{"defined", VOCABULARY::_PRE_PROCESSOR_DEFINED}, 
	{"define", VOCABULARY::_PRE_PROCESSOR_DEFINE},  {"undef", VOCABULARY::_PRE_PROCESSOR_UNDEF},
	{"__VA_ARGS__", VOCABULARY::_PRE_PROCESSOR_VA_ARGS},
	{"message", VOCABULARY::_PRE_PROCESSOR_MESSAGE}, {"warn", VOCABULARY::_PRE_PROCESSOR_WARN}, {"error", VOCABULARY::_PRE_PROCESSOR_ERROR}, {"ignore", VOCABULARY::_PRE_PROCESSOR_IGNORE},
	
	{"$identifier$", VOCABULARY::_IDENTIFIER}
};

END_NAMESPACE
#endif