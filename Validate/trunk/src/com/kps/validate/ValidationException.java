package com.kps.validate;

public class ValidationException extends RuntimeException {

    private static final long serialVersionUID = -7711130827755292323L;

    public ValidationException(final String message) {
        super(message);
    }
}
