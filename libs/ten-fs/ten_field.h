#ifndef ten_field_h
#define ten_field_h

#define ten_field_fun( REC, NAME, SYM, DAT, PARAMS... )                 \
do {                                                                    \
    ten_Tup vars = ten_pushA( ten, "UU" );                              \
    ten_Var funVar = { .tup = &vars, .loc = 0 };                        \
    ten_Var clsVar = { .tup = &vars, .loc = 1 };                        \
    ten_FunParams fp = {                                                \
        .name   = #NAME,                                                \
        .params = (char const*[]){ PARAMS },                            \
        .cb     = tf_ ## NAME                                           \
    };                                                                  \
    ten_newFun( ten, &fp, &funVar );                                    \
    ten_newCls( ten, &funVar, (DAT), &clsVar );                         \
                                                                        \
    ten_Var* sym = (SYM);                                               \
    if( !sym )                                                          \
        sym = ten_sym( ten, #NAME );                                    \
                                                                        \
    ten_recDef( ten, (REC), sym, &clsVar );                             \
                                                                        \
    ten_pop( ten );                                                     \
} while( 0 )

#define ten_field_log( REC, NAME, SYM, VAL )                            \
do {                                                                    \
    ten_Var* sym = (SYM);                                               \
    if( !sym )                                                          \
        sym = ten_sym( ten, #NAME );                                    \
                                                                        \
    ten_recDef( ten, (REC), sym, ten_log( VAL ) );                      \
} while( 0 )

#define ten_field_int( REC, NAME, SYM, VAL )                            \
do {                                                                    \
    ten_Var* sym = (SYM);                                               \
    if( !sym )                                                          \
        sym = ten_sym( ten, #NAME );                                    \
                                                                        \
    ten_recDef( ten, (REC), sym, ten_int( VAL ) );                      \
} while( 0 )

#define ten_field_dec( REC, NAME, SYM, VAL )                            \
do {                                                                    \
    ten_Var* sym = (SYM);                                               \
    if( !sym )                                                          \
        sym = ten_sym( ten, #NAME );                                    \
                                                                        \
    ten_recDef( ten, (REC), sym, ten_dec( VAL ) );                      \
} while( 0 )

#define ten_field_sym( REC, NAME, SYM, VAL )                            \
do {                                                                    \
    ten_Var* sym = (SYM);                                               \
    if( !sym )                                                          \
        sym = ten_sym( ten, #NAME );                                    \
                                                                        \
    ten_recDef( ten, (REC), sym, ten_sym( VAL ) );                      \
} while( 0 )

#define ten_field_str( REC, NAME, SYM, VAL )                            \
do {                                                                    \
    ten_Var* sym = (SYM);                                               \
    if( !sym )                                                          \
        sym = ten_sym( ten, #NAME );                                    \
                                                                        \
    ten_recDef( ten, (REC), sym, ten_str( VAL ) );                      \
} while( 0 )


#endif
