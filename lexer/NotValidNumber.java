package com.abc.pkg.basics.lexer;


/**
 * Created by c7s on 2016/1/21.
 */
public class NotValidNumber extends RuntimeException{
    public NotValidNumber(String msg) {
        super(msg);
    }
}
