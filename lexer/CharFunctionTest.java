package com.abc.pkg.basics.lexer;

/**
 * Created by c7s on 2016/1/21.
 */
public class CharFunctionTest {
    public static void main(String[] args) {
        char test[] = {'1', '"'};
        for (char c : test) {
            System.out.println("char:" + c + " isLetter:" + Character.isLetter(c));
        }
        char c = (char) 32 | '\t';
        switch (c) {
            case ' ' | '\t':
                System.out.println("space");
        }
    }
}
