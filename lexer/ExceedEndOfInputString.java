package com.abc.pkg.basics.lexer;

/**
 * Created by c7s on 2016/1/21.
 */
public class ExceedEndOfInputString extends RuntimeException {
    public ExceedEndOfInputString(String msg) {
        super(msg);
    }
}
