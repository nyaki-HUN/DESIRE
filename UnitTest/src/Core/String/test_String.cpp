#include "stdafx.h"
#include "Engine/Core/String/String.h"

TEST_CASE("String", "[Core]")
{
	const char charSeq[] = "String ASD";
	String string = charSeq;

	SECTION("Constructor | Equals()")
	{
		// Default empty string
		CHECK(strcmp(String::kEmptyString.Str(), "") == 0);
		CHECK(String::kEmptyString.Equals(""));

		// From char sequence
		String strCharSequence(charSeq, strlen(charSeq));
		CHECK(strcmp(strCharSequence.Str(), charSeq) == 0);
		CHECK(strCharSequence.Equals(charSeq));

		// From string literal (the pointer has to be the same)
		String str("stringLiteral");
		CHECK(str.Str() == static_cast<const char*>("stringLiteral"));
		CHECK(string.Str() == charSeq);
	}

	SECTION("Find()")
	{
		DESIRE_TODO("Add test cases");
//		size_t Find(const String& search, size_t startIndex = 0) const;
//		size_t Find(const char *search, size_t startIndex = 0) const;
//		size_t Find(char search, size_t startIndex = 0) const;
	}

	SECTION("FindLast()")
	{
		DESIRE_TODO("Add test cases");
//		size_t FindLast(const String& search) const;
//		size_t FindLast(const char *search) const;
//		size_t FindLast(char search) const;
	}

	SECTION("Length()")
	{
		CHECK(string.Length() == strlen(charSeq));
		CHECK(string.Length() == sizeof(charSeq) - 1);

		String stringWithNullSeparator = "Word0\0Word1";
		CHECK(stringWithNullSeparator.Length() != strlen("Word0\0Word1"));
		CHECK(stringWithNullSeparator.Length() == sizeof("Word0\0Word1") - 1);
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
		CHECK(intInString2.AsInt32() == 123);
		String intInString3 = "asd123";
		CHECK(intInString3.AsInt32() == 0);
		String intInString4 = "  123  ";
		CHECK(intInString4.AsInt32() == 123);
	}

	SECTION("AsInt64()")
	{
		String intInString1 = "0x123456789";
		CHECK(intInString1.AsInt64() == 0x123456789);
		String intInString2 = "123asd";
		CHECK(intInString2.AsInt64() == 123);
		String intInString3 = "asd123";
		CHECK(intInString3.AsInt64() == 0);
		String intInString4 = "  123  ";
		CHECK(intInString4.AsInt64() == 123);
	}

	SECTION("AsInt64()")
	{
		String intInString1 = "-12345";
		CHECK(intInString1.AsInt64() == -12345);
		String intInString2 = "123asd";
		CHECK(intInString2.AsInt64() == 123);
		String intInString3 = "asd123";
		CHECK(intInString3.AsInt64() == 0);
		String intInString4 = "  123  ";
		CHECK(intInString4.AsInt64() == 123);
	}

	SECTION("AsUint64()")
	{
		String intInString1 = "0xffffffffffffffff";
		CHECK(intInString1.AsUint64() == 0xffffffffffffffffu);
		String intInString2 = "123asd";
		CHECK(intInString2.AsUint64() == 123);
		String intInString3 = "asd123";
		CHECK(intInString3.AsUint64() == 0);
		String intInString4 = "  123  ";
		CHECK(intInString4.AsUint64() == 123);
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
		CHECK(floatInString5.AsFloat() == Approx(123.0f));
		String floatInString6 = "asd123.0";
		CHECK(floatInString6.AsFloat() == Approx(0.0f));
		String floatInString7 = ".123.0";
		CHECK(floatInString7.AsFloat() == Approx(0.123f));
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
		CHECK(floatInString5.AsDouble() == Approx(123.0));
		String floatInString6 = "asd123.0";
		CHECK(floatInString6.AsDouble() == Approx(0.0));
		String floatInString7 = ".123.0";
		CHECK(floatInString7.AsDouble() == Approx(0.123));
	}

	SECTION("Compare() | operator <()")
	{
		String s("XXX");

		CHECK(s.Compare("XXX") == 0);
		CHECK(string.Compare(s) < 0);
		CHECK(string < s);

		String stringWithNullSeparator = "Word0\0Word1";
		REQUIRE(strcmp(stringWithNullSeparator.Str(), "Word0") == 0);
		CHECK(stringWithNullSeparator.Compare("Word0") != 0);
	}

	SECTION("StartsWith()")
	{
		CHECK(string.StartsWith("Str"));
		CHECK(string.StartsWith('S'));
		CHECK(string.StartsWith(charSeq));
		CHECK_FALSE(string.StartsWith(String::kEmptyString));

		String stringWithNullSeparator = "Word0\0Word1";
		CHECK(stringWithNullSeparator.StartsWith("Word0"));
		CHECK(stringWithNullSeparator.StartsWith("Word0\0Word1"));
	}

	SECTION("EndsWith()")
	{
		CHECK(string.EndsWith(" ASD"));
		CHECK(string.EndsWith('D'));
		CHECK(string.EndsWith(charSeq));
		CHECK_FALSE(string.EndsWith(String::kEmptyString));

		String stringWithNullSeparator = "Word0\0Word1";
		CHECK(stringWithNullSeparator.EndsWith("Word1"));
		CHECK(stringWithNullSeparator.EndsWith("Word0\0Word1"));
	}

	SECTION("SubString()")
	{
		DESIRE_TODO("Add test cases");
//		String SubString(size_t pos) const;
	}
}
