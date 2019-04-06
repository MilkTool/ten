
#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <ten.h>
//#include <line-arg.h>


#define MAIN_PROMPT     "$ "
#define SUB_PROMPT      "? "
#define RESULT_PREFIX   ": "

typedef struct {
    ten_Source  base;
    char*       line;
    size_t      next;
    unsigned    npad;
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
    if( ls->line != NULL && ls->line[ls->next] != '\0' )
        return ls->line[ls->next++];
    
    if( ls->line == NULL )
        return -1;
    
    
    if( ls->line[ls->next] == '\0' ) {
        if( ls->npad > 0 ) {
            ls->npad--;
            return '\n';
        }
        
        free( ls->line );
        ls->line = readline( SUB_PROMPT );
        ls->next = 0;
        ls->npad = 3;
        
        if( ls->line == NULL ) {
            return -1;
        }
        else {
            add_history( ls->line );
            return ls->line[ls->next++];
        }
    }
    return -1;
}

static bool
prompt( ten_State* ten ) {
    char* line = readline( MAIN_PROMPT );
    if( line == NULL )
        return false;
    
    add_history( line );
    
    LineSource ls = {
        .base = {
            .name = "<repl>",
            .next = lsNext,
            .finl = lsFinl
        },
        .line = line,
        .npad = 3,
        .next = 0
    };
    
    ten_Tup r = ten_executeExpr( ten, (ten_Source*)&ls, ten_SCOPE_GLOBAL );
    fputs( RESULT_PREFIX, stdout );
    fputs( ten_string( ten, &r ), stdout );
    fputc( '\n', stdout );
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
        char const* fiber = "???";
        if( tIt->fiber )
            fiber = tIt->fiber;
        char const* file = "???";
        if( tIt->file )
            file = tIt->file;
        
        fprintf( stderr, "  %s#%u (%s)\n", file, tIt->line, fiber );
        tIt = tIt->next;
    }
}

static void
repl( ten_State* ten ) {
    jmp_buf  jmp;
    jmp_buf* old = ten_swapErrJmp( ten, &jmp );
    int sig = setjmp( jmp );
    if( sig ) {
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



int
main( int argc, char const** argv ) {
    ten_Config cfg = { .debug = true };
    ten_State  ten;
    jmp_buf    jmp;
    ten_init( &ten, &cfg, &jmp );
    
    int sig = setjmp( jmp );
    if( sig )
        exit( 1 );
    
    repl( &ten );
    
    ten_finl( &ten );
    return 0;
}
