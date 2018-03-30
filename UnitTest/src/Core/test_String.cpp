#include "stdafx.h"
#include "Engine/Core/String.h"

TEST_CASE("String", "[Core]")
{
	// The string variable represents a String which stores its data in the stack
	const char charSeq[] = "String ASD";
	String string = charSeq;

	// The bigString variable represents a String wich contains more data than String::kStackSize and stores it in the heap
	const char charSeq2[] = "String1234567890 QWE1234567890 asd1234567890";
	String bigString = charSeq2;

	SECTION("Constructors | Equals()")
	{
		String stringEmpty;
		CHECK(strcmp(stringEmpty.c_str(), "") == 0);
		CHECK(stringEmpty.Equals(""));

		// From char sequence
		CHECK(strcmp(string.c_str(), charSeq) == 0);
		CHECK(strcmp(bigString.c_str(), charSeq2) == 0);
		CHECK(string.Equals(charSeq));
		CHECK(bigString.Equals(charSeq2));

		// From char sequence with size
		String s(charSeq, 3);
		String s2(charSeq2, 34);
		CHECK(strcmp(s.c_str(), "Str") == 0);
		CHECK(strcmp(s2.c_str(), "String1234567890 QWE1234567890 asd") == 0);
		CHECK(s.Equals("Str"));
		CHECK(s2.Equals("String1234567890 QWE1234567890 asd"));

		// Copy constructor
		String stringCopyConstructed = string;
		String bigStringCopyConstructed = bigString;
		CHECK(strcmp(stringCopyConstructed.c_str(), charSeq) == 0);
		CHECK(strcmp(bigStringCopyConstructed.c_str(), charSeq2) == 0);
		CHECK(stringCopyConstructed.Equals(charSeq));
		CHECK(bigStringCopyConstructed.Equals(charSeq2));

		// Move constructor
		String stringMoveConstructed = std::move(string);
		String bigStringMoveConstructed = std::move(bigString);
		CHECK(strcmp(stringMoveConstructed.c_str(), charSeq) == 0);
		CHECK(strcmp(bigStringMoveConstructed.c_str(), charSeq2) == 0);
		CHECK(stringMoveConstructed.Equals(charSeq));
		CHECK(bigStringMoveConstructed.Equals(charSeq2));
	}

	SECTION("operator =")
	{
		String s;

		// Copy operator
		s = charSeq;
		CHECK(s.Equals(charSeq));
		s = bigString;
		CHECK(s.Equals(charSeq2));

		s = string;
		CHECK(s.Equals(charSeq));
		s = bigString;
		CHECK(s.Equals(charSeq2));

		// Move operator
		s = std::move(string);
		CHECK(s.Equals(charSeq));
		s = std::move(bigString);
		CHECK(s.Equals(charSeq2));
	}

	SECTION("ToLower() | EqualsIgnoreCase()")
	{
		string.ToLower();
		CHECK(string.Equals("string asd"));
		CHECK(string.EqualsIgnoreCase(charSeq));

		bigString.ToLower();
		CHECK(bigString.Equals("string1234567890 qwe1234567890 asd1234567890"));
		CHECK(bigString.EqualsIgnoreCase(charSeq2));
	}

	SECTION("ToUpper() | EqualsIgnoreCase()")
	{
		string.ToUpper();
		CHECK(string.Equals("STRING ASD"));
		CHECK(string.EqualsIgnoreCase(charSeq));

		bigString.ToUpper();
		CHECK(bigString.Equals("STRING1234567890 QWE1234567890 ASD1234567890"));
		CHECK(bigString.EqualsIgnoreCase(charSeq2));
	}

	SECTION("Trim()")
	{
		String trimString = "  A SD ";
		trimString.Trim();
		trimString += "123";
		CHECK(strcmp(trimString.c_str(), "A SD123") == 0);

		trimString = " \r\n \t ASD\t \r\n \t";
		trimString.Trim();
		CHECK(trimString.Equals("ASD"));

		trimString = "   ";
		trimString.Trim();
		CHECK(trimString.Equals(""));
		CHECK(trimString.Length() == 0);
	}

	SECTION("operator +() | operator +=() | Append()")
	{
		const String operator_plus = string + "123";
		CHECK(operator_plus.Equals("String ASD123"));
		const String operator_plus2 = operator_plus + string;
		CHECK(operator_plus2.Equals("String ASD123String ASD"));

		String numberString = string;
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

		String string_copy = string;
		String bigString_copy = bigString;

		string += " XYZ";
		bigString += " Append";
		CHECK(string.Equals("String ASD XYZ"));
		CHECK(string.Length() == 14);
		CHECK(bigString.Equals("String1234567890 QWE1234567890 asd1234567890 Append"));

		string += bigString;
		CHECK(string.Equals("String ASD XYZString1234567890 QWE1234567890 asd1234567890 Append"));
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
		CHECK(strcmp(string.c_str(), "String") == 0);

		string = charSeq;
		string.RemoveFromEnd(100);
		CHECK(strcmp(string.c_str(), "") == 0);
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

	SECTION("Replace() | ReplaceAll()")
	{
		String replaceStr = "aabbc";
		replaceStr.Replace("b", "XX");
		CHECK(replaceStr.Equals("aaXXbc"));

		string.ReplaceAll("S", "XX");
		CHECK(string.Equals("XXtring AXXD"));

		bigString.Replace(" ", "");
		CHECK(bigString.Equals("String1234567890QWE1234567890 asd1234567890"));
		bigString.ReplaceAll("234", "X");
		CHECK(bigString.Equals("String1X567890QWE1X567890 asd1X567890"));
	}

	SECTION("ReplaceAllChar()")
	{
		String replaceStr = "aabbc";
		replaceStr.ReplaceAllChar('b', 'X');
		CHECK(replaceStr.Equals("aaXXc"));

		bigString.ReplaceAllChar(' ', '\0');
		CHECK(bigString.Equals("String1234567890"));
	}

	SECTION("SubString()")
	{
		// TODO: Add test cases
//		String SubString(size_t start, size_t numChars = SIZE_MAX) const;
	}

	SECTION("Length()")
	{
		CHECK(string.Length() == strlen(charSeq));
		CHECK(bigString.Length() == strlen(charSeq2));
	}

	SECTION("LengthUTF8()")
	{
		String utf8Str = u8"\u20AC \U0001F34C";
		CHECK(utf8Str.Length() == 8);
		CHECK(utf8Str.LengthUTF8() == 3);
	}

	SECTION("IntValue()")
	{
		String intInString = "12345";
		CHECK(intInString.IntValue() == 12345);
		intInString = "123asd";
		CHECK(intInString.IntValue() == INT32_MAX);
		intInString = "asd123";
		CHECK(intInString.IntValue() == INT32_MAX);
		intInString = "  123  ";
		CHECK(intInString.IntValue() == INT32_MAX);
		intInString.Trim();
		CHECK(intInString.IntValue() == 123);
	}

	SECTION("FloatValue()")
	{
		String floatInString = "  123.45  ";
		CHECK(floatInString.FloatValue() == FLT_MAX);
		floatInString.Trim();
		CHECK(floatInString.FloatValue() == Approx(123.45f));
		floatInString = "123";
		CHECK(floatInString.FloatValue() == Approx(123.0f));
		floatInString = "000.1200";
		CHECK(floatInString.FloatValue() == Approx(0.12f));
		floatInString = ".5";
		CHECK(floatInString.FloatValue() == Approx(0.5f));
		floatInString = "123.0asd";
		CHECK(floatInString.FloatValue() == FLT_MAX);
		floatInString = "asd123.0";
		CHECK(floatInString.FloatValue() == FLT_MAX);
		floatInString = ".123.0";
		CHECK(floatInString.FloatValue() == FLT_MAX);
	}

	SECTION("Compare() | CompareIgnoreCase() | operator <")
	{
		String s("XXX");

		CHECK(strcmp(string.c_str(), bigString.c_str()) == string.Compare(bigString));
		CHECK(strcmp(bigString.c_str(), s.c_str()) == bigString.Compare(s));

		CHECK(strcmp(s.c_str(), "XXX") == s.Compare("XXX"));
		CHECK(strcmp(bigString.c_str(), "XXX") == bigString.Compare("XXX"));

		CHECK(s.Compare("XXX") == s.CompareIgnoreCase(String("xXx")));
		CHECK(s.Compare("XXX") == s.CompareIgnoreCase("xXx"));

		CHECK((string < bigString) == (string.Compare(bigString) < 0));
		CHECK((bigString < s) == (bigString.Compare(s) < 0));
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

	SECTION("CreateFormattedString()")
	{
		string = String::CreateFormattedString("%d %05d %.1f %c %s", 123, 123, 1.5f, 'X', "test");
		CHECK(string.Equals("123 00123 1.5 X test"));
		string = String::CreateFormattedString("%s", charSeq2);
		CHECK(string.Equals(charSeq2));
		string = String::CreateFormattedString("no args");
		CHECK(string.Equals("no args"));
	}

	SECTION("CreateFromInt()")
	{
		string = String::CreateFromInt(123);
		CHECK(string.Equals("123"));
		string = String::CreateFromInt(-2001);
		CHECK(string.Equals("-2001"));
	}
}
