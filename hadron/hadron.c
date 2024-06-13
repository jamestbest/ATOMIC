#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "hadron_internal.h"

static Vector create_vector()
{
    return (Vector){
        .arr = malloc(sizeof (MallocInfo*) * MIN_MALLOC_INFO_ARRAY_SIZE),
        .capacity = MIN_MALLOC_INFO_ARRAY_SIZE,
        .pos = 0
    };
}

static bool vector_at_capacity(const Vector* arr) {
    return arr->pos == arr->capacity;
}

static void vector_resize(Vector* arr) {
    const uint new_capacity = arr->capacity << 1;

    void* new_memory = realloc(arr->arr, sizeof (void*) * new_capacity);

    if (!new_memory) {
        exit(ENOMEM);
    }

    arr->capacity = new_capacity;
    arr->arr = new_memory;
}

static void vector_add(Vector* arr, void* element) {
    if (arr->arr == NULL) *arr = create_vector();

    if (vector_at_capacity(arr))
        vector_resize(arr);

    arr->arr[arr->pos++] = element;
}

static void vector_remove_by_index(Vector* arr, const uint index) {
    const uint elements_right = arr->pos - index - 1;

    arr->pos--;

    if (elements_right == 0) {
        return;
    }

    memmove(arr->arr[index], arr->arr[index + 1], elements_right * sizeof (void*));
}

static bool malloc_info_at_capacity() {
    return vector_at_capacity(&malloc_info_array);
}

static void malloc_info_resize() {
    vector_resize(&malloc_info_array);
}

static void malloc_info_add(MallocInfo* info)
{
    vector_add(&malloc_info_array, info);
}

static void malloc_info_remove_by_index(const uint index) {
    vector_remove_by_index(&malloc_info_array, index);
}

/*   |x| |x| |x| | | | |
 *       ^-i=1   ^-pos = 3
 */

static bool malloc_info_remove_by_ptr(const MallocInfo* info) {
    for (int i = 0; i < malloc_info_array.pos; ++i) {
        const MallocInfo* ptr = malloc_info_array.arr[i];

        if (ptr == info) malloc_info_remove_by_index(i);
    }
    return false;
}

static MallocInfoPosition malloc_info_get_by_id(const uint id) {
    for (int i = 0; i < malloc_info_array.pos; ++i) {
        MallocInfo* info = malloc_info_array.arr[i];

        if (info->id == id) return (MallocInfoPosition){info, i};
    }
    return (MallocInfoPosition){NULL, -1};
}

static Vector malloc_info_get_all_by_address(const void* ptr) {
    Vector collection = create_vector();
    for (int i = 0; i < malloc_info_array.pos; ++i) {
        MallocInfo* info = malloc_info_array.arr[i];

        if (info->address_given == ptr) {
            vector_add(&collection, info);
        }
    }
    return collection;
}

static MallocInfoPosition malloc_info_get_unique(const void* ptr) {
    for (int i = 0; i < malloc_info_array.pos; ++i) {
        MallocInfo* info = malloc_info_array.arr[i];

        if (info->address_given == ptr && info->id == *((uint*)ptr - 1)) {
            return (MallocInfoPosition){info, i};
        }
    }
    return (MallocInfoPosition){NULL, -1};
}

static void copy_sequence(void* sequence_start) {
    memcpy(sequence_start, SPECIAL_SEQUENCE, SPECIAL_SEQUENCE_LENGTH);
}

static void print_context(const Context context) {
    if (context.file_name == NULL && context.line_number == -1) {
        printf("TOP LEVEL CONTEXT\n");
        return;
    }

    printf("'%s' LINE: %d (%s)\n", context.file_name, context.line_number, context.func_sig);
}

static void print_info(const MallocInfo* info) {
    printf(
        "Malloc information below\n"
        "id: %d\n"
        "malloced in file: %s\n"
        "         on line: %d\n"
        "         in func: %s\n"
        "Requested: %d, Allocated: %d\n"
        "Base: %p, Given: %p\n"
        "mallocs: %d\n"
        "frees: %d\n",
        info->id,
        info->initializationContext.file_name,
        info->initializationContext.line_number,
        info->initializationContext.func_sig,
        info->size_requested,
        info->size_allocated,
        info->address_base,
        info->address_given,
        info->malloc_count,
        info->free_count
    );
    fputs("last freed by: ", stdout);
    if (info->lastFreeContext.file_name != NULL)
        printf("%s : %u (%s)\n",
            info->lastFreeContext.file_name,
            info->lastFreeContext.line_number,
            info->lastFreeContext.func_sig
            );
    else
        fputs("None", stdout);
    putchar('\n');

    fputs("Realloc of: ", stdout);
    if (info->realloc_of_id != -1)
        printf("%u",
            info->realloc_of_id);
    else
        fputs("None", stdout);
    putchar('\n');

    fputs("Realloced to: ", stdout);
    if (info->realloced_to_id != -1)
        printf("%u",
            info->realloced_to_id);
    else
        fputs("None", stdout);
    putchar('\n');

    fputs("\n\n", stdout);
}

static void print_message_type(MessageType type) {
    switch (type) {
        case INFORM:
            fputs(C_BLU"HADRON::INFO: "C_RST, stdout);
            break;
        case ERROR:
            fputs(C_RED"HADRON::ERROR: "C_RST, stdout);
            break;
        case WARNING:
            fputs(C_MGN"HADRON::WARN: "C_RST, stdout);
            break;
        default:
            assert(false);
    }
}

static void display_message_with_context(const MessageType type, const Context context, const char* message, ...) {
    va_list args;

    va_start(args, message);
    print_message_type(type);
    print_context(context);
    vprintf(message, args);
    va_end(args);
}

static void display_message(const MessageType type, const char* message, ...) {
    va_list args;

    va_start(args, message);
    print_message_type(type);
    vprintf(message, args);
    va_end(args);
}

static int verify_special_sequence(const MallocInfo* info) {
    return memcmp(SPECIAL_SEQUENCE, info->address_of_special_sequence, SPECIAL_SEQUENCE_LENGTH) == 0;
}

static int verify_id(const MallocInfo* info) {
    return *(uint*)info->address_base == info->id;
}

static bool verify_id_and_special_sequence(const MallocInfo* info) {
    return verify_id(info) && verify_special_sequence(info);
}

void gnome_sort(int (*compare)(const void*, const void*), const Vector* items) {
    int pos = 0;

    while (pos < items->pos) {
        if (pos == 0 || compare(items->arr[pos], items->arr[pos - 1]) >= 0) {
            pos++;
        } else {
            void* temp = items->arr[pos];
            items->arr[pos] = items->arr[pos - 1];
            items->arr[pos - 1] = temp;
            pos--;
        }
    }
}

static int compare_file_context(const FileContext* f1, const FileContext* f2) {
    return strcmp(f1->file_name, f2->file_name);
}

static int compare_file_context_wrapper(const void* f1, const void* f2) {
    return compare_file_context(f1, f2);
}

static void sort_files(const Vector* files) {
    gnome_sort(compare_file_context_wrapper, files);
}

static int compare_func_context(const FuncContext* f1, const FuncContext* f2) {
    return f1->line_number - f2->line_number;
}

static int compare_func_context_wrapper(const void* f1, const void* f2) {
    return compare_func_context(f1, f2);
}

static void sort_funcs(const Vector* funcs) {
    gnome_sort(compare_func_context_wrapper, funcs);
}

static bool verify_special_sequence_with_message(const MallocInfo* info, const Context context, const char* context_message) {
    const int cmp_seq = verify_special_sequence(info);
    const bool cmp_id = verify_id(info);

    if (cmp_seq != 1) {
        display_message_with_context(
            ERROR,
            context,
            "Corrupted SPECIAL_SEQUENCE found; expected '%s' got '%"STRINGIFY(SPECIAL_SEQUENCE_LENGTH)"."STRINGIFY(SPECIAL_SEQUENCE_LENGTH)"s' WHEN %s",
            SPECIAL_SEQUENCE,
            info->address_of_special_sequence,
            context_message
        );
        print_info(info);
    }

    if (cmp_id != 1) {
        display_message_with_context(
            ERROR,
            context,
            "Corrupted id found; expected '%d' got '%d'. Id is placed before user given memory could be corrupted from previous allocation. When %s",
            info->id,
            *((uint*)info->address_given - 1),
            context_message
        );
        print_info(info);
    }

    return false;
}

void* hadron_malloc(const uint bytes, char* file_name, const uint line_number, char* func_sig)
{
    const Context context = {file_name, line_number, func_sig};
    const uint alloc_amount = bytes + sizeof (uint) + SPECIAL_SEQUENCE_LENGTH;

    char* memory = malloc(alloc_amount);

    const uint id = current_id++;

    *(uint*)memory = id;
    copy_sequence(memory + sizeof (uint) + bytes);

    MallocInfo* info = malloc(sizeof (MallocInfo));

    *info = (MallocInfo){
        .id = id,
        .initializationContext = context,

        .size_requested = bytes,
        .size_allocated = alloc_amount,

        .address_base = memory,
        .address_given = memory + sizeof (uint),

        .address_of_special_sequence = memory + sizeof (uint) + bytes,

        .malloc_count = 1,
        .free_count = 0,

        .lastFreeContext = (Context){NULL, -1},
        .realloc_of_id = -1,
        .realloced_to_id = -1,
    };

    malloc_info_add(info);

    return info->address_given;
}

void* hadron_realloc(void* ptr, uint new_bytes, char* file_name, uint line_number, char* func_sig)
{
    const Context context = (Context){file_name, line_number, func_sig};

    const MallocInfoPosition position = malloc_info_get_unique(ptr);

    bool found_info = position.info != NULL;

    if (!found_info) {
        const Vector arr = malloc_info_get_all_by_address(ptr);

        if (arr.pos == 0) {
            display_message_with_context(
                ERROR,
                context,
                "Reallocation unable to find any allocation information. Creating new information, but assume corrupt\n"
                "Wanted to find ptr: %p",
                ptr
            );
        }
        else {
            display_message_with_context(
                ERROR,
                context,
                "Reallocation unable to find unique allocation information. "
                "Found some allocations for same address but different ids (id may have been corrupted)\nId found: %u Id wanted: %u\n"
                "Wanted to find ptr: %p\n",

                ptr
            );
            for (int i = 0; i < arr.pos; ++i) {
                const MallocInfo* info = arr.arr[i];

                print_info(info);
            }
        }
    }

    if (found_info) {
        verify_special_sequence_with_message(
            position.info,
            context,
            "Reallocating memory\n"
        );
    }

    const uint alloc_amount = new_bytes + sizeof (uint) + SPECIAL_SEQUENCE_LENGTH;

    void* memory = found_info ? position.info->address_base : (uint*)ptr - 1;


    char* new_mem;
    if (!HADRON_RETAIN)
        new_mem = realloc(memory, alloc_amount);
    else
        new_mem = malloc(alloc_amount);

    if (!new_mem) {
        exit(ENOMEM);
    }

    MallocInfo* new_info = malloc(sizeof (MallocInfo));

    *new_info = (MallocInfo) {
        .address_base = new_mem,
        .address_given = new_mem + sizeof (uint),
        .address_of_special_sequence = new_mem + sizeof (uint) + new_bytes,

        .free_count = found_info ? position.info->free_count : 0,
        .malloc_count = found_info ? position.info->malloc_count : 1,

        .id = current_id++,
        .realloc_of_id = found_info ? position.info->id : -1,
        .realloced_to_id = -1,

        .size_requested = new_bytes,
        .size_allocated = alloc_amount,

        .initializationContext = context,
        .lastFreeContext = found_info ? position.info->lastFreeContext : (Context){NULL, -1, NULL}
    };

    malloc_info_add(new_info);

    if (found_info) {
        position.info->realloced_to_id = new_info->id;
        position.info->free_count++;
    }

    *(uint*)new_info->address_base = new_info->id;
    copy_sequence(new_info->address_of_special_sequence);

    return new_info->address_given;
}

void hadron_free(void* ptr, char* file_name, uint line_number, char* func_sig)
{
    const Context context = (Context){file_name, line_number, func_sig};
    const MallocInfoPosition position = malloc_info_get_unique(ptr);

    if (!position.info) {
        const Vector arr = malloc_info_get_all_by_address(ptr);

        if (arr.pos == 0) {
            display_message_with_context(
                ERROR,
                context,
                "Given ptr to free that did not have a malloc information assosiated with it. ptr: %p\n",
                ptr
            );
            return;
        }

        display_message_with_context(
            ERROR,
            context,
            "Free given pointer to memory that cannot be identified. Listing all previous allocations from address\n"
            "Wanted to free ptr: %p\n",
            ptr
        );

        const uint id = *((uint*)ptr - 1);

        bool allFreed = true;
        const bool invalidId = id > current_id;

        for (uint i = 0; i < arr.pos; ++i) {
            const MallocInfo* info = arr.arr[i];

            if (info->free_count == 0) allFreed = false;
        }

        if (allFreed) {
            puts("All associated mallocs of this address have been freed. Likely double free");
        }

        if (invalidId) {
            printf("Corrupted id (%u) found; greater than any allocated (%u)\n", id, current_id);
        }

        for (uint i = 0; i < arr.pos; ++i) {
            const MallocInfo* info = arr.arr[i];

            printf(info->free_count > 0 ? "This data has been freed\n" : "This data has not been freed, could be that the id was corrupted\n");

            print_info(info);
        }

        return;
    }

    // malloc_info_remove_by_index(position.index);

    if (position.info->free_count > 0) {
        display_message_with_context(
            ERROR,
            context,
            "Double free found when freeing %p. Already has %u free(s)\n",
            ptr,
            position.info->free_count
        );
        return;
    }

    position.info->free_count += 1;
    position.info->lastFreeContext = context;

    if (!HADRON_RETAIN)
        free(position.info->address_base);
}

FileContext* get_file_context(const Vector* files, const char* file_name) {
    for (uint i = 0; i < files->pos; ++i) {
        FileContext* file_context = files->arr[i];

        if (file_context->file_name == file_name) return file_context;
    }
    return NULL;
}

FileContext* add_file_context(Vector* files, const char* file_name) {
    FileContext* file_context = malloc(sizeof (FileContext));

    *file_context = (FileContext) {
        .file_name = file_name,
        .functions = create_vector(),

        .bytes = {},
        .counts = {}
    };

    vector_add(files, file_context);

    return file_context;
}

FileContext* add_and_get_file_context(Vector* files, const char* file_name) {
    FileContext* file_context = get_file_context(files, file_name);

    if (!file_context) file_context = add_file_context(files, file_name);

    if (!file_context) assert(false);

    return file_context;
}

FuncContext* get_func_context(const FileContext* file_context, const MallocInfo* info) {
    for (uint i = 0; i < file_context->functions.pos; ++i) {
        FuncContext* func_context = file_context->functions.arr[i];

        if (func_context->func_sig == info->initializationContext.func_sig &&
            func_context->line_number == info->initializationContext.line_number
        ) return func_context;
    }
    return NULL;
}

FuncContext* add_func_context(FileContext* file_context, const MallocInfo* info) {
    FuncContext* func_context = malloc(sizeof (FuncContext));

    *func_context = (FuncContext){
        .func_sig = info->initializationContext.func_sig,
        .line_number = info->initializationContext.line_number,

        .bytes = {},
        .counts = {}
    };

    vector_add(&file_context->functions, func_context);

    return func_context;
}

FuncContext* add_and_get_func_context(FileContext* file_context, const MallocInfo* info) {
    FuncContext* func_context = get_func_context(file_context, info);

    if (!func_context) func_context = add_func_context(file_context, info);

    if (!func_context) assert(false);

    return func_context;
}

void update_func_context(FuncContext* func_context, const MallocInfo* info) {
    const bool freed = info->free_count == 1;

    Bytes* total = &func_context->bytes.total;

    total->allocated += info->size_allocated;
    total->requested += info->size_requested;

    // there are no errors for a freed piece of data, unless we are retaining the allocation
    if (freed && !HADRON_RETAIN) return;

    const bool double_free = info->free_count > 1;
    const bool non_free = info->free_count < 1;

    const bool corrupt_special = !verify_special_sequence(info);
    const bool corrupt_id = !verify_id(info);

    const bool error = double_free || non_free || corrupt_special || corrupt_id;

    if (error) {
        Bytes* total_errors = &func_context->bytes.total_errors;

        total_errors->allocated += info->size_allocated;
        total_errors->requested += info->size_requested;

        func_context->counts.witherrors++;
    }

    if (double_free) {
        Bytes* doublefree = &func_context->bytes.doublefree;

        doublefree->allocated += info->size_allocated;
        doublefree->requested += info->size_requested;

        func_context->counts.doublefrees++;
    }

    if (non_free) {
        Bytes* nonfree = &func_context->bytes.nonfree;

        nonfree->allocated += info->size_allocated;
        nonfree->requested += info->size_requested;

        func_context->counts.nonfrees++;
    }

    if (corrupt_special && !freed) {
        Bytes* corrupt = &func_context->bytes.corruptSpecial;

        corrupt->allocated += info->size_allocated;
        corrupt->requested += info->size_requested;

        func_context->counts.corruptSpecials++;
    }

    if (corrupt_id && !freed) {
        Bytes* corrupt = &func_context->bytes.corruptId;

        corrupt->allocated += info->size_allocated;
        corrupt->requested += info->size_requested;

        func_context->counts.corruptIds++;
    }
}

void update_bytes(Bytes* dst, const Bytes src) {
    dst->allocated += src.allocated;
    dst->requested += src.requested;
}

void update_byte_count(ByteCount* dst, const ByteCount src) {
    update_bytes(&dst->doublefree, src.doublefree);
    update_bytes(&dst->nonfree, src.nonfree);
    update_bytes(&dst->corruptSpecial, src.corruptSpecial);
    update_bytes(&dst->corruptId, src.corruptId);

    update_bytes(&dst->total, src.total);
    update_bytes(&dst->total_errors, src.total_errors);
}

void update_counts(Counts* dst, const Counts src) {
    dst->doublefrees += src.doublefrees;
    dst->nonfrees += src.nonfrees;
    dst->corruptSpecials += src.corruptSpecials;
    dst->corruptIds += src.corruptIds;

    dst->witherrors += src.witherrors;
}

void update_file_context(FileContext* file_context, const FuncContext* func_context) {
    update_byte_count(&file_context->bytes, func_context->bytes);
    update_counts(&file_context->counts, func_context->counts);
}

void print_single_error(const char* error_header, const Bytes error, const uint64_t total_of_error, const uint64_t total_errors) {
    printf(
        "    %s: %llu (%.2f%%)\n"
            "        total Requested: %llu bytes (%.2fKiB)\n"
            "        total Allocated: %llu bytes (%.2fKiB)\n",

            error_header,

            total_of_error,
            (double) total_of_error / total_errors * 100,
            error.requested,
            (double) error.requested / 1024,
            error.allocated,
            (double) error.allocated / 1024
    );
}

void print_error_breakdown(const Counts total_counts, const ByteCount total_bytes) {
    fputs("Error makeup:\n", stdout);

    print_single_error("Non frees", total_bytes.nonfree, total_counts.nonfrees, total_counts.witherrors);
    print_single_error("Double frees", total_bytes.doublefree, total_counts.doublefrees, total_counts.witherrors);
    print_single_error("Corrupt special sequence", total_bytes.corruptSpecial, total_counts.corruptSpecials, total_counts.witherrors);
    print_single_error("Corrupt ids", total_bytes.corruptId, total_counts.corruptIds, total_counts.witherrors);
}

void print_total_and_errors(const Counts counts, const Bytes totals, const Bytes errors) {
    display_message(
        counts.witherrors == 0 ? INFORM : ERROR,
        "Overall allocation information: \n"
        "Total allocations: %u\n"
        "    total Requested: %llu bytes (%.2fKiB)\n"
        "    total Allocated: %llu bytes (%.2fKiB)\n"
        "\n"
        "Error allocations %llu / %u (%.2f%%)\n"
        "    total Requested: %llu bytes (%.2fKiB) (%.2f%%)\n"
        "    total Allocated: %llu bytes (%.2fKiB) (%.2f%%)\n"
        "\n",

        malloc_info_array.pos,
        totals.requested,
        (double) totals.requested / 1024,
        totals.allocated,
        (double) totals.allocated / 1024,

        counts.witherrors,
        malloc_info_array.pos,
        (double) counts.witherrors / malloc_info_array.pos * 100,
        errors.requested,
        (double) errors.requested / 1024,
        (double) errors.requested / errors.requested * 100,
        errors.allocated,
        (double) errors.allocated / 1024,
        (double) errors.allocated / errors.allocated * 100
    );
}

void hadron_verify_two(void) {
    ByteCount total_bytes = {};
    Counts total_counts = {};

    Vector files = create_vector();

    // gather bytes and counts for each malloc info
    for (uint i = 0; i < malloc_info_array.pos; ++i) {
        const MallocInfo* info = malloc_info_array.arr[i];

        FileContext* file_context = add_and_get_file_context(&files, info->initializationContext.file_name);
        FuncContext* func_context = add_and_get_func_context(file_context, info);

        update_func_context(func_context, info);
    }

    sort_files(&files);

    // update information in the files & sort the functions
    for (uint i = 0; i < files.pos; ++i) {
        FileContext* file_context = files.arr[i];

        sort_funcs(&file_context->functions);

        for (uint j = 0; j < file_context->functions.pos; ++j) {
            const FuncContext* func_context = file_context->functions.arr[j];

            update_file_context(file_context, func_context);
        }

        update_byte_count(&total_bytes, file_context->bytes);
        update_counts(&total_counts, file_context->counts);
    }

    print_total_and_errors(total_counts, total_bytes.total, total_bytes.total_errors);
    if (HADRON_VERBOSE || true) {
        print_error_breakdown(total_counts, total_bytes);
    }

    for (uint i = 0; i < files.pos; ++i) {
        const FileContext* file_context = files.arr[i];

        display_message(
            file_context->counts.witherrors == 0 ? INFORM : ERROR,
            "Information for file: %s\n",
            file_context->file_name
        );

        print_total_and_errors(file_context->counts, file_context->bytes.total, file_context->bytes.total_errors);
        if (HADRON_VERBOSE || true) {
            print_error_breakdown(file_context->counts, file_context->bytes);
        }

        for (uint j = 0; j < file_context->functions.pos; ++j) {
            const FuncContext* func_context = file_context->functions.arr[j];

            display_message_with_context(
                func_context->counts.witherrors == 0 ? INFORM : ERROR,
                (Context){file_context->file_name, func_context->line_number, func_context->func_sig},
                "Information on function\n"
            );

            print_total_and_errors(func_context->counts, func_context->bytes.total, func_context->bytes.total_errors);
            if (HADRON_VERBOSE || true) {
                print_error_breakdown(func_context->counts, func_context->bytes);
            }
        }
    }
    
    printf("Overall information");
}