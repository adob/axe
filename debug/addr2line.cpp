/*-
 * Copyright (c) 2009 Kai Wang. All rights reserved.
 * Portions copyright 2013-2014 Aleksandr Dobkin. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#include <sys/param.h>
#include <dwarf.h>
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <getopt.h>
#include <libdwarf.h>
//#include <libelftc.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <cxxabi.h>
#include <errno.h>


#include <axe/print.h>
#include <axe/fmt/fmt.h>
#import "debug.h"
#import "addr2line.h"

//#include "_elftc.h"

extern "C" {
    char*
    __cxa_demangle(const char* __mangled_name, char* __output_buffer,
                   size_t* __length, int* __status);
    
    std::type_info*
    __cxa_current_exception_type();
    
}

namespace axe {
namespace {

int dwarf_attrval_addr(Dwarf_Die die, Dwarf_Half attr, Dwarf_Addr *addrp, Dwarf_Error *err) {
    Dwarf_Attribute at;
    int ret;
    if ((ret = dwarf_attr(die, attr, &at, err)))
        return ret;
    
    Dwarf_Half form;
    if ((ret = dwarf_whatform(at, &form, err))) 
        return ret;
    //print "got form", form;
    
    switch (form) {
    case DW_FORM_ref1:
    case DW_FORM_ref2:
    case DW_FORM_ref4:
    case DW_FORM_ref8:
    case DW_FORM_ref_udata:
        print "got ref udata";
    case DW_FORM_addr: {
        if ((ret = dwarf_formaddr(at, addrp, err)))
            return ret;
        //print "got addr", *addrp;
        return DW_DLV_OK;
        
    } break;
    case DW_FORM_data8: {
        if ((ret = dwarf_formudata(at, addrp, err)))
            return ret;
        return DW_DLV_OK;
    }
    default:
        print "got other type", form;
        return DW_DLV_ERROR;
    }   
    return DW_DLV_ERROR;
}

int dwarf_attrval_string(Dwarf_Die die, Dwarf_Half attr, const char **strp, Dwarf_Error *err) {
    Dwarf_Attribute at;
    int ret;
    if ((ret = dwarf_attr(die, attr, &at, err)))
        return ret;
    
    Dwarf_Half form;
    if ((ret = dwarf_whatform(at, &form, err))) 
        return ret;
    
    switch (form) {
//     case DW_FORM_string:
//         break;
    case DW_FORM_strp: {
        if (dwarf_formstring(at, const_cast<char **>(strp), err))
            print "error", dwarf_errmsg(*err);
        return DW_DLV_OK;
    } break;
    default:
        printf("unexpected form %x\n", form);
        return DW_DLV_ERROR;
    }
    return DW_DLV_ERROR;
    
}
    
void search_func(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Addr addr,
                 const char **rlt_func, errorparam err) {
    Dwarf_Die ret_die, spec_die;
    Dwarf_Error de;
    Dwarf_Half tag;
    Dwarf_Addr lopc, hipc;
    Dwarf_Off ref;
    Dwarf_Attribute sub_at, spec_at;
    char *func0;
    int ret;
    
    if (*rlt_func != NULL)
        return;
    
    if (dwarf_tag(die, &tag, &de)) {
        //err = dwarf_errmsg(de);
        warnx("dwarf_tag: %s", dwarf_errmsg(de));
        goto cont_search;
    }
    if (tag == DW_TAG_subprogram) {
        //print "PC", addr;
        if (dwarf_attrval_addr(die, DW_AT_low_pc, &lopc, &de) ||
            dwarf_attrval_addr(die, DW_AT_high_pc, &hipc, &de))
            goto cont_search;
        
        if (addr < lopc || addr >= lopc+hipc)
            goto cont_search;
        
        /* Found it! */
        //print "Found it";
        
        *rlt_func = "??";
        ret = dwarf_attr(die, DW_AT_name, &sub_at, &de);
        if (ret == DW_DLV_ERROR) {
            return;
        }
        if (ret == DW_DLV_OK) {
            if (dwarf_formstring(sub_at, &func0, &de)) {
                *rlt_func = "??";
            } else {
                *rlt_func = func0;
            }
            return;
        }
        
        /*
         * If DW_AT_name is not present, but DW_AT_specification is
         * present, then probably the actual name is in the DIE
         * referenced by DW_AT_specification.
         */
        if (dwarf_attr(die, DW_AT_specification, &spec_at, &de))
            return;
        if (dwarf_global_formref(spec_at, &ref, &de))
            return;
        if (dwarf_offdie(dbg, ref, &spec_die, &de))
            return;
        if (dwarf_attrval_string(spec_die, DW_AT_linkage_name, rlt_func, &de))
            *rlt_func = "??";
        //print "XXXX", *rlt_func;
        return;
    }
    
    cont_search:
    
    /* Search children. */
    ret = dwarf_child(die, &ret_die, &de);
    if (ret == DW_DLV_ERROR) {
        err = dwarf_errmsg(de);
        return;
    } else if (ret == DW_DLV_OK)
        search_func(dbg, ret_die, addr, rlt_func, err);
    
    /* Search sibling. */
    ret = dwarf_siblingof(dbg, die, &ret_die, &de);
    if (ret == DW_DLV_ERROR) {
        err = dwarf_errmsg(de);
        return;
    } else if (ret == DW_DLV_OK)
        search_func(dbg, ret_die, addr, rlt_func, err);
}


void translate(Dwarf_Debug dbg, Dwarf_Unsigned addr, debug::LineInfo& info, errorparam err) {
    Dwarf_Die die;
    Dwarf_Line *lbuf;
    Dwarf_Error de;
    Dwarf_Half tag;
    Dwarf_Unsigned lopc, hipc, lineno, plineno, pplineno;
    Dwarf_Signed lcount;
    Dwarf_Addr lineaddr, plineaddr, pplineaddr;
    const char *funcname, *file, *pfile;
    char *file0;
    int i, ret;
    bool func = true;
    Dwarf_Unsigned  next_cu_header;
    
    lineno = 0;
    file = "??";
    
    while ((ret = dwarf_next_cu_header(dbg, NULL, NULL, NULL, NULL, &next_cu_header, &de)) ==  DW_DLV_OK) {
        //print "dwarf_next_cu_header";
        die = NULL;
        while (dwarf_siblingof(dbg, die, &die, &de) == DW_DLV_OK) {
                if (dwarf_tag(die, &tag, &de) != DW_DLV_OK) {
                        warnx("dwarf_tag failed: %s",
                            dwarf_errmsg(de));
                        goto out;
                }
                /* XXX: What about DW_TAG_partial_unit? */
                if (tag == DW_TAG_compile_unit)
                        break;
        }
        if (die == NULL) {
                warnx("could not find DW_TAG_compile_unit die");
                goto out;
        }

        Dwarf_Attribute at;
        if (dwarf_attr(die, DW_AT_ranges, &at, &de) == 0)  {
            Dwarf_Off offset;
            if (dwarf_global_formref(at, &offset, &de))
                print "error", dwarf_errmsg(de);
            
            Dwarf_Ranges *ranges;
            Dwarf_Signed nr_ranges;
            Dwarf_Unsigned bytecount;
            
            if (dwarf_get_ranges_a(dbg, offset, die, &ranges, &nr_ranges, &bytecount, &de))
                print "error", dwarf_errmsg(de);
            
            for (int i = 0; i < nr_ranges; i++) {
                //print "range", ranges[i].dwr_addr1, ranges[i].dwr_addr2;
                
                if (ranges[i].dwr_addr1 <= addr && addr < ranges[i].dwr_addr2) {
                    //print "TRUE!!!";
                    goto range_ok;
                }
            }
            //print "skipping";
            continue;
        }
        
        range_ok:
        
        
        if (!dwarf_attrval_addr(die, DW_AT_low_pc,  &lopc, &de) &&
            !dwarf_attrval_addr(die, DW_AT_high_pc, &hipc, &de)) {
                /*
                    * Check if the address falls into the PC range of
                    * this CU.
                    */
                if (addr < lopc || addr >= lopc+hipc)
                        continue;
        }
        if (dwarf_srclines(die, &lbuf, &lcount, &de) != DW_DLV_OK) {
                warnx("dwarf_srclines: %s", dwarf_errmsg(de));
                //goto out;
                continue;
        }
        //print "START";
        plineaddr = ~0ULL;
        pplineaddr = plineaddr;
        plineno = 0;
        pfile = "??";
        for (i = 0; i < lcount; i++) {
            if (dwarf_lineaddr(lbuf[i], &lineaddr, &de)) {
                warnx("dwarf_lineaddr: %s", dwarf_errmsg(de));
                    goto out;
            }
            if (dwarf_lineno(lbuf[i], &lineno, &de)) {
                warnx("dwarf_lineno: %s", dwarf_errmsg(de));
                goto out;
            }
            if (dwarf_linesrc(lbuf[i], &file0, &de)) {
                warnx("dwarf_linesrc: %s", dwarf_errmsg(de));
            } else
                    file = file0;
            //fmt::printf("LINEADDR %x %x %x\n", addr, lineaddr, plineaddr);
            //print lineaddr;
            
            Dwarf_Bool lineend;
            if (dwarf_lineendsequence(lbuf[i], &lineend, &de))
                warnx("dwarf_linesrc: %s", dwarf_errmsg(de));
            
            
            
            if (addr == lineaddr) {
                //print "!111", file, pfile;
                if (plineaddr != ~0ULL) {
                    lineno = plineno;
                }
                goto out;
            } else if (addr < lineaddr && addr > plineaddr) {
                if (pplineaddr != ~0ULL) {
                    lineno = pplineno;
                }
                lineno = plineno;
                //fmt::printf("!!!2 %x %s %s %x %x\n", addr, file, pfile, plineaddr, lineaddr);
                file = pfile;
                
                goto out;
            }
            
            pplineaddr = plineaddr;
            if (lineend)
                plineaddr = ~0ULL;
            else
                plineaddr = lineaddr;
            
            pplineno = plineno;
            plineno = lineno;
            pfile = file;
        }
    }
    
    file = "??";
    lineno = -1;

out:
    funcname = NULL;
    if (ret == DW_DLV_OK && func)
        search_func(dbg, die, addr, &funcname, err);
    
    if (funcname == nullptr) {
        info.symbolname = "??";
        info.funcname   = "??";
    } else {
        info.symbolname = funcname;
        char *demangled = __cxa_demangle(funcname, nullptr, 0,  &ret);
        if (demangled)
            info.funcname   = demangled;
        else
            info.funcname   = info.symbolname;
        
        free(demangled);
    }

    info.filepath   = file;
    info.lineno     = lineno;
    
        
}

} // namespace

namespace debug {

LineInfo addr2line(void* addr, errorparam err) {
    LineInfo info;
    int fd = open("/proc/self/exe", O_RDONLY);
    if (fd < 0) {
        return err=errno, info;
    }
    
    //Elf *e;
    Dwarf_Debug dbg;
    Dwarf_Error de;
    
    if (dwarf_init(fd, DW_DLC_READ, nullptr, nullptr, &dbg, &de)) {
        print "error", dwarf_errmsg(de);
        goto done;
    }
        
    
//     if (dwarf_get_elf(dbg, &e, &de) != DW_DLV_OK) {
//         return err=dwarf_errmsg(de), info;
//     }
    
    translate(dbg, (uintptr) addr, info, err);
    
    if (dwarf_finish(dbg, &de)) {
        print "error", dwarf_errmsg(de);
    }
    
    //elf_end(e);
    close(fd);
    
    done:
    return info;
}
} // namespace debug
} // namespace axe
