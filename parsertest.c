int global_counter = 0;
int result_accumulator = 0;

int factorial(int n) {
    if (n < 2) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

// 3. BOOLEAN LOGIC & TYPE CHECKING
// Tests: bool type, logical operators (&&, ||, !), comparison (>, ==)
bool is_even(int num) {
    if ((num % 2) == 0) {
        return true;
    } else {
        return false;
    }
}

// 4. LOOPS, BREAK/CONTINUE, & POSTFIX OPERATORS
// Tests: for-loops, while-loops, scope nesting, 'break', 'continue', 'i++'
int loop_logic_test(int limit) {
    int sum = 0;
    
    // Test: For loop with variable declaration and Postfix Increment (i++)
    for (int i = 0; i < limit; i++) {
        
        // Test: Continue statement
        if (i == 2) {
            continue;
        }

        sum = sum + i;

        // Test: Break statement
        if (sum > 20) {
            break;
        }
    }

    return sum;
}

// 5. MATH & PRECEDENCE
// Tests: Binary operations precedence (* before +), floating point logic
int math_test() {
    int a = 10;
    int b = 5;
    int c = 2;
    
    // Precedence check: should be 10 + (5 * 2) = 20, not (10+5)*2 = 30
    int res = a + b * c; 
    
    return res;
}
int math_test(int a)
{
    return a * 2;
}
int math_test(float b)
{
    return  b*b;
}
// 6. MAIN ENTRY POINT
int main() {
    // A. Test Assignment & Globals
    global_counter = 5;
    
    // B. Test Function Call & Recursion
    // factorial(5) = 120
    int fact_res = factorial(global_counter); 

    // C. Test Boolean Logic
    // is_even(120) = true
    bool check = is_even(fact_res); 

    // D. Test Branching
    if (check) {
        result_accumulator = 1;
    } else {
        result_accumulator = 0;
    }

    // E. Test Loops & Postfix (i++)
    // limit = 10. Loop sums: 0+1+(skip 2)+3+4+5+6 = 19. Next is 7, sum=26, break.
    int loop_res = loop_logic_test(10);
    int a = math_test(10.0);
    // F. Test Math
    // math_res = 20
    int math_res = math_test();

    // Final Calculation to verify everything
    // 120 + 1 + 26 + 20 = 167
    int final_result = fact_res + result_accumulator + loop_res + math_res;

    return final_result;
}