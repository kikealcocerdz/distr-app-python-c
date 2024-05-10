program SERVIDOR_RPC {
    version SERVIDOR_RPCVER {
        int terminal_rpc(string user<>, int request, string timestamp<>, string filename<>) = 1;
    } = 1;
} = 99;
