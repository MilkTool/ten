#ifndef ten_export_h
#define ten_export_h

#ifndef ten_export_DEST
    #define ten_export_DEST export
#endif

#define ten_export_fun( NAME, DAT, PARAMS... )                          \
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
    ten_recDef( ten, ten_export_DEST, ten_sym( ten, #NAME ), &clsVar ); \
                                                                        \
    ten_pop( ten );                                                     \
} while( 0 )

#define ten_export_log( NAME, VAL ) \
    ten_recDef( ten, ten_export_DEST, ten_sym( ten, #NAME ), ten_log( VAL ) )

#define ten_export_int( NAME, VAL ) \
    ten_recDef( ten, ten_export_DEST, ten_sym( ten, #NAME ), ten_int( VAL ) )

#define ten_export_dec( NAME, VAL ) \
    ten_recDef( ten, ten_export_DEST, ten_sym( ten, #NAME ), ten_dec( VAL ) )

#define ten_export_sym( NAME, VAL ) \
    ten_recDef( ten, ten_export_DEST, ten_sym( ten, #NAME ), ten_sym( VAL ) )

#define ten_export_str( NAME, VAL ) \
    ten_recDef( ten, ten_export_DEST, ten_sym( ten, #NAME ), ten_str( VAL ) )


#endif
