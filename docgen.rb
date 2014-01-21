#!/usr/bin/env ruby
require 'cgi'

out = `clang++ -fno-color-diagnostics -Xclang -ast-dump -fsyntax-only -std=c++1y -x c++ #{ARGV.join(" ")}`
puts '<!--'
#puts CGI::escapeHTML(out)
#warn out
puts '-->'
warn "done with clang"
INDENT = "  "

class Decl 
    def print_comments(comments, ind)
        if comments == nil || comments.length == 0
            return
        end
#         comments.map! do |para|
#             text = para
#             
#             idx = 0
#             while text[idx..-1].length > 80
#                 idx2 = text[idx..(idx+80)].rindex(/\s/)
#                 if idx == nil
#                     break;
#                 end
#                 idx += idx2
#                 #p idx
#                 text[idx] = "\n"
#                 #idx += 1
#             end
#             text
#         end
        
        out = comments.join("\n\n<p>")
        #out = out.gsub(/^/, INDENT*(ind+1))
        
        
        puts indent(ind, '<div class="comment subelement non-namespace">')
        puts out
        puts indent(ind, '</div>')
    end
    
    def html(str)
        CGI::escapeHTML(str)
    end
    
    def indent(n, str)
        INDENT*n + str
    end
    
    def fmt_name_and_loc(obj)
        name = html(obj[:name])
        pos  = html("#{obj[:filename]}:#{obj[:lineno]}:#{obj[:colno]}")
        "#{name} <span class=\"location\">#{pos}</span>"
    end
end

class FunctionDecl < Decl
    def initialize(obj)
        @obj = obj
        @overloads = [obj]
        @overload_names = { obj[:name] => true }
    end
    
    def add_overload(obj)
        if @overload_names[obj[:name]] != nil
            return
        end
        @overloads.push(obj)
        @overload_names[obj[:name]] = true
    end
    
    def print(ind)
        
        puts indent ind, "<div class=\"non-namespace #{longname()} subelement declaration\">"
        puts indent ind+1, html("#{name()} #{@obj[:symbol]}")
        
        for overload in @overloads
            puts indent ind+1, '<div class="overload subelement">'
            puts indent ind+2, fmt_name_and_loc(overload)
            puts indent ind+1, '</div>'
        end
        for overload in @overloads
            puts indent(ind+1, '')
            print_comments(overload[:comment], ind+1)
        end
        
        puts indent ind, '</div>'
    end
    
    def name
        "func"
    end
    
    def longname
        "function"
    end
end

class MethodDecl < FunctionDecl
    def initialize(obj)
        super(obj)
    end
    
    def name
        "meth"
    end
    
    def longname
        "method"
    end
end

class StructDecl < Decl
    def initialize(obj)
        @obj = obj
        @fields = []
        @field_names = {}
        @ctors = []
        @ctor_names = {}
        @meths = {}
        @dtors = []
        @dtor_names = {}
    end
    
    def print(ind)
        puts indent ind, '<div class="non-namespace misc declaration subelement">'
        puts indent ind+1, fmt_name_and_loc(@obj)
        for field in @fields
            puts indent ind+1, '<div class="non-namespace field subelement declaration">'
            puts indent ind+2, "field " + fmt_name_and_loc(field)
            print_comments(field[:comment], ind+2)
            puts indent ind+1, '</div>'
        end
        for ctor in @ctors
            puts indent ind+1, '<div class="non-namespace constructor subelement declaration">'
            puts indent ind+2, "ctor " + fmt_name_and_loc(ctor)
            print_comments(ctor[:comment], ind+2)
            puts indent ind+1, '</div>'
        end
        for key in @meths.keys.sort
            @meths[key].print(ind+1)
        end
        for dtor in @dtors
            puts indent ind+1, '<div class="non-namespace destructor subelement declaration">'
            puts indent ind+2, "dtor " + fmt_name_and_loc(dtor)
            print_comments(dtor[:comment], ind+2)
            puts indent ind+1, '</div>'
        end
        puts indent ind, '</div>'
    end
    
    def add(obj)
        case obj[:type]
        when 'CXXConstructorDecl'
            if @ctor_names[obj[:name]] != nil
                return
            end
            @ctors.push(obj)
            @ctor_names[obj[:name]] = true
        when 'CXXDestructorDecl'
            if @dtor_names[obj[:name]] != nil
                return
            end
            @dtors.push(obj)
            @dtor_names[obj[:name]] = true
        when 'CXXMethodDecl'
            sym = obj[:symbol]
            meth = @meths[sym]
            if meth != nil
                meth.add_overload(obj)
                return
            end
            meth = MethodDecl::new(obj)
            @meths[sym] = meth
        when 'FieldDecl'
            name = obj[:name]
            if @field_names[name] != nil
                return
            end
            @fields.push(obj)
            @field_names[name] = true
        end
    end
end

class NamespaceDecl < Decl
    def initialize(obj)
        @obj = obj
        @childns = {}
        @vars = {}
        @structs = {}
        @funcs  = {}
    end
    
    def add(obj, stack)
        if stack.length > 0
            top = stack.first
            case top[:type]
            when 'NamespaceDecl'
                name = top[:name]
                ns = @childns[name]
                #warn "ns: #{name}"
                ns.add(obj, stack[1..-1])
            when 'CXXRecordDecl'
                name = top[:name]
                @structs[name].add(obj)
            end
            return
        end
        
        case obj[:type]
        when 'NamespaceDecl'
            name = obj[:name]
            if @childns[name] != nil
                return
            end
            ns = NamespaceDecl::new(obj)
            @childns[name] = ns
            #warn "Created new namespace #{name} in #{@name}"
        when 'FunctionDecl'
            sym = obj[:symbol]
            func = @funcs[sym]
            if func != nil
                func.add_overload(obj)
                return
            end
            func = FunctionDecl::new(obj)
            @funcs[sym] = func
        when 'CXXRecordDecl'
            name = obj[:name]
            struct = @structs[name]
            if struct != nil
                return
            end
            @structs[name] = StructDecl::new(obj)
        when 'VarDecl'
            name = obj[:name]
            decl = @vars[:name]
            if decl != nil
                return
            end
            @vars[name] = obj
        end
    end
    
    def print(ind)
        puts indent ind, '<div class="namespace declaration subelement">'
        puts indent ind+1, '<span>' + html("namespace #{@obj[:name]}") + '</span>'
        print_comments(@obj[:comment], ind+2)
        for key in @childns.keys.sort
            @childns[key].print(ind+1)
        end
        puts indent ind+1, '<div class="non-namespace">'
        for key in @vars.keys.sort
            var = @vars[key]
            puts indent ind+2, '<div class="non-namespace var subelement declaration">'
            puts indent ind+3, html("var #{var[:name]}")
            print_comments(var[:comment], ind+3)
            puts indent ind+2, '</div>'
        end
        for key in @structs.keys.sort
            @structs[key].print(ind+2)
        end
        for key in @funcs.keys.sort
            @funcs[key].print(ind+2)
        end
        puts indent ind+1, '</div>'
        puts indent ind, '</div>'
    end
end

class DocGen
    def initialize
        @stack = [{
                :indent   => -1,
                :children => [],
                :type => "root"
                }]
        @file   = ""
        @lineno = 0
        @colno  = 0
        @root   = NamespaceDecl::new({ :name => "::" })
        @stack2 = [@root]
        @dict   = {
            "NamespaceDecl" => :namespace
        }
    end
    
    def print(ind)
        @root.print(ind)
    end
    
    def popstack
        item = @stack.pop()
        if item[:type] == "FunctionDecl"
            ns = []
            for i in (@stack.length-1).downto(1)
                if @stack[i][:type] == "NamespaceDecl"
                    ns.push(@stack[i][:name])
                end
            end
            item[:ns] = ns.reverse
            #@funcs.push(item)
        end
    end
    
    def process(text)
        for line in text.split("\n")
            if line.match(/[^>]invalid sloc>/)
                next
            end
            #p line
            indent   = line.match(/(^.*?)[a-zA-Z]/)[1].length
            cmd      = line.match(/([A-Za-z]+)/)[1]
            loc      = line.match(/<([a-z0-9:.\/\\_\-+$~]+)(?:, ([a-z0-9:.]+)>)/)
            #p line
            
            
            if loc != nil
                locparts = loc[1].split(":")
                case locparts[0]
                when "line"
                    @lineno = locparts[1].to_i
                    @colno  = locparts[2].to_i
                when "col"
                    @colno  = locparts[1].to_i
                else
                    file = locparts[0]
                    #if file != nil
                        @file = file
                        @lineno = locparts[1]
                        @colno  = locparts[2]
                    #end
                    
                end
            end
            
            obj = {}
            obj[:type]   = cmd
            obj[:indent] = indent
            obj[:filename] = @file
            obj[:lineno] = @lineno
            obj[:colno] = @colno
            dopush = false
            doprocess = false
            
            while indent <= @stack.last[:indent]
                #puts "pop"
                popstack
            end
            
            case cmd
            when "FunctionDecl", "CXXRecordDecl", "CXXMethodDecl", "NamespaceDecl",
                "CXXDestructorDecl", "CXXConstructorDecl",
                "FieldDecl", "VarDecl"
                
                #"ClassTemplateDecl"
                
                #p "functiondecl"
                name = line.match(/.*?>(.*)/)[1].strip
                if cmd == "CXXRecordDecl" and (!name.end_with?(" definition") and !name.end_with?(" definition invalid"))
                    next
                end
                
                if (cmd == 'CXXDestructorDecl' or cmd == 'CXXConstructorDecl') and
                        (name.match(/-unevaluated 0x[a-f0-9]+$/))
                    next
                end
        #         if cmd == "CXXRecordDecl" and name.end_with?(" definition")
        #             next
        #         elsif cmd == "CXXRecordDecl"
        #             obj[:indent] -= 1
        #         end
                #puts "#{cmd} #{name} on #{file}:#{lineno}:#{colno}"
                obj[:name] = name
                obj[:symbol] = name.match(/([^ ]*)/)[1]
                rest = name.match(/ (.*)/)
                if rest != nil
                    obj[:rest] = {
                        :rest => rest[1],
                        :filename => obj[:filename],
                        :lineno => obj[:lineno],
                        :colno => obj[:colno]
                    }
                else
                    obj[:rest] = {}
                end
                
                obj[:rests] = [obj[:rest]]
                #obj[:comment] = []
                @stack.last[:children].push(obj)
                obj[:children] = []
                dopush = true
                doprocess = true
            when "FullComment"
                @stack.last[:comment] = []
            when "ParagraphComment"
                @stack.last[:comment].push("")
                #doprocess = true
            when "TextComment"
                comment = line.match(/"(.*)"/)[1]
                if @stack.last[:comment].last != ""
                    comment = comment.sub(/^ /, "\n")
                end
                @stack.last[:comment].last << CGI::escapeHTML(comment)
            when 'HTMLStartTagComment'
                tag = line.match(/"(.*?)"/)[1]
                @stack.last[:comment].last << "<#{tag}>"
            when 'HTMLEndTagComment'
                tag = line.match(/"(.*?)"/)[1]
                @stack.last[:comment].last << "</#{tag}>"
            end
            
            if doprocess
                @root.add(obj, @stack[1..-1])
            end
            
            if dopush
                @stack.push(obj)
            end
            
        end

        while @stack.length > 1
            popstack
        end
        
        #print_items(@stack[0][:children], 2)
    end
end








puts '
<script src="jquery-2.0.3.js"></script>
<script>
    onload = function() {
        //$(".namespace > .children > .non-namespace").hide()
        $(".namespace > span").on("click", function(item) {
            var target = item.target;
            var showing = target.classList.contains("showing")
            console.log(showing)
            if (showing) {
                $("> .non-namespace", target.parentElement).hide()
                target.classList.remove("showing")
            } else {
                $("> .non-namespace", target.parentElement).show()
                target.classList.add("showing")
            }
        });
    }
</script>
<style>
    .doc {
        font-family: monospace;
    }
                                                                                                                                                                                                                                            
    .subelement {
        margin-left: 2em;
    }
    
    .declaration {
        margin-top: 1em;
    }
    
    .comment {
        color: purple;
        margin-left: 2em;
        max-width: 45em;
        margin-top: 0.5em;
    }
    
    .children {
        
    }
                                                                                                                                                                                                                                                                
    .showing {
        font-weight: bold;
    }
    
    .namespace > span {
        cursor: pointer;
    }
    
    .namespace > span:hover {
        text-decoration: underline;
    }
    
    .namespace > .non-namespace {
        display: none;
    }
    
    .location {
        color: gray;
    }
</style>
<div class="doc">
'

docgen = DocGen::new
docgen.process(out)
docgen.print(2)

puts '
</div>
'




#     for item in out
#         if item[:file].start_with?("/")
#             next
#         end
#         ourindent = indent
#         print " " * indent
#         case item[:type]
#         when "NamespaceDecl"
#             print '<div class="namespace"><span>'
#             print "namespace #{item[:name]}"
#             print "</span>"
#             print "\n"
#         when "FunctionDecl"
#             #print "func #{item[:name]} in #{item[:file]}:#{item[:lineno]}:#{item[:colno]}\n"
#             item[:rests].uniq!{|rest| rest[:rest]}
#             if item[:rests].length > 1
#                 print "func #{item[:symbol]}\n"
#                 #ourindent += 2
#                 for rest in item[:rests]
#                     print " " * (ourindent+2) + "#{rest[:rest]} in #{rest[:file]}:#{rest[:lineno]}:#{rest[:colno]}\n"
#                 end
#             else
#                 print "func #{item[:name]} in #{item[:file]}:#{item[:lineno]}:#{item[:colno]}\n"
#             end
#         when "CXXMethodDecl"
#             item[:rests].uniq!{|rest| rest[:rest]}
#             if item[:rests].length > 1
#                 print "meth #{item[:symbol]}\n"
#                 #ourindent += 2
#                 for rest in item[:rests]
#                     print " " * (ourindent+2) + "#{rest[:rest]} in #{rest[:file]}:#{rest[:lineno]}:#{rest[:colno]}\n"
#                 end
#             else
#                 print "meth #{item[:name]} in #{item[:file]}:#{item[:lineno]}:#{item[:colno]}\n"
#             end
#                 
#         when "CXXConstructorDecl"
#             #print "constructor #{item[:name]} in #{item[:file]}:#{item[:lineno]}:#{item[:colno]}\n"
#             item[:rests].uniq!{|rest| rest[:rest]}
#             if item[:rests].length > 1
#                 print "constructor #{item[:symbol]}\n"
#                 #ourindent += 2
#                 for rest in item[:rests]
#                     print " " * (ourindent+2) + "#{rest[:rest]} in #{rest[:file]}:#{rest[:lineno]}:#{rest[:colno]}\n"
#                 end
#             else
#                 print "constructor #{item[:name]} in #{item[:file]}:#{item[:lineno]}:#{item[:colno]}\n"
#             end
#         when "FieldDecl"
#             print "field #{item[:name]} in #{item[:file]}:#{item[:lineno]}:#{item[:colno]}\n"
#         when "CXXDestructorDecl"
#             print "destructor #{item[:name]} in #{item[:file]}:#{item[:lineno]}:#{item[:colno]}\n"
#         else
#             print "#{item[:name]} in #{item[:file]}:#{item[:lineno]}:#{item[:colno]}\n"
#         end
#         
#         if item[:comment].length != 0
#             print_comments(item[:comment], ourindent+2)
#         end
#         print '<div class="comment">'
#         print_items(item[:children], ourindent+2)
#         print "</div>"
#         print "</div>"
#     end