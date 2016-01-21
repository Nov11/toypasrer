package com.abc.pkg.basics.lexer;

/**
 * Created by c7s on 2016/1/21.
 */
public enum TokenType {
    NONE(-1, "no more token"),
    LBRACKET(0, "["),
    RBRACKET(1, "]"),
    LBRACE(2, "{"),
    RBRACE(3, "}"),
    COMMA(4, ","),
    BLANK(5, "space tab newline carriage return"),
    DOUBLEQUOTE(6, "\""),
    NUMBER(7, "number"),
    STRINGLITERAL(8, "string literal"),
    COLON(9, ":"),
    ;


    int code;
    String meaning;

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getMeaning() {
        return meaning;
    }

    public void setMeaning(String meaning) {
        this.meaning = meaning;
    }

    TokenType(int c, String m) {
        code = c;
        meaning = m;
    }
}
