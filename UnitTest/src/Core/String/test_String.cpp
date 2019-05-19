#include "stdafx.h"
#include "Engine/Core/String/String.h"

TEST_CASE("String", "[Core]")
{
	const char charSeq[] = "String ASD";
	String string = charSeq;

	SECTION("Constructor | Equals()")
	{
		// From char sequence
		CHECK(strcmp(string.Str(), charSeq) == 0);
		CHECK(string.Equals(charSeq));

		// Default empty string
		CHECK(strcmp(String::kEmptyString.Str(), "") == 0);
		CHECK(String::kEmptyString.Equals(""));

		// From string literal (the pointer has to be the same)
		String str("stringLiteral");
		CHECK(str.Str() == (const char*)"stringLiteral");
	}

	SECTION("Find()")
	{
		// TODO: Add test cases
//		size_t Find(const String& search, size_t startIndex = 0) const;
//		size_t Find(const char *search, size_t startIndex = 0) const;
//		size_t Find(char search, size_t startIndex = 0) const;
	}
	
	SECTION("FindLast()")
	{
		// TODO: Add test cases
//		size_t FindLast(const String& search) const;
//		size_t FindLast(const char *search) const;
//		size_t FindLast(char search) const;
	}

	SECTION("Length()")
	{
		CHECK(string.Length() == strlen(charSeq));
	}

	SECTION("LengthUTF8()")
	{
		String utf8Str = u8"\u20AC \U0001F34C";
		CHECK(utf8Str.Length() == 8);
		CHECK(utf8Str.LengthUTF8() == 3);
	}

	SECTION("AsInt32()")
	{
		String intInString1 = "-12345";
		CHECK(intInString1.AsInt32() == -12345);
		String intInString2 = "123asd";
		CHECK(intInString2.AsInt32() == INT32_MAX);
		String intInString3 = "asd123";
		CHECK(intInString3.AsInt32() == INT32_MAX);
		String intInString4 = "  123  ";
		CHECK(intInString4.AsInt32() == INT32_MAX);
	}

	SECTION("AsInt64()")
	{
		String intInString1 = "0x123456789";
		CHECK(intInString1.AsInt64() == 0x123456789);
		String intInString2 = "123asd";
		CHECK(intInString2.AsInt64() == INT64_MAX);
		String intInString3 = "asd123";
		CHECK(intInString3.AsInt64() == INT64_MAX);
		String intInString4 = "  123  ";
		CHECK(intInString4.AsInt64() == INT64_MAX);
	}

	SECTION("AsInt64()")
	{
		String intInString1 = "-12345";
		CHECK(intInString1.AsInt64() == -12345);
		String intInString2 = "123asd";
		CHECK(intInString2.AsInt64() == INT64_MAX);
		String intInString3 = "asd123";
		CHECK(intInString3.AsInt64() == INT64_MAX);
		String intInString4 = "  123  ";
		CHECK(intInString4.AsInt64() == INT64_MAX);
	}

	SECTION("AsUint64()")
	{
		String intInString1 = "0xffffffffffffffff";
		CHECK(intInString1.AsUint64() == 0xffffffffffffffffu);
		String intInString2 = "123asd";
		CHECK(intInString2.AsUint64() == UINT64_MAX);
		String intInString3 = "asd123";
		CHECK(intInString3.AsUint64() == UINT64_MAX);
		String intInString4 = "  123  ";
		CHECK(intInString4.AsUint64() == UINT64_MAX);
	}

	SECTION("AsFloat()")
	{
		String floatInString1 = "123.45";
		CHECK(floatInString1.AsFloat() == Approx(123.45f));
		String floatInString2 = "123";
		CHECK(floatInString2.AsFloat() == Approx(123.0f));
		String floatInString3 = "000.1200";
		CHECK(floatInString3.AsFloat() == Approx(0.12f));
		String floatInString4 = ".5";
		CHECK(floatInString4.AsFloat() == Approx(0.5f));
		String floatInString5 = "123.0asd";
		CHECK(floatInString5.AsFloat() == FLT_MAX);
		String floatInString6 = "asd123.0";
		CHECK(floatInString6.AsFloat() == FLT_MAX);
		String floatInString7 = ".123.0";
		CHECK(floatInString7.AsFloat() == FLT_MAX);
	}

	SECTION("AsDouble()")
	{
		String floatInString1 = "123.45";
		CHECK(floatInString1.AsDouble() == Approx(123.45));
		String floatInString2 = "123";
		CHECK(floatInString2.AsDouble() == Approx(123.0));
		String floatInString3 = "000.1200";
		CHECK(floatInString3.AsDouble() == Approx(0.12));
		String floatInString4 = ".5";
		CHECK(floatInString4.AsDouble() == Approx(0.5));
		String floatInString5 = "123.0asd";
		CHECK(floatInString5.AsDouble() == DBL_MAX);
		String floatInString6 = "asd123.0";
		CHECK(floatInString6.AsDouble() == DBL_MAX);
		String floatInString7 = ".123.0";
		CHECK(floatInString7.AsDouble() == DBL_MAX);
	}

	SECTION("Compare() | CompareIgnoreCase() | operator <()")
	{
		String s("XXX");

		CHECK(s.Compare("XXX") == 0);
		CHECK(string.Compare(s) < 0);
		CHECK(string < s);

		CHECK(s.Compare("XXX") == s.CompareIgnoreCase(String("xXx")));
		CHECK(s.Compare("XXX") == s.CompareIgnoreCase("xXx"));
	}

	SECTION("StartsWith()")
	{
		CHECK(string.StartsWith(String("Str")));
		CHECK(string.StartsWith("Str!!!", 3));
		CHECK(string.StartsWith(charSeq));
		CHECK_FALSE(string.StartsWith(String::kEmptyString));
	}

	SECTION("EndsWith()")
	{
		CHECK(string.EndsWith(String(" ASD")));
		CHECK(string.EndsWith(" ASD!!!", 4));
		CHECK(string.EndsWith(charSeq));
		CHECK_FALSE(string.EndsWith(String::kEmptyString));
	}
}
