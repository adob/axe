namespace axe {
    template <typename T, typename U>
    void memcopy(T& t, U const& u) {
        static_assert(sizeof(t) == sizeof(u), "size mismatch");
        memcpy(&t, &u, sizeof(t));
    }
}