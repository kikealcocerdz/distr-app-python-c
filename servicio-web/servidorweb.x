struct convertir_request {
    long timestamp;
    int operation_code;
    string data<>;
};

program STRING_PROG {
    version STRING_VER {
        int vocales(request) = 1;
        string first(request) = 2;
        string convertir(convertir_request) = 3; 
    } = 1;
} = 0x20000001;
