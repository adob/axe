namespace axe {
    template <typename T, size N>
    size len(T (&)[N]) {
        return N;
    }
    
    template <typename T>
    size len(T const& t) {
        return t.size();
    }
    
    template <typename ArgFirst, typename ArgSecond, typename... ArgRest>
    size len(ArgFirst a1, ArgSecond a2, ArgRest... args) {
        return len(a1) + len(a2, args...);
    }
    
}