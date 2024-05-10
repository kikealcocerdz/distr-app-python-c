program SERVIDOR_RPC {
    version SERVIDOR_RPCVER {
        int terminal_rpc(int request, string timestamp<>, string user<>) = 1;
    } = 1;
} = 99;
