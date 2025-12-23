int main() {
    char a_arr[16];
    char b_arr[16];
    char *a = a_arr;
    char *b = b_arr;
    a[0] = 'h';
    a[1] = 'a';
    a[2] = 'l';
    a[3] = 'l';
    a[4] = 'o';
    a[5] = ' ';
    a[6] = '\0';
    b[0] = 'w';
    b[1] = 'e';
    b[2] = 'l';
    b[3] = 't';
    b[4] = '\0';

    unsigned int n = 10;

    char *dest = a - 1;
    do {
        dest++;
    } while (*dest != '\0');

    do {
        char chr = *b++;
        *dest++ = chr;
        if (chr == '\0') {
            break;
        }
    } while (n-- != 0);
    *dest = '\0';

    int x = (unsigned int)a;
    x <<= 2;
    float f = 0;
    f *= 2;
    return 0;
}
