#include <unistd.h>
#include <limits.h>
#include <glob.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ten.h>

#include "ten_export.h"

typedef enum {
    FS_SYM_owner,
    FS_SYM_group,
    FS_SYM_world,
    FS_SYM_self,
    FS_SYM_read,
    FS_SYM_write,
    FS_SYM_exec,
    FS_SYM_bdev,
    FS_SYM_cdev,
    FS_SYM_dir,
    FS_SYM_fifo,
    FS_SYM_link,
    FS_SYM_reg,
    FS_SYM_sock,
    FS_LAST
} FsMember;

typedef struct {
    ten_DatInfo* globInfo;
} FsState;

typedef struct {
    glob_t glob;
    size_t next;
} Glob;

static ten_Tup
tf_globN( ten_PARAMS ) {
    Glob* g = dat;
    
    ten_Tup rets = ten_pushA( ten, "U" );
    ten_Var ret  = { .tup = &rets, .loc = 0 };
    if( g->next < g->glob.gl_pathc ) {
        char const* path = g->glob.gl_pathv[g->next++];
        size_t      len  = strlen( path );
        ten_newStr( ten, path, len, &ret );
    }
    
    return rets;   
}

static void
globD( ten_State* ten, void* dat ) {
    Glob* g = dat;
    globfree( &g->glob );
}

static ten_Tup
tf_glob( ten_PARAMS ) {
    FsState* fs = dat;
    
    ten_Var patArg = { .tup = args, .loc = 0 };
    ten_expect( ten, "path", ten_strType( ten ), &patArg );
    
    ten_Tup rets = ten_pushA( ten, "U" );
    ten_Var ret = { .tup = &rets, .loc = 0 };
    
    glob_t buf = { 0 };
    int err = glob( ten_getStrBuf( ten, &patArg ), 0, NULL, &buf );
    if( err && err != GLOB_NOMATCH )
        return rets;
    
    
    ten_Tup vars = ten_pushA( ten, "UU" );
    ten_Var funVar = { .tup = &vars, .loc = 0 };
    ten_Var datVar = { .tup = &vars, .loc = 1 };
    
    Glob* g = ten_newDat( ten, fs->globInfo, &datVar );
    g->glob = buf;
    g->next = 0;
    
    ten_FunParams fp = {
        .name   = "globN",
        .params = (char const*[]){ NULL },
        .cb     = tf_globN
    };
    ten_newFun( ten, &fp, &funVar );
    
    ten_newCls( ten, &funVar, &datVar, &ret );
    
    return rets;
}

static ten_Tup
tf_can( ten_PARAMS ) {
    ten_Var whomArg = { .tup = args, .loc = 0 };
    ten_Var whatArg = { .tup = args, .loc = 1 };
    ten_Var pathArg = { .tup = args, .loc = 2 };
    
    ten_expect( ten, "whom", ten_symType( ten ), &whomArg );
    ten_expect( ten, "what", ten_symType( ten ), &whatArg );
    ten_expect( ten, "path", ten_strType( ten ), &pathArg );
    
    ten_Var ownerMem = { .tup = mems, .loc = FS_SYM_owner };
    ten_Var groupMem = { .tup = mems, .loc = FS_SYM_group };
    ten_Var worldMem = { .tup = mems, .loc = FS_SYM_world };
    ten_Var selfMem  = { .tup = mems, .loc = FS_SYM_self };
    ten_Var readMem  = { .tup = mems, .loc = FS_SYM_read };
    ten_Var writeMem = { .tup = mems, .loc = FS_SYM_write };
    ten_Var execMem  = { .tup = mems, .loc = FS_SYM_exec };
    
    char const* path = ten_getStrBuf( ten, &pathArg );
    struct stat buf;
    if( stat( path, &buf ) < 0 )
        ten_panic( ten, ten_str( ten, strerror( errno ) ) );
        
    bool can = false;
    if( ten_equal( ten, &whomArg, &ownerMem ) ) {
        if( ten_equal( ten, &whatArg, &readMem ) ) {
            can = !!(buf.st_mode & S_IRUSR);
        }
        else
        if( ten_equal( ten, &whatArg, &writeMem ) ) {
            can = !!(buf.st_mode & S_IWUSR);
        }
        else
        if( ten_equal( ten, &whatArg, &execMem ) ) {
            can = !!(buf.st_mode & S_IXUSR);
        }
        else {
            goto badWhat;
        }
    }
    else
    if( ten_equal( ten, &whomArg, &groupMem ) ) {
        if( ten_equal( ten, &whatArg, &readMem ) ) {
            can = !!(buf.st_mode & S_IRGRP);
        }
        else
        if( ten_equal( ten, &whatArg, &writeMem ) ) {
            can = !!(buf.st_mode & S_IWGRP);
        }
        else
        if( ten_equal( ten, &whatArg, &execMem ) ) {
            can = !!(buf.st_mode & S_IXGRP);
        }
        else {
            goto badWhat;
        }
    }
    else
    if( ten_equal( ten, &whomArg, &worldMem ) ) {
        if( ten_equal( ten, &whatArg, &readMem ) ) {
            can = !!(buf.st_mode & S_IROTH);
        }
        else
        if( ten_equal( ten, &whatArg, &writeMem ) ) {
            can = !!(buf.st_mode & S_IWOTH);
        }
        else
        if( ten_equal( ten, &whatArg, &execMem ) ) {
            can = !!(buf.st_mode & S_IXOTH);
        }
        else {
            goto badWhat;
        }
    }
    else
    if( ten_equal( ten, &whomArg, &selfMem ) ) {
        uid_t uid = geteuid();
        gid_t gid = getegid();
        
        int omsk = 0;
        int gmsk = 0;
        int umsk = 0;
        if( ten_equal( ten, &whatArg, &readMem ) ) {
            omsk = S_IROTH;
            gmsk = S_IRGRP;
            umsk = S_IRUSR;
        }
        else
        if( ten_equal( ten, &whatArg, &writeMem ) ) {
            omsk = S_IWOTH;
            gmsk = S_IWGRP;
            umsk = S_IWUSR;
        }
        else
        if( ten_equal( ten, &whatArg, &execMem ) ) {
            omsk = S_IXOTH;
            gmsk = S_IXGRP;
            umsk = S_IXUSR;
        }
        else {
            goto badWhat;
        }
        
        if( buf.st_mode & omsk )
            can = true;
        else
        if( buf.st_mode & gmsk && buf.st_gid == gid )
            can = true;
        else
        if( buf.st_mode & umsk && buf.st_uid == uid )
            can = true;
    }
    
    return ten_pushA( ten, "L", can );
    
badWhat:
    ten_panic( ten, ten_str( ten, "Invalid 'what' symbol value" ) );
badWhom:
    ten_panic( ten, ten_str( ten, "Invalid 'whom' symbol value" ) );
    
    return ten_pushA( ten, "" );
}

ten_Tup
tf_type( ten_PARAMS ) {
    ten_Var pathArg = { .tup = args, .loc = 0 };
    ten_expect( ten, "path", ten_strType( ten ), &pathArg );
    
    ten_Var bdevMem = { .tup = mems, .loc = FS_SYM_bdev };
    ten_Var cdevMem = { .tup = mems, .loc = FS_SYM_cdev };
    ten_Var dirMem  = { .tup = mems, .loc = FS_SYM_dir };
    ten_Var fifoMem = { .tup = mems, .loc = FS_SYM_fifo };
    ten_Var linkMem = { .tup = mems, .loc = FS_SYM_link };
    ten_Var regMem  = { .tup = mems, .loc = FS_SYM_reg };
    ten_Var sockMem = { .tup = mems, .loc = FS_SYM_sock };
    
    char const* path = ten_getStrBuf( ten, &pathArg );
    struct stat buf;
    if( stat( path, &buf ) < 0 )
        ten_panic( ten, ten_str( ten, strerror( errno ) ) );
    
    ten_Tup rets = ten_pushA( ten, "U" );
    ten_Var ret  = { .tup = &rets, .loc = 0 };
    
    switch( buf.st_mode & S_IFMT ) {
        case S_IFBLK:  ten_copy( ten, &bdevMem, &ret ); break;
        case S_IFCHR:  ten_copy( ten, &cdevMem, &ret ); break;
        case S_IFDIR:  ten_copy( ten, &dirMem, &ret );  break;
        case S_IFIFO:  ten_copy( ten, &fifoMem, &ret ); break;
        case S_IFLNK:  ten_copy( ten, &linkMem, &ret ); break;
        case S_IFREG:  ten_copy( ten, &regMem, &ret );  break;
        case S_IFSOCK: ten_copy( ten, &sockMem, &ret ); break;
    }
    
    return rets;
}

ten_Tup
tf_size( ten_PARAMS ) {
    ten_Var pathArg = { .tup = args, .loc = 0 };
    ten_expect( ten, "path", ten_strType( ten ), &pathArg );
    
    char const* path = ten_getStrBuf( ten, &pathArg );
    struct stat buf;
    if( stat( path, &buf ) < 0 )
        ten_panic( ten, ten_str( ten, strerror( errno ) ) );
    
    return ten_pushA( ten, "D", (double)buf.st_size );
}

ten_Tup
tf_cwd( ten_PARAMS ) {
    char buf[PATH_MAX];
    
    ten_Tup rets = ten_pushA( ten, "U" );
    ten_Var ret = { .tup = &rets, .loc = 0 };
    
    if( getcwd( buf, sizeof(buf) ) != NULL )
        ten_newStr( ten, buf, strlen( buf ), &ret );
    
    return rets;
}

void
ten_export( ten_State* ten, ten_Var* export ) {
    
    ten_Tup vars = ten_pushA( ten, "U" );
    ten_Var datVar = { .tup = &vars, .loc = 0 };
    
    ten_DatInfo* fsInfo = ten_addDatInfo(
        ten,
        &(ten_DatConfig){
            .tag   = "FsState",
            .size  = sizeof(FsState),
            .mems  = FS_LAST,
            .destr = NULL
        }
    );
    FsState* fs = ten_newDat( ten, fsInfo, &datVar );
    
    ten_setMember( ten, &datVar, FS_SYM_owner, ten_sym( ten, "owner" ) );
    ten_setMember( ten, &datVar, FS_SYM_group, ten_sym( ten, "group" ) );
    ten_setMember( ten, &datVar, FS_SYM_world, ten_sym( ten, "world" ) );
    ten_setMember( ten, &datVar, FS_SYM_self, ten_sym( ten, "self" ) );
    ten_setMember( ten, &datVar, FS_SYM_read, ten_sym( ten, "read" ) );
    ten_setMember( ten, &datVar, FS_SYM_write, ten_sym( ten, "write" ) );
    ten_setMember( ten, &datVar, FS_SYM_exec, ten_sym( ten, "exec" ) );
    ten_setMember( ten, &datVar, FS_SYM_bdev, ten_sym( ten, "bdev" ) );
    ten_setMember( ten, &datVar, FS_SYM_cdev, ten_sym( ten, "cdev" ) );
    ten_setMember( ten, &datVar, FS_SYM_dir, ten_sym( ten, "dir" ) );
    ten_setMember( ten, &datVar, FS_SYM_fifo, ten_sym( ten, "fifo" ) );
    ten_setMember( ten, &datVar, FS_SYM_link, ten_sym( ten, "link" ) );
    ten_setMember( ten, &datVar, FS_SYM_reg, ten_sym( ten, "reg" ) );
    ten_setMember( ten, &datVar, FS_SYM_sock, ten_sym( ten, "sock" ) );
    
    
    fs->globInfo = ten_addDatInfo(
        ten,
        &(ten_DatConfig){
            .tag   = "Glob",
            .size  = sizeof(Glob),
            .mems  = 0,
            .destr = globD
        }
    );
    
    
    
    ten_export_fun( glob, &datVar, "path", NULL );
    ten_export_fun( can, &datVar, "whom", "what", "path", NULL );
    ten_export_fun( type, &datVar, "path", NULL );
    ten_export_fun( size, &datVar, "size", NULL );
    ten_export_fun( cwd, &datVar, NULL );
    
    ten_pop( ten );
}
