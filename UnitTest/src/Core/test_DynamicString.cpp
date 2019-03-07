#include "stdafx.h"
#include "Engine/Core/DynamicString.h"

TEST_CASE("DynamicString", "[Core]")
{
	const char charSeq[] = "String ASD";
	DynamicString string = charSeq;

	SECTION("Constructors")
	{
		DynamicString stringEmpty;
		CHECK(stringEmpty.Equals(""));

		DynamicString stringWithReservedSize(10);
		CHECK(stringWithReservedSize.Equals(""));

		// From char sequence
		CHECK(string.Equals(charSeq));

		// From char sequence with size
		DynamicString s(charSeq, 3);
		CHECK(s.Equals("Str"));

		// Copy constructor
		DynamicString stringCopyConstructed = string;
		CHECK(stringCopyConstructed.Equals(charSeq));

		// Move constructor
		DynamicString stringMoveConstructed = std::move(string);
		CHECK(stringMoveConstructed.Equals(charSeq));
	}

	SECTION("operator =")
	{
		DynamicString s;

		// Copy operator
		String xxx("xxx");
		s = xxx;
		CHECK(s.Equals(xxx));

		s = string;
		CHECK(s.Equals(charSeq));

		// Move operator
		s = std::move(string);
		CHECK(s.Equals(charSeq));
	}

	SECTION("SubString()")
	{
		// TODO: Add test cases
//		DynamicString SubString(size_t startIndex, size_t numChars = SIZE_MAX) const;
	}

	SECTION("CreateFormattedString()")
	{
		string = DynamicString::CreateFormattedString("%d %05d %.1f %c %s", 123, 123, 1.5f, 'X', "test");
		CHECK(string.Equals("123 00123 1.5 X test"));
		string = DynamicString::CreateFormattedString("no args");
		CHECK(string.Equals("no args"));
	}

	SECTION("operator +()")
	{
		const DynamicString operator_plus = string + String("123");
		CHECK(operator_plus.Equals("String ASD123"));
	}
}

TEST_CASE("WritableString", "[Core]")
{
	const char charSeq[] = "String ASD";
	DynamicString string = charSeq;

	SECTION("ToLower()")
	{
		string.ToLower();
		CHECK(string.Equals("string asd"));

		DynamicString s = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		s.ToLower();
		CHECK(s.Equals("abcdefghijklmnopqrstuvwxyz"));
	}

	SECTION("ToUpper()")
	{
		string.ToUpper();
		CHECK(string.Equals("STRING ASD"));

		DynamicString s = "abcdefghijklmnopqrstuvwxyz";
		s.ToUpper();
		CHECK(s.Equals("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
	}

	SECTION("Trim()")
	{
		DynamicString trimString = "  A SD ";
		trimString.Trim();
		trimString += "123";
		CHECK(strcmp(trimString.Str(), "A SD123") == 0);

		trimString = " \r\n \t ASD\t \r\n \t";
		trimString.Trim();
		CHECK(trimString.Equals("ASD"));

		trimString = "   ";
		trimString.Trim();
		CHECK(trimString.Equals(""));
		CHECK(trimString.Length() == 0);
	}

	SECTION("Insert()")
	{
		string.Insert(0, string);
		CHECK(string.Equals("String ASDString ASD"));
		string.Insert(0, charSeq);
		CHECK(string.Equals("String ASDString ASDString ASD"));
		string.Insert(6, charSeq, 3);
		CHECK(string.Equals("StringStr ASDString ASDString ASD"));
		string.Insert(string.Length(), charSeq, 3);
		CHECK(string.Equals("StringStr ASDString ASDString ASDStr"));
	}

	SECTION("RemoveFrom()")
	{
		string.RemoveFrom(0, 3);
		CHECK(string.Equals("ing ASD"));

		string.RemoveFrom(string.Length(), 3);
		CHECK(string.Equals("ing ASD"));

		string = charSeq;
		string.RemoveFrom(5, 3);
		CHECK(string.Equals("StrinSD"));

		string = charSeq;
		string.RemoveFrom(3);
		CHECK(string.Equals("Str"));
	}

	SECTION("RemoveFromEnd()")
	{
		string.RemoveFromEnd(4);
		CHECK(strcmp(string.Str(), "String") == 0);

		string = charSeq;
		string.RemoveFromEnd(100);
		CHECK(strcmp(string.Str(), "") == 0);
	}

	SECTION("Replace() | ReplaceAll()")
	{
		DynamicString replaceStr = "aabbc";
		replaceStr.Replace("b", "XX");
		CHECK(replaceStr.Equals("aaXXbc"));

		string.ReplaceAll("S", "XX");
		CHECK(string.Equals("XXtring AXXD"));
	}

	SECTION("ReplaceAllChar()")
	{
		DynamicString replaceStr = "aabbc";
		replaceStr.ReplaceAllChar('b', 'X');
		CHECK(replaceStr.Equals("aaXXc"));

		string.ReplaceAllChar(' ', '\0');
		CHECK(string.Equals("String"));
	}

	SECTION("Append() | operator +=()")
	{
		string += " XYZ";
		CHECK(string.Equals("String ASD XYZ"));
		CHECK(string.Length() == 14);

		DynamicString numberString = charSeq;
		numberString += -12;
		CHECK(numberString.Equals("String ASD-12"));
		numberString += 12345u;
		CHECK(numberString.Equals("String ASD-1212345"));
		numberString += -123456789123456789ll;
		CHECK(numberString.Equals("String ASD-1212345-123456789123456789"));
		numberString += 123456789123456789ull;
		CHECK(numberString.Equals("String ASD-1212345-123456789123456789123456789123456789"));
		numberString += 654.321f;
		CHECK(numberString.Equals("String ASD-1212345-123456789123456789123456789123456789654.321"));
	}
}