#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "calculator.h"

// Forward declarations for recursive‑descent parser
static const char *gExpr;
static double parse_expr(void);
static double parse_term(void);
static double parse_factor(void);
static void skip_spaces(void);

static void skip_spaces(void) {
    while (isspace((unsigned char)*gExpr)) gExpr++;
}

static double parse_factor(void) {
    skip_spaces();
    if (*gExpr == '(') {
        gExpr++; // '('
        double v = parse_expr();
        skip_spaces();
        if (*gExpr == ')') gExpr++; // ')'
        return v;
    }
    // number (int or float)
    char *endptr;
    double val = strtod(gExpr, &endptr);
    if (endptr == gExpr) {
        // no conversion – treat as zero
        return 0.0;
    }
    gExpr = endptr;
    return val;
}

static double parse_term(void) {
    double left = parse_factor();
    while (1) {
        skip_spaces();
        if (*gExpr == '*') {
            gExpr++;
            left *= parse_factor();
        } else if (*gExpr == '/') {
            gExpr++;
            double denom = parse_factor();
            if (denom != 0.0) left /= denom; else left = 0.0; // protect div by zero
        } else {
            break;
        }
    }
    return left;
}

static double parse_expr(void) {
    double left = parse_term();
    while (1) {
        skip_spaces();
        if (*gExpr == '+') {
            gExpr++;
            left += parse_term();
        } else if (*gExpr == '-') {
            gExpr++;
            left -= parse_term();
        } else {
            break;
        }
    }
    return left;
}

double calc_evaluate(const char *expr, bool *ok) {
    if (!expr) { if (ok) *ok = false; return 0.0; }
    gExpr = expr;
    double result = parse_expr();
    skip_spaces();
    if (ok) *ok = (*gExpr == '\0'); // true if we consumed entire string
    return result;
}
