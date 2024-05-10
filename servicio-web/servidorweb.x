program SERVIDOR_RPC {
    version SERVIDOR_RPCVER {
        int terminal_rpc(char user, int request, char timestamp, char filename) = 1;
    } = 1;
} = 99;
