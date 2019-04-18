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

	SECTION("IntValue()")
	{
		String intInString1 = "12345";
		CHECK(intInString1.IntValue() == 12345);
		String intInString2 = "123asd";
		CHECK(intInString2.IntValue() == INT32_MAX);
		String intInString3 = "asd123";
		CHECK(intInString3.IntValue() == INT32_MAX);
		String intInString4 = "  123  ";
		CHECK(intInString4.IntValue() == INT32_MAX);
	}

	SECTION("FloatValue()")
	{
		String floatInString1 = "123.45";
		CHECK(floatInString1.FloatValue() == Approx(123.45f));
		String floatInString2 = "123";
		CHECK(floatInString2.FloatValue() == Approx(123.0f));
		String floatInString3 = "000.1200";
		CHECK(floatInString3.FloatValue() == Approx(0.12f));
		String floatInString4 = ".5";
		CHECK(floatInString4.FloatValue() == Approx(0.5f));
		String floatInString5 = "123.0asd";
		CHECK(floatInString5.FloatValue() == FLT_MAX);
		String floatInString6 = "asd123.0";
		CHECK(floatInString6.FloatValue() == FLT_MAX);
		String floatInString7 = ".123.0";
		CHECK(floatInString7.FloatValue() == FLT_MAX);
	}

	SECTION("Compare() | CompareIgnoreCase() | operator <")
	{
		String s("XXX");

		CHECK(strcmp(s.Str(), "XXX") == s.Compare("XXX"));
		CHECK(strcmp(string.Str(), s.Str()) == string.Compare(s));

		CHECK(s.Compare("XXX") == s.CompareIgnoreCase(String("xXx")));
		CHECK(s.Compare("XXX") == s.CompareIgnoreCase("xXx"));

		CHECK((string < s) == (string.Compare(s) < 0));
	}

	SECTION("StartsWith()")
	{
		CHECK(string.StartsWith(String("Str")));
		CHECK(string.StartsWith("Str"));
		CHECK(string.StartsWith(charSeq));
	}

	SECTION("EndsWith()")
	{
		CHECK(string.EndsWith(String(" ASD")));
		CHECK(string.EndsWith(" ASD"));
		CHECK(string.EndsWith(charSeq));
	}
}
