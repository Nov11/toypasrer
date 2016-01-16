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

char trimBlankChar(char* p) {
	char c = nextChar();
	while (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
		c = nextChar();
	}
	if (p != NULL) {
		*p = c;
	}
	return c;
}

char* currentPosition() {
	return p;
}

int no_more_chars() {
	return len - processed_chars ;
}

void error_msg() {
	printf("error occured position:%p char:%c processing char:%c\n", currentPosition(), *currentPosition(), *(currentPosition() - 1));
}

void matchDIGIT(char* c) {
	*c = nextChar();
	while (isdigit(*c)) {
		*c = nextChar();
	}
}

int isNumber() {
	char c = nextChar();

	/*starting minus sign*/
	if (c == '-') {
		c = nextChar();
	}
	/*0 or digits before possibly '.' */
	if (isdigit(c)) {
		if (c == '0') {
			c = nextChar();
		}
		else {
			matchDIGIT(&c);
		}
	}
	else {
		error_msg();
		return -1;
	}
	/*fractions*/
	if (c == '.') {
		c = nextChar();
		if (isdigit(c)) {
			matchDIGIT(&c);
		}
		else {
			error_msg();
			return -1;
		}
	}
	/*exponent*/
	if (c == 'e' || c == 'E') {
		c = nextChar();
		/* '-' or '+' */
		if (c == '-' || c == '+') {
			c = nextChar();
		}
		/*digits*/
		if (isdigit(c)) {
			matchDIGIT(&c);
		}
		else {
			error_msg();
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
		trimBlankChar(&c);
		if (c == ']') {
			return 0;
		}
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
		if (c == ',' && ret != 0) {
			//no valid value after ','
			return -1;
		}
		else if (c != ',' && ret == 0) {
			//last valid value is found
			//nothing here
		}
		else {
			//if c == ',' && ret == 0, while will continue
			//it's c != ',' && ret != 0 here
			//shouldn't arrive at this state
			printf("ERROR\n");
			return -1;
		}
		trimBlankChar(&c);
		if (c == ']') {
			return 0;
		}
		else {
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
			return match("fasle");
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
			return -1;
		}
		trimBlankChar(&c);
		//match colon
		if (c == ':') {
			c = nextChar();
		}
		else {
			error_msg();
			printf("parsing piar. no colon follows a string\n");
			return -1;
		}
		//match a value 
		trimBlankChar(&c);
		ret = isValue();
		if (ret != 0) {
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
		trimBlankChar(&c);
		c = nextChar();
		if (c == ',') {
			ret = parse_pair();
		}
		else {
			break;
		}
	}
	trimBlankChar(&c);
	c = nextChar();
	if (c == '}') {
		put_back();
		return 0;
	}
	else {
		error_msg();
		printf("pasre members. member doesn't end with '}' \n");
		return -1;
	}
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
			return -1;
		}
		if (c == '}') {
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
		return match("fasle");
	case 'n':
		json_string_type = "null";
		return match("null");
	default:
		json_string_type = "number";
		return isNumber();
	}
}

int main() {
	p = number;
	p = "qt";
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
	return 0;
}