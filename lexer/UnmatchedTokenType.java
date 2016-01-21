package com.abc.pkg.basics.lexer;

/**
 * Created by c7s on 2016/1/21.
 */
public class UnmatchedTokenType extends RuntimeException{
    public UnmatchedTokenType(String msg) {
        super(msg);
    }

    public UnmatchedTokenType() {
        super("current token doesn't match given one");
    }
}
