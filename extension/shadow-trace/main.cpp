#define _GNU_SOURCE
extern "C"
{
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <asm/errno.h>
#include <link.h>
#include <dlfcn.h>
#include <elf.h>
#include <unistd.h>
#include <cxxabi.h>
#include <elfutils/libdwfl.h>
#include <libunwind.h>
#include <libunwind-x86_64.h>
#include <libunwind-ptrace.h>
#include "ltrace.h"
#include "proc.h"
#include "library.h"
#include "param.h"
#include "type.h"
#include "value.h"
#include "fetch.h"
#include "value_dict.h"
#include "prototype.h"
#include "breakpoint.h"
#include "string.h"
#include "ltrace-elf.h"
}

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <memory>
#include "CallLibraryInfo.hpp"
#include "InputParser.hpp"
#define TARGET_PROGRAM "shadow"
std::ostream *logger_stream = &std::cout;
std::ofstream logger;

std::string demangle(const char *name)
{
    int status = -4;
    std::unique_ptr<char, void (*)(void *)> res{
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free};

    return (status == 0) ? res.get() : name;
}

static int backtrace(unw_addr_space_t *as, pid_t pid, int error_code)
{
    unw_word_t ip, sp, off, return_value;
    int n = 0;
    unw_proc_info_t pi;
    char buf[512];

    unw_cursor_t unw_c;
    int ret;
    void *ui;
    struct user_regs_struct regs;
    std::vector<CallLibraryInfo> v_backtrace;

    ui = _UPT_create(pid);
    ret = unw_init_remote(&unw_c, *as, ui);
    if (0 > ret)
    {
        goto __cleanup;
    }

    if (0 > ret)
    {
        goto __cleanup;
    }

    do
    {
        if ((ret = unw_get_reg(&unw_c, UNW_REG_IP, &ip)) < 0 ||
            (ret = unw_get_reg(&unw_c, UNW_REG_SP, &sp)) < 0)
        {
            break;
        }

        buf[0] = '\0';
        if ((ret = unw_get_proc_name(&unw_c, buf, sizeof(buf), &off)) == 0)
        {
            std::string name = demangle(buf);
            char *debuginfo_path = NULL;

            Dwfl *dwfl = nullptr;
            {
                Dwfl_Callbacks callbacks = {};
                char *debuginfo_path = nullptr;
                callbacks.find_elf = dwfl_linux_proc_find_elf;
                callbacks.find_debuginfo = dwfl_standard_find_debuginfo;
                callbacks.debuginfo_path = &debuginfo_path;
                dwfl = dwfl_begin(&callbacks);
                assert(dwfl);
                int r;
                r = dwfl_linux_proc_report(dwfl, pid);
                assert(!r);
                r = dwfl_report_end(dwfl, nullptr, nullptr);
                assert(!r);
                static_cast<void>(r);
            }
            Dwfl_Module* module = dwfl_addrmodule(dwfl, (uintptr_t)ip);
            if (Dwfl_Line* dwfl_line = dwfl_module_getsrc(module, (uintptr_t)ip)) {
                int nline;
                Dwarf_Addr addr;
                char const* file = dwfl_lineinfo(dwfl_line, &addr, &nline, nullptr, nullptr, nullptr);
                std::string filename(file);
                CallLibraryInfo CallLibraryInfo(ip, name, off, filename, nline);
                v_backtrace.push_back(CallLibraryInfo);
            }
            else
            {
                CallLibraryInfo CallLibraryInfo(ip, name, off);
                v_backtrace.push_back(CallLibraryInfo);
            }
        }
        else
        {
            break;
        }

        if ((ret = unw_get_proc_info(&unw_c, &pi)) < 0)
        {
            break;
        }

        ret = unw_step(&unw_c);
        if (ret < 0)
        {
            unw_get_reg(&unw_c, UNW_REG_IP, &ip);
        }

        if (++n > 64)
        {
            break;
        }
    } while (ret > 0);
    {

        *logger_stream << "----------- backtrace -----------\n";
        *logger_stream << "ERROR CODE : " << error_code << "\n";
        for (std::vector<CallLibraryInfo>::const_iterator i = v_backtrace.begin(); i != v_backtrace.end(); ++i)
        {
            if (i->file_line >= 0)
            {
                *logger_stream << (void *)i->instruction_pointer << " " << i->function_name << " " << i->file_name << " " << i->file_line << "\n";
            }
            else
            {
                *logger_stream << (void *)i->instruction_pointer << " " << i->function_name << "\n";
            }
        }

        *logger_stream << "------------- end -------------\n";
    }
    return 0;
__cleanup:
    _UPT_destroy(ui);
    return 0;
}

static void callback_fork(Event *ev)
{
    process *proc = ev->proc;
    struct library *mainlib = proc_each_library(
        proc->leader, NULL, [](struct process *proc, struct library *lib, void *data) {
            return CBS_STOP_IF(lib->type == LT_LIBTYPE_MAIN);
        },
        NULL);
    struct user_regs_struct regs;
    char pid_maps[255];
    char buffer[1024];
    sprintf(pid_maps, "/proc/%d/maps", proc->pid);
    arch_addr_t baseAddr;
    FILE *maps = fopen(pid_maps, "r");
    if (!maps)
    {
        perror("fopen");
        return;
    }
    while (fgets(buffer, sizeof(buffer), maps))
    {
        if (strstr(buffer, "/shadow") && strstr(buffer, "r-x"))
        {
            baseAddr = (arch_addr_t)strtoull(buffer, NULL, 16);
            break;
        }
    }

    if (fclose(maps))
    {
        perror("fclose");
        return;
    }
    int fd = open(proc->filename, O_RDONLY);
    Elf *elf;
    if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
    {
        return;
    }
    GElf_Shdr symtab_shdr;
    Elf_Scn *symtab_section = [](Elf *elf, GElf_Shdr &symtab_shdr) {
        Elf_Scn *section = nullptr;
        int number = 0;
        while ((section = elf_nextscn(elf, section)) != 0)
        {
            if (gelf_getshdr(section, &symtab_shdr) != 0)
            {
                if (symtab_shdr.sh_type == SHT_SYMTAB)
                {
                    return section;
                }
            }
        }
        return section;
    }(elf, symtab_shdr);
    if (symtab_section != nullptr)
    {
        Elf_Data *data;
        char *name;
        char *stringName;
        data = 0;
        int number = 0;
        if ((data = elf_getdata(symtab_section, data)) == 0 || data->d_size == 0)
        {
            fprintf(stderr, "Section had no data!\n");
            exit(-1);
        }
        unsigned count = symtab_shdr.sh_size / symtab_shdr.sh_entsize;

        for (unsigned i = 0; i < count; ++i)
        {
            GElf_Sym esym;
            GElf_Sym *ret = gelf_getsym(data, i, &esym);

            if (ret == 0)
            {
                break;
            }
            if ((esym.st_value == 0) ||
                (GELF_ST_BIND(esym.st_info) == STB_WEAK) ||
                (GELF_ST_BIND(esym.st_info) == STB_NUM))
                continue;

            name = elf_strptr(elf, symtab_shdr.sh_link, (size_t)esym.st_name);
            if (!name)
            {
                printf("%s\n", elf_errmsg(elf_errno()));
                exit(-1);
            }
            if (strcmp(name, "_process_setErrno") == 0)
            {
                struct breakpoint *prebp;
                arch_addr_t _process_setErrno_addr = baseAddr + data->d_off + ret->st_value;
                if (DICT_FIND_VAL(ev->proc->breakpoints, &_process_setErrno_addr, &prebp) == -1)
                {
                    static struct library_symbol *_process_setErrno_symbol = (library_symbol *)malloc(sizeof(library_symbol));
                    _process_setErrno_symbol->next = NULL;
                    _process_setErrno_symbol->lib = mainlib;
                    _process_setErrno_symbol->proto = NULL;
                    _process_setErrno_symbol->delayed = 0;
                    _process_setErrno_symbol->latent = 0;
                    _process_setErrno_symbol->plt_type = LS_TOPLT_EXEC;
                    _process_setErrno_symbol->own_name = 1;
                    _process_setErrno_symbol->enter_addr = (void *)(uintptr_t)_process_setErrno_addr;
                    _process_setErrno_symbol->name = "_process_setErrno";
                    breakpoint_for_symbol(_process_setErrno_symbol, proc);
                    break;
                }
            }
        }
    }
}
static void callback_breakpoint(Event *ev)
{
    void *brk_addr = ev->e_un.brk_addr;
    int now_callstack_index = ev->proc->callstack_depth - 1;
    if (now_callstack_index > -1)
    {
        struct callstack_element *now_callstack = &ev->proc->callstack[now_callstack_index];
        struct library_symbol *libsym = now_callstack->c_un.libfunc;
        if (libsym != NULL && brk_addr == now_callstack->return_addr)
        {
            if (libsym->name != NULL)
            {
                if (strcmp(libsym->name, "_process_setErrno") == 0)
                {
                    struct process *proc = ev->proc;
                    struct arg_type_info return_type = {
                        .type = ARGTYPE_VOID,
                    };
                    struct param params0;
                    struct param params1;
                    struct arg_type_info argument_0_type;
                    argument_0_type.type = ARGTYPE_POINTER;
                    argument_0_type.own_lens = 0;
                    struct arg_type_info argument_1_type;
                    argument_1_type.type = ARGTYPE_INT;
                    argument_1_type.own_lens = 0;
                    struct prototype _process_setErrno_func;
                    prototype_init(&_process_setErrno_func);
                    _process_setErrno_func.return_info = &return_type;
                    _process_setErrno_func.own_return_info = 0;

                    VECT_INIT(&_process_setErrno_func.params, struct param);
                    param_init_type(&params0, &argument_0_type, 1);
                    if (prototype_push_param(&_process_setErrno_func, &params0) < 0)
                    {
                        return;
                    }
                    param_init_type(&params1, &argument_1_type, 1);
                    if (prototype_push_param(&_process_setErrno_func, &params1) < 0)
                    {
                        return;
                    }

                    struct fetch_context *fetch_context = fetch_arg_init(LT_TOF_FUNCTION, proc, _process_setErrno_func.return_info);
                    if (fetch_context == NULL)
                    {
                        return;
                    }
                    struct value_dict *arguments = (value_dict *)malloc(sizeof(*arguments));
                    if (arguments == NULL)
                    {
                        fetch_arg_done(fetch_context);
                        return;
                    }
                    val_dict_init(arguments);
                    ssize_t params_left = -1;
                    int need_delim = 0;
                    if (fetch_params(LT_TOF_FUNCTION, proc, fetch_context, arguments, &_process_setErrno_func, &params_left) < 0)
                    {
                        val_dict_destroy(arguments);
                        fetch_arg_done(fetch_context);
                    }
                    else
                    {
                        struct value *value = val_dict_get_num(arguments, 1);
                        if (value == NULL)
                        {
                            return;
                        }
                        else
                        {
                            unw_addr_space_t unw_as = unw_create_addr_space(&_UPT_accessors, 0);
                            int result = backtrace(&unw_as, proc->pid, value->u.value);
                            if (result)
                            {
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}
static void exit_handler(void)
{
    if (logger.is_open())
    {
        logger.close();
    }
}
static int handler_load_library(char *libname)
{
    std::string name = std::string(libname);
    if (name.find("libshadow-interpose.so") != std::string::npos)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    InputParser input(argc, argv);
    atexit(exit_handler);
    char *prefix_argv_array[] = {argv[0], "-N", "-f", "-l", "*", "shadow", argv[argc - 1], nullptr};

    std::vector<char *> prefix_argv(prefix_argv_array, prefix_argv_array + (sizeof(prefix_argv_array) / sizeof(void *)));
    char *log = input.getCmdOption(argv, argv + argc, "-o");
    if (log)
    {
        logger.open(log);
        logger_stream = &logger;
    }
    prefix_argv.push_back(TARGET_PROGRAM);
    prefix_argv.push_back(argv[argc - 1]);
    prefix_argv.push_back(nullptr);

    ltrace_init(prefix_argv.size() - 1, prefix_argv.data());

    ltrace_library_add_handler(handler_load_library);
    ltrace_add_callback(callback_breakpoint, EVENT_BREAKPOINT);
    ltrace_add_callback(callback_fork, EVENT_CLONE);
    ltrace_main();

    return 0;
}
