package com.abc.pkg.basics.lexer;

/**
 * Created by c7s on 2016/1/21.
 */
public interface Lexer {
    void resetLexer(String inputParam);

    Token nextToken();
}
