// This file is designed to pass scope analysis but fail type checking.

// Function for testing calls
int process(int id, float value, bool active) {
    if (active) {
        return id + 1;
    }
    return 0;
}

// Function with a specific return type for testing
bool is_ready() {
    return true;
}

int main() {

    // 1. ErroneousVarDecl: Assigning a boolean to a float variable.
    //float temperature = true;

    // 2. FnCallParamCount: Calling 'process' with too few arguments.
    //int result_a = process(101, 99.5);

    // 3. FnCallParamType: Calling 'process' with a string instead of a bool.
    int result_b = process(102, 50.0, "true");

    // 4. ErroneousReturnType: A function returning 'bool' cannot be assigned to an 'int' without a cast.
    int status = is_ready();

    // 5. ExpressionTypeMismatch (AttemptedAddOpOnNonNumeric): Adding a number and a boolean.
    int value = 10 + false;

    // 6. NonBooleanCondStmt: The condition of an 'if' statement must be a boolean.
    if (10 * 5) {
        // This block should not be analyzed if the condition fails type check.
    }

    // 7. AttemptedOpOnNonInt (for modulus '%'): Modulus requires integers.
    int remainder = 10.5 % 2;

    // 8. InvalidAssignment: Cannot assign a float to a bool variable.
    bool flag = 12.34;
    
    // 9. ErroneousBreak: 'break' statement outside of a loop.
    break;

    return 0;
}

void loop_function() {
    int i = 0;
    while (i < 5) {
        i = i + 1;
        // This is a valid use of continue, here for contrast.
        continue;
    }
    
    // 10. ErroneousContinue: 'continue' statement outside of a loop.
    continue;
}