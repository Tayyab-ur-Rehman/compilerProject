int global = 100;

int factorial(int n) {
    if (n < 2) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
int main ()
{
    int result = factorial(5);
    return 0;
}
