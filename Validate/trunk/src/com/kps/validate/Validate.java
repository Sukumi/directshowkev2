package com.kps.validate;

public final class Validate {

    private Validate() {
    // Prevents construction
    }

    public static void between(final double value, final String variableName, final double min, final double max) {
        if ((value < min) || (value > max)) {
            final String message = "%s (%s) must be between %s and %s";
            throw new ValidationException(String.format(message, variableName, value, min, max));
        }
    }

    public static void positive(final double value, final String variableName) {
        if (value < 0.0) {
            final String message = "%s (%s) must be positive";
            throw new ValidationException(String.format(message, variableName, value));
        }
    }

    public static void notNull(final Object value, final String variableName) {
        if (value == null) {
            final String message = "%s must not be null";
            throw new ValidationException(String.format(message, variableName));
        }
    }

    public static void notEmpty(final String value, final String variableName) {
        if (value.trim().length() == 0) {
            final String message = "%s (\"%s\") must not be empty";
            throw new ValidationException(String.format(message, value, variableName));
        }
    }
}
