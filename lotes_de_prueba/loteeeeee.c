int var;
int swap (int & a, int b, int f) {
    int c;
    if(3 < f) {
        a=b;
        c=8;
    }
    else {
     c=5;
    }
    return(c);
}

void main ( ) {
    int c, d, f;
    cin >> d >> var;
    f = swap(c, d, var);
    cout << "0123" << c + 5 << "89";
}