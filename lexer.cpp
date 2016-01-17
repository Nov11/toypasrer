#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

char* p;
int len;
int processed_chars;
char number[] = "-1234.56789e+1234";
char* json_string_type;

void put_back() {
	processed_chars--;
	p--;
}

char nextChar() {
	processed_chars++;
	return *p++;
}

char trimBlankChar(char* p = NULL) {
	char c = nextChar();
	while (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
		c = nextChar();
	}
	if (p != NULL) {
		*p = c;
	}
	return c;
}

char peek() {
	return *p;
}

char peek_trimed() {
	char c = trimBlankChar();
	put_back();
	return c;
}

char* currentPosition() {
	return p;
}

int no_more_chars() {
	return len - processed_chars ;
}

void error_msg(char* str = NULL) {
	printf("error occured position: chars:%s processing char:%c\n%s\n", currentPosition(), *(currentPosition() - 1), str);
}

void matchDIGIT() {
	char tmp = nextChar();
	while (isdigit(tmp)) {
		tmp = nextChar();
	}
	put_back();
}

int isNumber() {
	char c = trimBlankChar();

	/*starting minus sign*/
	if (c == '-') {
		c = nextChar();
	}
	/*0 or digits before possibly '.' */
	if (isdigit(c)) {
		if (c == '0') {
			//pass
		}
		else {
			matchDIGIT();
		}
	}
	else {
		error_msg();
		printf("isNumber error:doesn't start with digit but %c.\n", c);
		return -1;
	}
	/*fractions*/
	c = peek();
	if (c == '.') {
		nextChar();
		c = nextChar();
		if (isdigit(c)) {
			matchDIGIT();
		}
		else {
			error_msg();
			printf("isNumber error: no digit exists after period.\n");
			return -1;
		}
	}

	/*exponent*/
	c = peek();
	if (c == 'e' || c == 'E') {
		nextChar();
		c = nextChar();
		/* '-' or '+' */
		if (c == '-' || c == '+') {
			c = nextChar();
		}
		/*digits*/
		if (isdigit(c)) {
			matchDIGIT();
		}
		else {
			error_msg();
			printf("isNumber error:no digit exists after exponent sign.\n");
			return -1;
		}
	}
	return 0;
}

int isString() {
	char c = trimBlankChar(NULL);
	if (c == '"') {
		c = nextChar();
		if (c == '"') {
			printf("empty string\n");
			return 0;
		}
		while (c != '"') {
			if (c == '\\') {
				c = nextChar();
				if (c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't') {
					/*it's ok*/
				}
				else if (c == 'u') {
					for (int i = 0; i < 4; i++) {
						c = nextChar();
						if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
							/*it's hex*/
						}
						else {
							error_msg();
							return -1;
						}
					}
				}
			}
			c = nextChar();
		}
		if (c == '"') {
			/*一个string到这里结束*/
			return 0;
		}
		else {
			return -1;
		}
	}
	else {
		error_msg();
		return -1;
	}
}

int parse_array() {
	char c = trimBlankChar(NULL);
	int ret = 0;
	if (c == '[') {
		c = trimBlankChar();
		if (c == ']') {
			return 0;
		}
		put_back();
		//pasre values split by ','
		ret = isValue();
		while (ret == 0) {
			trimBlankChar(&c);
			if (c == ',') {
				ret = isValue();
			}
			else {
				put_back();
				break;
			}
		}
		if (ret != 0) {
			error_msg();
			printf("parse_array error: no element exists after comma.\n");
			return -1;
		}

		trimBlankChar(&c);
		if (c == ']') {
			return 0;
		}
		else {
			error_msg("array doesn't end with ']'.");
			return -1;
		}
	}
	else {
		error_msg();
		printf("array doesn't start with '['\n");
		return -1;
	}
}

int match(char* word) {
	char c = trimBlankChar(NULL);
	for (size_t i = 0; i < strlen(word); i++) {
		if (word[i] != c) {
			error_msg();
			printf("match word: %s \n", word);
			return -1;
		}
		else {
			c = nextChar();
		}
	}
	put_back();
	return 0;
}

int isValue() {
	char c = trimBlankChar(NULL);
	put_back();
	switch (c) {
		case '"':
			return isString();
		case '{':
			return parse_object();
		case '[':
			return parse_array();
		case 't':
			return match("true");
		case 'f':
			return match("false");
		case 'n':
			return match("null");
		default:
			return isNumber(); 
	}
}

int parse_pair() {
	char c = trimBlankChar(NULL);
	int ret = 0;
	if (c == '"') {
		put_back();
		//match a string 
		ret = isString();
		if (ret != 0) {
			printf("pasre_pair error: not a string\n");
			return -1;
		}
		trimBlankChar(&c);
		//match colon
		if (c == ':') {
			//here exists a colon
		}
		else {
			error_msg();
			printf("parsing piar. no colon follows a string\n");
			return -1;
		}
		//match a value 
		ret = isValue();
		if (ret != 0) {
			printf("pasre_pair error: not a value\n");
			return -1;
		}
		return 0;
	}
	else {
		error_msg();
		printf("pair dosen't start with double quote\n");
		return -1;
	}
}
/*
input : first call of nextChar returns char after '{' in original string
*/
int parse_members() {
	char c;
	int ret = parse_pair();
	while (ret == 0) {
		c = trimBlankChar();
		if (c == ',') {
			ret = parse_pair();
		}
		else {
			put_back();
			break;
		}
	}
	if (ret != 0) {
		error_msg();
		printf("pasre_members: there exists one comma but no pair follows it.\n");
		return -1;
	}
	return 0;
}

int parse_object() {
	char c = nextChar();
	int ret = 0;
	if(c == '{'){
		c = nextChar();
		if (c == '}') {
			return 0;
		}
		put_back();
		ret = parse_members();
		if (ret != 0) {
			error_msg("not valid members.\n");
			return -1;
		}
		c = trimBlankChar();
		if (c == '}') {
			return 0;
		}
		else {
			error_msg("object doesn't end with '}'.");
			return -1;
		}
	}
	else {
		error_msg();
		return -1;
	}
}
void check(int input, char* s) {
	if (input == 0) {
		printf("%s is valid %s\n", s, json_string_type);
	}
	else {
		printf("%s is not valid %s\n", s, json_string_type);
	}
}
void init_test(char* str) {
	p = str;
	len = strlen(p);
	processed_chars = 0;
}

int valid_json() {
	char c = trimBlankChar(NULL);
	put_back();
	switch (c) {
	case '"':
		json_string_type = "string";
		return isString();
	case '{':
		json_string_type = "object";
		return parse_object();
	case '[':
		json_string_type = "array";
		return parse_array();
	case 't':
		json_string_type = "true";
		return match("true");
	case 'f':
		json_string_type = "false";
		return match("false");
	case 'n':
		json_string_type = "null";
		return match("null");
	default:
		json_string_type = "number";
		return isNumber();
	}
}

void test(char* test_str) {
	init_test(test_str);
	printf("===>>>>>test: %s\n", p);
	check(valid_json(), p);
}

int main() {
	p = number;
	p = "-3.1234e+123";
	if (isNumber() == 0) {
		printf("is number\n");
	}
	else {
		printf("not a number\n");
	}

	p = "\"abcdefg\nhijklmn\"";
	len = strlen(p);
	processed_chars = 0;
	if (isString() == 0) {
		printf("is String\n");
	}
	else {
		printf("not string\n");
	}
	init_test("{}");
	printf("////////////////////test: %s\n", p);
	check(valid_json(), p);

	init_test("[]");
	printf("////////////////////test: %s\n", p);
	check(valid_json(), p);

	init_test("{[]}");
	printf("////////////////////test: %s\n", p);
	check(valid_json(), p);

	test(
		"{ \"hello\" : \"world\", \"t\" : true , "\
		"\"f\" : false, \"n\": null, \"i\":123,\"pi\": 3.1416, \"a\":[1, 2, 3] } "
		);
	test("[-1]");
	test("[-1234567890123456789]");
	test("[1234567890123456789]");
	test("[\"\\\"\\\\/\\b\\f\\n\\r\\t\"]");
	return 0;
}