typedef int64_t zend_long;
typedef uint64_t zend_ulong;
typedef int64_t zend_off_t;

typedef unsigned char zend_bool;
typedef unsigned char zend_uchar;
typedef uintptr_t zend_type;

typedef enum {
  SUCCESS =  0,
  FAILURE = -1,		/* this MUST stay a negative number, or it may affect functions! */
} ZEND_RESULT_CODE;

typedef struct _zend_object_handlers zend_object_handlers;
typedef struct _zend_class_entry     zend_class_entry;
typedef union  _zend_function        zend_function;
typedef struct _zend_execute_data    zend_execute_data;

typedef struct _zval_struct     zval;

typedef struct _zend_refcounted zend_refcounted;
typedef struct _zend_string     zend_string;
typedef struct _zend_array      zend_array;
typedef struct _zend_object     zend_object;
typedef struct _zend_resource   zend_resource;
typedef struct _zend_reference  zend_reference;
typedef struct _zend_ast_ref    zend_ast_ref;
typedef struct _zend_ast        zend_ast;

typedef int  (*compare_func_t)(const void *, const void *);
typedef void (*swap_func_t)(void *, void *);
typedef void (*sort_func_t)(void *, size_t, size_t, compare_func_t, swap_func_t);
typedef void (*dtor_func_t)(zval *pDest);
typedef void (*copy_ctor_func_t)(zval *pElement);

typedef union _zend_value {
    zend_long         lval;				/* long value */
    double            dval;				/* double value */
    zend_refcounted  *counted;
    zend_string      *str;
    zend_array       *arr;
    zend_object      *obj;
    zend_resource    *res;
    zend_reference   *ref;
    zend_ast_ref     *ast;
    zval             *zv;
    void             *ptr;
    zend_class_entry *ce;
    zend_function    *func;
    struct {
        uint32_t w1;
        uint32_t w2;
    } ww;
} zend_value;

struct _zval_struct {
    zend_value        value;			/* value */
    union {
        struct {
            zend_uchar    type;			/* active type */
            zend_uchar    type_flags;
            union {
                uint16_t  extra;        /* not further specified */
            } u;
        } v;
        uint32_t type_info;
    } u1;
    union {
        uint32_t     next;                 /* hash collision chain */
        uint32_t     cache_slot;           /* cache slot (for RECV_INIT) */
        uint32_t     opline_num;           /* opline number (for FAST_CALL) */
        uint32_t     lineno;               /* line number (for ast nodes) */
        uint32_t     num_args;             /* arguments number for EX(This) */
        uint32_t     fe_pos;               /* foreach position */
        uint32_t     fe_iter_idx;          /* foreach iterator index */
        uint32_t     access_flags;         /* class constant access flags */
        uint32_t     property_guard;       /* single property guard */
        uint32_t     constant_flags;       /* constant flags */
        uint32_t     extra;                /* not further specified */
    } u2;
};

typedef struct _zend_refcounted_h {
    uint32_t         refcount;			/* reference counter 32-bit */
    union {
        uint32_t type_info;
    } u;
} zend_refcounted_h;

struct _zend_refcounted {
    zend_refcounted_h gc;
};

struct _zend_string {
    zend_refcounted_h gc;
    zend_ulong        h;                /* hash value */
    size_t            len;
    char              val[1];
};

typedef struct _Bucket {
    zval              val;
    zend_ulong        h;                /* hash value (or numeric index)   */
    zend_string      *key;              /* string key or NULL for numerics */
} Bucket;

typedef struct _zend_array HashTable;

struct _zend_array {
    zend_refcounted_h gc;
    union {
        struct {
            zend_uchar    flags;
            zend_uchar    _unused;
            zend_uchar    nIteratorsCount;
            zend_uchar    _unused2;
        } v;
        uint32_t flags;
    } u;
    uint32_t          nTableMask;
    Bucket           *arData;
    uint32_t          nNumUsed;
    uint32_t          nNumOfElements;
    uint32_t          nTableSize;
    uint32_t          nInternalPointer;
    zend_long         nNextFreeElement;
    dtor_func_t       pDestructor;
};

typedef uint32_t HashPosition;

typedef struct _HashTableIterator {
    HashTable    *ht;
    HashPosition  pos;
} HashTableIterator;

struct _zend_object {
    zend_refcounted_h gc;
    uint32_t          handle; // TODO: may be removed ???
    zend_class_entry *ce;
    const zend_object_handlers *handlers;
    HashTable        *properties;
    zval              properties_table[1];
};

struct _zend_resource {
    zend_refcounted_h gc;
    int               handle; // TODO: may be removed ???
    int               type;
    void             *ptr;
};

typedef struct _zend_property_info zend_property_info;

typedef struct {
    size_t num;
    size_t num_allocated;
    zend_property_info *ptr[1];
} zend_property_info_list;

typedef union {
    zend_property_info *ptr;
    uintptr_t list;
} zend_property_info_source_list;

struct _zend_reference {
    zend_refcounted_h              gc;
    zval                           val;
    zend_property_info_source_list sources;
};

struct _zend_ast_ref {
    zend_refcounted_h gc;
    /*zend_ast        ast; zend_ast follows the zend_ast_ref structure */
};

/* zend_ast.h */
typedef uint16_t zend_ast_kind;
typedef uint16_t zend_ast_attr;

struct _zend_ast {
    zend_ast_kind kind; /* Type of the node (ZEND_AST_* enum constant) */
    zend_ast_attr attr; /* Additional attribute, use depending on node type */
    uint32_t lineno;    /* Line number */
    zend_ast *child[1]; /* Array of children (using struct hack) */
};

/* Same as zend_ast, but with children count, which is updated dynamically */
typedef struct _zend_ast_list {
    zend_ast_kind kind;
    zend_ast_attr attr;
    uint32_t lineno;
    uint32_t children;
    zend_ast *child[1];
} zend_ast_list;

/* Lineno is stored in val.u2.lineno */
typedef struct _zend_ast_zval {
    zend_ast_kind kind;
    zend_ast_attr attr;
    zval val;
} zend_ast_zval;

/* Separate structure for function and class declaration, as they need extra information. */
typedef struct _zend_ast_decl {
    zend_ast_kind kind;
    zend_ast_attr attr; /* Unused - for structure compatibility */
    uint32_t start_lineno;
    uint32_t end_lineno;
    uint32_t flags;
    unsigned char *lex_pos;
    zend_string *doc_comment;
    zend_string *name;
    zend_ast *child[4];
} zend_ast_decl;

typedef void (*zend_ast_process_t)(zend_ast *ast);

/* zend_types.h */
typedef intptr_t zend_intptr_t;
typedef uintptr_t zend_uintptr_t;

/* zend_arena.h */
typedef struct _zend_arena zend_arena;

struct _zend_arena {
    char		*ptr;
    char		*end;
    zend_arena  *prev;
};

/* zend_compile.h */
typedef struct _zend_op_array zend_op_array;
typedef struct _zend_op zend_op;

typedef union _znode_op {
    uint32_t      constant;
    uint32_t      var;
    uint32_t      num;
    uint32_t      opline_num; /*  Needs to be signed */

    // We haven't support for #if..#endif constructions, so resolved manually
    // #if ZEND_USE_ABS_JMP_ADDR
        zend_op       *jmp_addr;
    // #else
    //    uint32_t      jmp_offset;
    // #endif
    // #if ZEND_USE_ABS_CONST_ADDR
        zval          *zv;
    // #endif
} znode_op;

typedef struct _znode { /* used only during compilation */
    zend_uchar op_type;
    zend_uchar flag;
    union {
        znode_op op;
        zval constant; /* replaced by literal/zv */
    } u;
} znode;

typedef struct _zend_ast_znode {
    zend_ast_kind kind;
    zend_ast_attr attr;
    uint32_t lineno;
    znode node;
} zend_ast_znode;

typedef struct _zend_declarables {
    zend_long ticks;
} zend_declarables;

typedef struct _zend_file_context {
    zend_declarables declarables;

    zend_string *current_namespace;
    zend_bool in_namespace;
    zend_bool has_bracketed_namespaces;

    HashTable *imports;
    HashTable *imports_function;
    HashTable *imports_const;

    HashTable seen_symbols;
} zend_file_context;

typedef union _zend_parser_stack_elem {
    zend_ast *ast;
    zend_string *str;
    zend_ulong num;
    unsigned char *ptr;
} zend_parser_stack_elem;

typedef int (*user_opcode_handler_t) (zend_execute_data *execute_data);

struct _zend_op {
    const void *handler;
    znode_op op1;
    znode_op op2;
    znode_op result;
    uint32_t extended_value;
    uint32_t lineno;
    zend_uchar opcode;
    zend_uchar op1_type;
    zend_uchar op2_type;
    zend_uchar result_type;
};

typedef struct _zend_brk_cont_element {
    int start;
    int cont;
    int brk;
    int parent;
    zend_bool is_switch;
} zend_brk_cont_element;

typedef struct _zend_label {
    int brk_cont;
    uint32_t opline_num;
} zend_label;

typedef struct _zend_try_catch_element {
    uint32_t try_op;
    uint32_t catch_op;  /* ketchup! */
    uint32_t finally_op;
    uint32_t finally_end;
} zend_try_catch_element;

typedef struct _zend_live_range {
    uint32_t var; /* low bits are used for variable type (ZEND_LIVE_* macros) */
    uint32_t start;
    uint32_t end;
} zend_live_range;

typedef struct _zend_oparray_context {
    uint32_t   opcodes_size;
    int        vars_size;
    int        literals_size;
    uint32_t   fast_call_var;
    uint32_t   try_catch_offset;
    int        current_brk_cont;
    int        last_brk_cont;
    zend_brk_cont_element *brk_cont_array;
    HashTable *labels;
} zend_oparray_context;

typedef struct _zend_property_info {
    uint32_t offset; /* property offset for object properties or
                          property index for static properties */
    uint32_t flags;
    zend_string *name;
    zend_string *doc_comment;
    zend_class_entry *ce;
    zend_type type;
} zend_property_info;

typedef struct _zend_class_constant {
    zval value; /* access flags are stored in reserved: zval.u2.access_flags */
    zend_string *doc_comment;
    zend_class_entry *ce;
} zend_class_constant;

/* arg_info for internal functions */
typedef struct _zend_internal_arg_info {
    const char *name;
    zend_type type;
    zend_uchar pass_by_reference;
    zend_bool is_variadic;
} zend_internal_arg_info;

/* arg_info for user functions */
typedef struct _zend_arg_info {
    zend_string *name;
    zend_type type;
    zend_uchar pass_by_reference;
    zend_bool is_variadic;
} zend_arg_info;

typedef struct _zend_internal_function_info {
    zend_uintptr_t required_num_args;
    zend_type type;
    zend_bool return_reference;
    zend_bool _is_variadic;
} zend_internal_function_info;

struct _zend_op_array {
    /* Common elements */
    zend_uchar type;
    zend_uchar arg_flags[3]; /* bitset of arg_info.pass_by_reference */
    uint32_t fn_flags;
    zend_string *function_name;
    zend_class_entry *scope;
    zend_function *prototype;
    uint32_t num_args;
    uint32_t required_num_args;
    zend_arg_info *arg_info;
    /* END of common elements */

    int cache_size;     /* number of run_time_cache_slots * sizeof(void*) */
    int last_var;       /* number of CV variables */
    uint32_t T;         /* number of temporary variables */
    uint32_t last;      /* number of opcodes */

    zend_op *opcodes;
    void ** * run_time_cache; ## __run_time_cache
    HashTable * * static_variables_ptr; ## __static_variables_ptr
    HashTable *static_variables;
    zend_string **vars; /* names of CV variables */

    uint32_t *refcount;

    int last_live_range;
    int last_try_catch;
    zend_live_range *live_range;
    zend_try_catch_element *try_catch_array;

    zend_string *filename;
    uint32_t line_start;
    uint32_t line_end;
    zend_string *doc_comment;

    int last_literal;
    zval *literals;

    void *reserved[/* ZEND_MAX_RESERVED_RESOURCES */ 6];
};

/* zend_internal_function_handler */
typedef void (*zif_handler)(/* INTERNAL_FUNCTION_PARAMETERS */ zend_execute_data *execute_data, zval *return_value);

typedef struct _zend_internal_function {
	/* Common elements */
	zend_uchar type;
	zend_uchar arg_flags[3]; /* bitset of arg_info.pass_by_reference */
	uint32_t fn_flags;
	zend_string* function_name;
	zend_class_entry *scope;
	zend_function *prototype;
	uint32_t num_args;
	uint32_t required_num_args;
	zend_internal_arg_info *arg_info;
	/* END of common elements */

	zif_handler handler;
	struct _zend_module_entry *module;
	void *reserved[/* ZEND_MAX_RESERVED_RESOURCES */ 6];
} zend_internal_function;

union _zend_function {
	zend_uchar type;	/* MUST be the first element of this struct! */
	uint32_t   quick_arg_flags;

	struct {
		zend_uchar type;  /* never used */
		zend_uchar arg_flags[3]; /* bitset of arg_info.pass_by_reference */
		uint32_t fn_flags;
		zend_string *function_name;
		zend_class_entry *scope;
		zend_function *prototype;
		uint32_t num_args;
		uint32_t required_num_args;
		zend_arg_info *arg_info;
	} common;

	zend_op_array op_array;
	zend_internal_function internal_function;
};

struct _zend_execute_data {
	const zend_op       *opline;           /* executed opline                */
	zend_execute_data   *call;             /* current call                   */
	zval                *return_value;
	zend_function       *func;             /* executed function              */
	zval                 This;             /* this + call_info + num_args    */
	zend_execute_data   *prev_execute_data;
	zend_array          *symbol_table;
	void               **run_time_cache;   /* cache op_array->run_time_cache */
};

/* zend_llist.h*/
typedef struct _zend_llist_element {
    struct _zend_llist_element *next;
    struct _zend_llist_element *prev;
    char data[1]; /* Needs to always be last in the struct */
} zend_llist_element;

typedef void (*llist_dtor_func_t)(void *);
typedef int (*llist_compare_func_t)(const zend_llist_element **, const zend_llist_element **);
typedef void (*llist_apply_with_args_func_t)(void *data, int num_args, va_list args);
typedef void (*llist_apply_with_arg_func_t)(void *data, void *arg);
typedef void (*llist_apply_func_t)(void *);

typedef struct _zend_llist {
    zend_llist_element *head;
    zend_llist_element *tail;
    size_t count;
    size_t size;
    llist_dtor_func_t dtor;
    unsigned char persistent;
    zend_llist_element *traverse_ptr;
} zend_llist;

typedef zend_llist_element* zend_llist_position;

/* zend_multibyte.h */
typedef struct _zend_encoding zend_encoding;

/* zend_stack.h */
typedef struct _zend_stack {
    int size, top, max;
    void *elements;
} zend_stack;

struct _zend_compiler_globals {
    zend_stack loop_var_stack;

    zend_class_entry *active_class_entry; /* Usually empty */

    zend_string *compiled_filename; /* Usually empty */

    int zend_lineno; /* Usually 0 */

    zend_op_array *active_op_array; /* Usually null */

    HashTable *function_table;	/* function symbol table */
    HashTable *class_table;		/* class table */

    HashTable filenames_table; /* List of loaded files */

    HashTable *auto_globals;  /* List of superglobal variables */

    zend_bool parse_error;
    zend_bool in_compilation;
    zend_bool short_tags;

    zend_bool unclean_shutdown;

    zend_bool ini_parser_unbuffered_errors;

    zend_llist open_files; /* Usually empty */

    struct _zend_ini_parser_param *ini_parser_param;

    uint32_t start_lineno; /* 0 */
    zend_bool increment_lineno; /* 0 */

    zend_string *doc_comment;
    uint32_t extra_fn_flags;

    uint32_t compiler_options; /* set of ZEND_COMPILE_* constants */

    zend_oparray_context context; /* Empty context */
    zend_file_context file_context; /* Empty context */

    zend_arena *arena;

    HashTable interned_strings; /* Cache of all interned string */

    const zend_encoding ** script_encoding_list;
    size_t script_encoding_list_size;
    zend_bool multibyte;
    zend_bool detect_unicode;
    zend_bool encoding_declared;

    zend_ast *ast; /* Usually empty */
    zend_arena *ast_arena;

    zend_stack delayed_oplines_stack;
    HashTable *memoized_exprs; /* Usually empty */
    int memoize_mode;

    void   *map_ptr_base;
    size_t  map_ptr_size;
    size_t  map_ptr_last;

    HashTable *delayed_variance_obligations; /* Usually empty */
    HashTable *delayed_autoloads; /* Usually empty */
};

/* zend_iterators.h */
typedef struct _zend_object_iterator zend_object_iterator;

typedef struct _zend_object_iterator_funcs {
    /* release all resources associated with this iterator instance */
    void (*dtor)(zend_object_iterator *iter);

    /* check for end of iteration (FAILURE or SUCCESS if data is valid) */
    int (*valid)(zend_object_iterator *iter);

    /* fetch the item data for the current element */
    zval *(*get_current_data)(zend_object_iterator *iter);

    /* fetch the key for the current element (optional, may be NULL). The key
     * should be written into the provided zval* using the ZVAL_* macros. If
     * this handler is not provided auto-incrementing integer keys will be
     * used. */
    void (*get_current_key)(zend_object_iterator *iter, zval *key);

    /* step forwards to next element */
    void (*move_forward)(zend_object_iterator *iter);

    /* rewind to start of data (optional, may be NULL) */
    void (*rewind)(zend_object_iterator *iter);

    /* invalidate current value/key (optional, may be NULL) */
    void (*invalidate_current)(zend_object_iterator *iter);
} zend_object_iterator_funcs;

struct _zend_object_iterator {
    zend_object std;
    zval data;
    const zend_object_iterator_funcs *funcs;
    zend_ulong index; /* private to fe_reset/fe_fetch opcodes */
};

typedef struct _zend_class_iterator_funcs {
    zend_function *zf_new_iterator;
    zend_function *zf_valid;
    zend_function *zf_current;
    zend_function *zf_key;
    zend_function *zf_next;
    zend_function *zf_rewind;
} zend_class_iterator_funcs;

/* zend.h */
struct _zend_serialize_data;
struct _zend_unserialize_data;

typedef struct _zend_serialize_data zend_serialize_data;
typedef struct _zend_unserialize_data zend_unserialize_data;

typedef struct _zend_class_name {
    zend_string *name;
    zend_string *lc_name;
} zend_class_name;

typedef struct _zend_trait_method_reference {
    zend_string *method_name;
    zend_string *class_name;
} zend_trait_method_reference;

typedef struct _zend_trait_precedence {
    zend_trait_method_reference trait_method;
    uint32_t num_excludes;
    zend_string *exclude_class_names[1];
} zend_trait_precedence;

typedef struct _zend_trait_alias {
    zend_trait_method_reference trait_method;

    /**
    * name for method to be added
    */
    zend_string *alias;

    /**
    * modifiers to be set on trait method
    */
    uint32_t modifiers;
} zend_trait_alias;

struct _zend_class_entry {
    char type;
    zend_string *name;
    /* class_entry or string depending on ZEND_ACC_LINKED */
    union {
        zend_class_entry *parent;
        zend_string *parent_name;
    };
    int refcount;
    uint32_t ce_flags;

    int default_properties_count;
    int default_static_members_count;
    zval *default_properties_table;
    zval *default_static_members_table;
    zval ** static_members_table;
    HashTable function_table;
    HashTable properties_info;
    HashTable constants_table;

    struct _zend_property_info **properties_info_table;

    zend_function *constructor;
    zend_function *destructor;
    zend_function *clone;
    zend_function *__get;
    zend_function *__set;
    zend_function *__unset;
    zend_function *__isset;
    zend_function *__call;
    zend_function *__callstatic;
    zend_function *__tostring;
    zend_function *__debugInfo;
    zend_function *serialize_func;
    zend_function *unserialize_func;

    /* allocated only if class implements Iterator or IteratorAggregate interface */
    zend_class_iterator_funcs *iterator_funcs_ptr;

    /* handlers */
    union {
        zend_object* (*create_object)(zend_class_entry *class_type);
        int (*interface_gets_implemented)(zend_class_entry *iface, zend_class_entry *class_type); /* a class implements this interface */
    };
    zend_object_iterator *(*get_iterator)(zend_class_entry *ce, zval *object, int by_ref);
    zend_function *(*get_static_method)(zend_class_entry *ce, zend_string* method);

    /* serializer callbacks */
    int (*serialize)(zval *object, unsigned char **buffer, size_t *buf_len, zend_serialize_data *data);
    int (*unserialize)(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data);

    uint32_t num_interfaces;
    uint32_t num_traits;

    /* class_entry or string(s) depending on ZEND_ACC_LINKED */
    union {
        zend_class_entry **interfaces;
        zend_class_name *interface_names;
    };

    zend_class_name *trait_names;
    zend_trait_alias **trait_aliases;
    zend_trait_precedence **trait_precedences;

    union {
        struct {
            zend_string *filename;
            uint32_t line_start;
            uint32_t line_end;
            zend_string *doc_comment;
        } user;
        struct {
            const struct _zend_function_entry *builtin_functions;
            struct _zend_module_entry *module;
        } internal;
    } info;
};


typedef struct _zend_executor_globals zend_executor_globals;


extern zend_executor_globals executor_globals;
extern struct _zend_compiler_globals compiler_globals;

/**
 * Zend Hash API
 */
extern int zend_hash_del(HashTable *ht, zend_string *key);
extern zval* zend_hash_find(const HashTable *ht, zend_string *key);
