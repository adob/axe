#!/usr/bin/env ruby


SRCDIR = "src"
BINDIR = "bin"
INCPATH = "-Isrc"

CC      = "g++-10"

ROOT   = File::dirname($0)


module Base 
    CCFLAGS = ["-std=c++1z", "-pthread", "-fnon-call-exceptions", "-g" ,
               "-Wall", "-Wextra", "-Wno-sign-compare", "-Wno-deprecated"]
    LFLAGS  = ["-lrt", "-ldwarf", "-lelf"]
    
    OBJDIR = "obj"
    DEPDIR = "obj"
    SUFFIX = ""
    #OFILE  = File::basename(TARGET, ".cpp")
end

module Release 
    include Base
    
    CCFLAGS = Base::CCFLAGS + [#"-flto",
                               "-O3", "-mtune=native"]
    LFLAGS  = Base::LFLAGS + [#"-flto", 
                              "-fwhole-program", "-O3", "-mtune=native"]
    OBJDIR  = Base::OBJDIR + "/release"
    DEPDIR  = Base::DEPDIR + "/release"
end

module Debug 
    include Base
    
    CCFLAGS = Base::CCFLAGS + ["-fsanitize=address"]

    OBJDIR  = Base::OBJDIR + "/debug"
    DEPDIR  = Base::DEPDIR + "/debug"
    SUFFIX  = "+debug"
    #OFILE = OFILE + "+debug"
end

require 'shellwords'
require 'fileutils'
require 'pathname'
require 'set'
require 'getoptlong'

@processed_files = Set.new
@objs = []
@last_mtime = Time::new(0)

def parse_makefile_rules(text)
    rules = text.gsub(/^.*?:/, '').gsub(/\\\n/, '').split()
    return rules
end

def shell(*args)
    #args = args.map{|item| Shellwords.escape(item) }
    cmd = args.join(" ")
    puts cmd
    result = `#{cmd}`
    if $?.exitstatus != 0
        exit(1)
    end
    return result
end

def mtime(file)
    if not File::exists?(file)
        return Time::new(0)
    end
    return File::mtime(file)
end

def strip_suffix(file) 
    return file.sub(/\.[^\/.]+\Z/, '')
end

def cpp2obj(file)
    file = file.sub(SRCDIR+'/', '')
    file = OBJDIR + "/" + strip_suffix(file) + '.o'
    return file
end

def build_cpp(cppfile)
    objfile = cpp2obj(cppfile)
    objdir = File::dirname(objfile)
    if not File::exists?(objdir)
        FileUtils::mkdir_p(objdir)
    end
    
    shell(CC, *CCFLAGS, *INCPATH, "-o"+objfile, "-c", cppfile)
    @last_mtime = Time::now()
    
end

def process_file(file)
    if file.end_with?(".h")
        process_h(file)
    else
        print "uknown file type"
    end
end

def process_h(hfile)
    hfile = Pathname.new(hfile).cleanpath.to_s
    
    if @processed_files.include?(hfile)
        return
    else
        @processed_files.add(hfile)
    end
    
#     puts "  >> process hfile #{hfile}"
    file = strip_suffix(hfile)
    
    if File::exists?(file + ".cpp") 
        process_cpp(file + ".cpp")
    end
    
#     if File::directory?(file)
#         files = Dir::glob(file + "/*.cpp")
#         for cppfile in files 
#             process_cpp(cppfile)
#         end
#         
#     end
    
    if File::basename(hfile) == "PKG.h"
        files = Dir::glob(File::dirname(hfile) + "/*.cpp")
        for cppfile in files
            process_cpp(cppfile)
        end
    end
end

def build_deps(cppfile)
    file = strip_suffix(cppfile.sub(SRCDIR+"/", ''))
    depfile = DEPDIR + "/" + file + ".dep"
    print("DEPS:\t")
    depfile_content = shell(CC, "-MM", "-MT"+cppfile, *CCFLAGS, *INCPATH, cppfile)
    IO.write(depfile, depfile_content)
end

def get_makefile_rules(cppfile)
    file = strip_suffix(cppfile.sub(SRCDIR+"/", ''))
    depfile = DEPDIR + "/" + file + ".dep"
    
    if not File::exists?(depfile) or mtime(cppfile) >= mtime(depfile)
        dir = File::dirname(depfile)
        if not File::exists?(dir)
            FileUtils::mkdir_p(dir)
        end
        build_deps(cppfile)
    end
    
    rules = parse_makefile_rules(IO::read(depfile))
end

def process_cpp(cppfile)
    cppfile = Pathname.new(cppfile).cleanpath.to_s
    
    if @processed_files.include?(cppfile)
        return
    else
        @processed_files.add(cppfile)
    end
    
    file = strip_suffix(cppfile.sub(SRCDIR+"/", ''))
    objfile = "#{OBJDIR}/#{file}.o"
    #cppfile = "#{SRCDIR}/#{file}"
    
    rules = get_makefile_rules(cppfile)
    
    buildtime = mtime(cpp2obj(cppfile))
    if buildtime > @last_mtime
        @last_mtime = buildtime
    end
    need_update = false
    need_deps = false
    
    #puts "cppfile #{cppfile}"
    #puts "buildtime #{buildtime}"
    for rule in rules
        if rule != cppfile
            process_file(rule)
        end
        currmtime = mtime(rule)
        #puts "mtime #{rule} #{currmtime}"
        if currmtime > buildtime 
            puts "need to build #{cppfile} due to #{rule}"
            need_update = true
            if not need_deps and rule != cppfile
                need_deps = true
            end
        end
        #puts "rules #{rule}"
    end
    
    @objs << cpp2obj(cppfile)
    if need_update
        if need_deps and buildtime != Time::new(0)
            build_deps(cppfile)
        end
        build_cpp(cppfile)
        return true
    end
    return false
end

def build(target, buildmode)
    case buildmode
    when :release
        include Release
    when :debug
        include Debug
    else
        raise "unkown buildmode"
    end
    
    process_cpp(target)
    
    FileUtils::mkdir_p(BINDIR)
    ofile = BINDIR+"/"+File::basename(target, ".cpp") + SUFFIX
    if @last_mtime >= mtime(ofile)
        shell(CC, *CCFLAGS, "-o"+ofile, *@objs, *LFLAGS)
    end
    
    return ofile
    
end


opts = GetoptLong.new(
    ['--release', '-r', GetoptLong::NO_ARGUMENT],
    ['--debug', '-d', GetoptLong::NO_ARGUMENT]
)

buildmode = :release

opts.each do |opt, arg|
    case opt
    when "--release"
        buildmode = :release
    when "--debug"
        buildmode = :debug
    end
end



if ARGV.length == 0
    warn "Missing argument"
    exit 1
end



case ARGV.shift
when "build"
    if ARGV.length == 0
        warn "no file"
        exit 1
    end
    file = ARGV.shift
    target = File::absolute_path(file).sub(File::absolute_path(ROOT) + "/", '')
    
    if ROOT != "."
        Dir::chdir(ROOT)
        #puts "CD #{ROOT}"
    end
    
    build(target, buildmode)
when "run"
    if ARGV.length == 0
        warn "no file"
        exit 1
    end
    file = ARGV.shift
    target = File::absolute_path(file).sub(File::absolute_path(ROOT) + "/", '')
    
    oldwd = nil
    if ROOT != "."
        oldwd = Dir::getwd
        Dir::chdir(ROOT)
        
    end
    
    bin = File::absolute_path(build(target, buildmode))
    
    if oldwd != nil
        Dir::chdir(oldwd)
    end
    
    exec bin
    
else
    warn "uknown command"
    exit 1
end








