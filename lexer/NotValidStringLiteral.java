package com.abc.pkg.basics.lexer;

/**
 * Created by c7s on 2016/1/21.
 */
public class NotValidStringLiteral extends RuntimeException {
    public NotValidStringLiteral(String msg){
        super(msg);
    }
}
