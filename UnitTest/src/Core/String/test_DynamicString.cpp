#include "stdafx.h"
#include "Engine/Core/String/DynamicString.h"

TEST_CASE("DynamicString", "[Core]")
{
	const char charSeq[] = "String ASD";
	DynamicString string = charSeq;

	SECTION("Constructor | Equals()")
	{
		DynamicString stringEmpty;
		CHECK(stringEmpty.Equals(""));

		// From char sequence
		CHECK(strcmp(string.Str(), charSeq) == 0);
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

	SECTION("operator =()")
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

	SECTION("Reset()")
	{
		string.Reset();
		CHECK(string.Equals(""));
	}

	SECTION("Reserve()")
	{
		// TODO: Add test cases
	}
}

TEST_CASE("WritableString", "[Core]")
{
	const char charSeq[] = "String ASD";
	DynamicString string = charSeq;

	SECTION("Set()")
	{
		string.Set(charSeq, 4);
		CHECK(string.Equals("Stri"));
	}

	SECTION("Clear()")
	{
		string.Clear();
		CHECK(string.Length() == 0);
		CHECK(string.Equals(""));
		CHECK(string == String::kEmptyString);
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

	SECTION("TruncateAt()")
	{
		string.TruncateAt(6);
		CHECK(string.Length() == 6);
		CHECK(string.Equals("String"));
	}

	SECTION("Replace() | ReplaceAll()")
	{
		DynamicString replaceStr = "aabbc";
		replaceStr.Replace("b", "XX");
		CHECK(replaceStr.Equals("aaXXbc"));
		CHECK(replaceStr.Length() == 6);

		replaceStr.Replace("XX", "Y");
		CHECK(replaceStr.Equals("aaYbc"));
		CHECK(replaceStr.Length() == 5);

		replaceStr.Replace("Ybc", "Z");
		CHECK(replaceStr.Equals("aaZ"));
		CHECK(replaceStr.Length() == 3);

		string.ReplaceAll("S", "XX");
		CHECK(string.Equals("XXtring AXXD"));
	}

	SECTION("ReplaceAllChar()")
	{
		DynamicString replaceStr = "aabbc";
		replaceStr.ReplaceAllChar('\0', 'X');
		CHECK(strcmp(replaceStr.Str(), "aabbc") == 0);
		CHECK(replaceStr.Equals("aabbc"));

		replaceStr.ReplaceAllChar('b', 'X');
		CHECK(replaceStr.Equals("aaXXc"));

		// When replacing to the null-character the string has to be truncated
		string.ReplaceAllChar(' ', '\0');
		CHECK(string.Length() == 6);
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
		numberString += 654.321;
		CHECK(numberString.Equals("String ASD-1212345-123456789123456789123456789123456789654.321654.321"));
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

	SECTION("ToLower()")
	{
		string.ToLower();
		CHECK(string.Equals("string asd"));

		DynamicString s = "123 ABCDEFGHIJKLMNOPQRSTUVWXYZ !@#";
		s.ToLower();
		CHECK(s.Equals("123 abcdefghijklmnopqrstuvwxyz !@#"));
	}

	SECTION("ToUpper()")
	{
		string.ToUpper();
		CHECK(string.Equals("STRING ASD"));

		DynamicString s = "123 abcdefghijklmnopqrstuvwxyz !@#";
		s.ToUpper();
		CHECK(s.Equals("123 ABCDEFGHIJKLMNOPQRSTUVWXYZ !@#"));
	}

	SECTION("AsCharBufferWithSize()")
	{
		char* str = string.AsCharBufferWithSize(6);
		str[0] = 'X';
		CHECK(string.Equals("Xtring"));

		str = string.AsCharBufferWithSize(10);
		memcpy(str, "1234567", 7);
		CHECK(string.Equals("1234567ASD"));
	}

	SECTION("Sprintf()")
	{
		DynamicString s;
		s.Sprintf("%d %05d %.1f %c %s", 123, 123, 1.5f, 'X', "test");
		CHECK(s.Equals("123 00123 1.5 X test"));
		s.Sprintf("no args");
		CHECK(s.Equals("no args"));
	}

	SECTION("SprintfAppend()")
	{
		string.SprintfAppend(" %d", 123);
		CHECK(string.Equals("String ASD 123"));
		string.SprintfAppend(" %05d", 123);
		CHECK(string.Equals("String ASD 123 00123"));
		string.SprintfAppend(" %.1f", 1.5f);
		CHECK(string.Equals("String ASD 123 00123 1.5"));
		string.SprintfAppend(" %c", 'X');
		CHECK(string.Equals("String ASD 123 00123 1.5 X"));
		string.SprintfAppend(" %s", "test");
		CHECK(string.Equals("String ASD 123 00123 1.5 X test"));
	}
}
