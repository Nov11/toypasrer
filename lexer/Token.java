package com.abc.pkg.basics.lexer;

/**
 * Created by c7s on 2016/1/21.
 */
public class Token {
    private TokenType type;
    private String content;

    public Token(TokenType t, String c) {
        type = t;
        content = c;
    }

    public Token(TokenType t){
        type = t;
        content = t.getMeaning();
    }

    public TokenType getType() {
        return type;
    }

    public void setType(TokenType type) {
        this.type = type;
    }

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    @Override
    public String toString() {
        return "Token{" +
                "type=" + type +
                ", content='" + content + '\'' +
                '}';
    }
}
