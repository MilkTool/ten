
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <ten.h>
#include <tml.h>
//#include <line-arg.h>


#define MAIN_PROMPT             "$ "
#define SUB_PROMPT              "? "
#define RESULT_PREFIX           ": "
#define DEFAULT_LIBRARY_PATH    "/usr/lib/ten/:/usr/lib64/ten/:/usr/local/lib/ten/:/usr/local/lib64/ten/"


typedef enum {
    LS_MPROMPT,
    LS_SPROMPT,
    LS_NEXT,
    LS_EOL
} LsState;

typedef struct {
    ten_Source  base;
    LsState     state;
    char*       line;
    size_t      next;
} LineSource;

static void
lsFinl( ten_Source* src ) {
    LineSource* ls = (LineSource*)src;
    if( ls->line ) {
        if( ls->line[ls->next] != '\0' )
            ungetc( ls->line[ls->next], stdin );
        
        free( ls->line );
        ls->line = NULL;
    }
}

static int
lsNext( ten_Source* src ) {
    LineSource* ls = (LineSource*)src;
    switch( ls->state ) {
        case LS_MPROMPT:
            ls->line = readline( MAIN_PROMPT );
            if( !ls->line )
                return ten_EOF;
            add_history( ls->line );
            ls->state = LS_NEXT;
            ls->next  = 0;
            return lsNext( src );
        break;
        case LS_SPROMPT:
            free( ls->line );
            ls->line = readline( SUB_PROMPT );
            if( !ls->line )
                return ten_EOF;
            add_history( ls->line );
            ls->state = LS_NEXT;
            ls->next  = 0;
            return lsNext( src );
        break;
        case LS_NEXT:
            if( ls->line[ls->next] == '\0' ) {
                ls->state = LS_EOL;
                return '\n';
            }
            else {
                return ls->line[ls->next++];
            }
        break;
        case LS_EOL:
            ls->state = LS_SPROMPT;
            return ten_PAD;
        break;
    }
    
    return ten_EOF;
}

static bool
prompt( ten_State* ten ) {
    LineSource ls = {
        .base = {
            .name = "<REPL>",
            .next = lsNext,
            .finl = lsFinl
        }
    };
    
    ten_Tup r = ten_executeExpr( ten, (ten_Source*)&ls, ten_SCOPE_GLOBAL );
    fputs( RESULT_PREFIX, stdout );
    fputs( ten_string( ten, &r ), stdout );
    fputc( '\n', stdout );
    ten_pop( ten );
    return true;
}

static void
showError( ten_State* ten ) {
    ten_ErrNum  err = ten_getErrNum( ten, NULL );
    char const* msg = ten_getErrStr( ten, NULL );
    if( err == ten_ERR_NONE )
        return;
    fprintf( stderr, "Error: %s\n", msg );
    
    ten_Trace* tIt = ten_getTrace( ten, NULL );
    while( tIt ) {
        char const* unit = "???";
        if( tIt->unit )
            unit = tIt->unit;
        char const* file = "???";
        if( tIt->file )
            file = tIt->file;
        
        fprintf(
            stderr,
            "  unit: %-10s line: %-4u file: %-20s\n",
            unit, tIt->line, file
        );
        tIt = tIt->next;
    }
    fflush( stderr );
}

static void
showVersion( void ) {
    printf( "Ten 0.5.0\n" );
    printf( "License MIT\n" );
    printf( "Copyright (C) 2019 Ray Stubbs\n" );
}

static void
showHelp( void ) {
    printf( "ten\n" );
    printf( "  : launch a REPL\n" );
    printf( "ten script\n" );
    printf( "  : run a script file\n" );
    printf( "ten [-v | --version]\n" );
    printf( "  : show version and copyright info\n" );
    printf( "ten [-h | --help]\n" );
    printf( "  : show this help message\n" );
}

static void
runRepl( ten_State* ten ) {
    jmp_buf  jmp;
    jmp_buf* old = ten_swapErrJmp( ten, &jmp );
    if( setjmp( jmp ) ) {
        showError( ten );
        if( ten_getErrNum( ten, NULL ) == ten_ERR_FATAL ) {
            ten_swapErrJmp( ten, old );
            ten_propError( ten, NULL );
        }
        
        // If non-fatal error then continue repl.
    }
    
    while( prompt( ten ) )
        ;
}

static void
runScript( ten_State* ten, char const* script ) {
    jmp_buf  jmp;
    jmp_buf* old = ten_swapErrJmp( ten, &jmp );
    if( setjmp( jmp ) ) {
        showError( ten );
        ten_swapErrJmp( ten, old );
        ten_propError( ten, NULL );
    }
    
    ten_Source* src = ten_pathSource( ten, script );
    ten_executeScript( ten, src, ten_SCOPE_LOCAL );
}


int
main( int argc, char** argv ) {
    char* script = NULL;
    if( argc > 2 ) {
        fprintf( stderr, "Invalid usage pattern, try 'ten --help'\n" );
        return 1;
    }
    
    if( argc == 2 ) {
        if( !strcmp( argv[1], "--version" ) || !strcmp( argv[1], "-v" ) ) {
            showVersion();
            return 0;
        }
        if( !strcmp( argv[1], "--help" ) || !strcmp( argv[1], "-h" ) ) {
            showHelp();
            return 0;
        }
        if( argv[1][0] == '-' ) {
            fprintf( stderr, "Invalid usage pattern, try 'ten --help'\n" );
            return 1;
        }
        script = argv[1];
    }
    
    ten_State* volatile ten = NULL;
    jmp_buf             jmp;
    if( setjmp( jmp ) ) {
        ten_free( ten );
        return 1;
    }
    ten = ten_make( NULL, &jmp );
    
    char const* plib = getenv( "TEN_LIBRARY_PATH" );
    if( !plib || plib[0] == '\0' )
        plib = DEFAULT_LIBRARY_PATH;
    
    if( script ) {
        char* scpy = strdup( script );
        char* ppro = dirname( scpy );
        tml_install( ten, ppro, plib, "eng" );
        free( scpy );
    }
    else {
        tml_install( ten, ".", plib, "eng" );
    }
    
    if( script )
        runScript( ten, script );
    else
        runRepl( ten );
    
    ten_free( ten );
    return 0;
}
