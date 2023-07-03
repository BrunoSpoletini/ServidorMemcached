-module(client).

-export([start/0, start/1, put/3, del/2, get/2, stats/1, close/1, test/0]).

-define(PUT, 11).
-define(DEL, 12).
-define(GET, 13).
-define(STATS, 21).
-define(OK, 101).
-define(EINVALID, 111).
-define(ENOTFOUND, 112).
-define(EBINARY, 113).
-define(EBIG, 114).
-define(EUNK, 115).
-define(EOOM, 116).

test() -> {_,B} = start(),
put(B,"clave","testdon
    ereaawgfawgawgawgawawgasehgosjslekghjwslieghlsejnmhlskjmhlkñdrjmhldkjmholpdreehljsdlrhkldrhmldmñrtjlgawgawgawgawgdn"),
get(B,"clave").

decode(Code) ->
    case Code of
        ?OK ->
            ok;
        ?EINVALID ->
            einvalid;
        ?ENOTFOUND ->
            enotfound;
        ?EBINARY ->
            ebinary;
        ?EBIG ->
            ebig;
        ?EUNK ->
            eunk;
        ?EOOM ->
            eoom
    end.

start_(Host) ->
    case gen_tcp:connect(Host, 889, [binary, {active, false}, {packet, raw}]) of
        {ok, Sock} ->
            PID = spawn(fun() -> manager(Sock) end),
            {ok, {conn, PID}};
        {error, Reason} ->
            {error, Reason}
    end.

start() ->
    start_("localhost").

start(Host) ->
    start_(Host).

manager(Sock) ->
    receive
        {put, K, V, PID} ->
            R = put_(Sock, K, V),
            PID ! R;
        {del, K, PID} ->
            R = del_(Sock, K),
            PID ! R;
        {get, K, PID} ->
            R = get_(Sock, K),
            PID ! R;
        {stats, PID} ->
            R = stats_(Sock),
            PID ! R;
        {close, PID} ->
            R = close_(Sock),
            PID ! R
    end,
    manager(Sock).

% Handlers para cada tipo de request
put_(Sock, K, V) ->
    gen_tcp:send(Sock, encode_cmd(?PUT, K, V)),
    parse_code(Sock).

del_(Sock, K) ->
    gen_tcp:send(Sock, encode_cmd(?DEL, K)),
    parse_code(Sock).

get_(Sock, K) ->
    gen_tcp:send(Sock, encode_cmd(?GET, K)),
    
    parse(Sock, fun(Bin) ->list_to_atom(binary_to_list(Bin))end).

stats_(Sock) ->
    gen_tcp:send(Sock, <<?STATS>>),
    parse(Sock, fun(Bin) ->list_to_atom(binary_to_list(Bin))end).
    %parse(Sock, fun binary_to_list/1).

close_(Sock) ->
    case gen_tcp:shutdown(Sock, read_write) of
        ok ->
            io:fwrite("The connection on the socket ~p was closed~n", [Sock]),
            closed;
        {error, Reason} ->
            {error, Reason}
    end.



%5-255*255 -> 4
%255*255 - 255*255*255 -> 5
%255^3 - 255^4 -> 6
headerlen(Size, Top) ->
    if (Size > Top) -> 1 + headerlen(Size,Top*255);
        true -> 4
    end.

header(Size) -> headerlen (Size,255*255).


% Funciones para poder encodear requests
encode(Data) ->
    Bin = term_to_binary(Data),
    Size = byte_size(Bin),
    NewSize = Size - header(Size),
    NewBin = binary:list_to_bin( binary:bin_to_list( Bin , {Size-NewSize,NewSize}  ) ),
    BSize = <<NewSize:32>>,
    <<BSize/binary, NewBin/binary>>.

encode_cmd(Cmd, K) ->
    BKey = encode(K),
    <<Cmd, BKey/binary>>.

encode_cmd(Cmd, K, V) ->
    BKey = encode(K),
    BValue = encode(V),
    <<Cmd, BKey/binary, BValue/binary>>.

% Parsea el codigo de la respuesta
parse_code(Sock) ->
    case gen_tcp:recv(Sock, 1) of
        {ok, <<Code>>} ->
            {response, decode(Code)};
        {error, Reason} ->
            {error, Reason}
    end.

% Parsea la data de la respuesta
parse_data(Sock) ->
    case gen_tcp:recv(Sock, 4) of
        {ok, Size} ->
            gen_tcp:recv(Sock, binary:decode_unsigned(Size));
        {error, Reason} ->
            {error, Reason}
    end.

% Handler para respuestas con data
parse(Sock, EncodeData) ->
    case parse_code(Sock) of
        {response, ok} ->
            case parse_data(Sock) of
                {ok, Data} ->
                    {response, ok, EncodeData(Data)};
                {error, Reason} ->
                    {error, Reason}
            end;
        {response, Code} ->
            {response, Code};
        {error, Reason} ->
            {error, Reason}
    end.

receive_response() ->
    receive
        closed ->
            ok;
        {response, ok} ->
            ok;
        {response, enotfound} ->
            enotfound;
        {response, ok, Data} ->
            {ok, Data};
        {response, Code} ->
            {error, Code};
        {error, Reason} ->
            {error, Reason}
    end.

% Funciones de uso
put(Conn, K, V) ->
    case Conn of
        {conn, Manager} ->
            Manager ! {put, K, V, self()},
            receive_response();
        _ ->
            {error, noconn}
    end.

del(Conn, K) ->
    case Conn of
        {conn, Manager} ->
            Manager ! {del, K, self()},
            receive_response();
        _ ->
            {error, noconn}
    end.

get(Conn, K) ->
    case Conn of
        {conn, Manager} ->
            Manager ! {get, K, self()},
            receive_response();
        _ ->
            {error, noconn}
    end.

stats(Conn) ->
    case Conn of
        {conn, Manager} ->
            Manager ! {stats, self()},
            receive_response();
        _ ->
            {error, noconn}
    end.

close(Conn) ->
    case Conn of
        {conn, Manager} ->
            Manager ! {close, self()},
            receive_response();
        _ ->
            {error, noconn}
    end.


