#ifndef CALCULATOR_H
#define CALCULATOR_H

// Simple arithmetic evaluator – supports +, -, *, / and parentheses.
// Returns the result as a double. On error, sets *ok = false.

double calc_evaluate(const char *expr, bool *ok);

#endif // CALCULATOR_H
