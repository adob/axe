#pragma once

template <typename T>
struct val {
    T value;
    
    val(T t) : value(t) {};
    
    operator T () const  { return value; } 
    T operator = (T other) { return value = other;}
};