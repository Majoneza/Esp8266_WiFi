#pragma once

template<typename T>
struct is_unsigned {
    static constexpr bool value = T(0) < T(-1);
};

template<>
struct is_unsigned<const char*> {
    static constexpr bool value = false;
};

template<bool b, typename T = void>
struct enable_if {
};

template<typename T>
struct enable_if<true, T> {
    using type = T;
};

template<bool b, typename T = void>
using enable_if_t = typename enable_if<b, T>::type;

template<typename T1, typename T2>
struct is_same {
    static constexpr bool value = false;
};

template<typename T>
struct is_same<T, T> {
    static constexpr bool value = true;
};

template<typename T, typename... Ts>
struct is_same_pack {
    static constexpr bool value = is_same_pack<T, Ts...>::value;
};

template<typename T1, typename T2, typename... Ts>
struct is_same_pack<T1, T2, Ts...> {
    static constexpr bool value = false;
};

template<typename T, typename... Ts>
struct is_same_pack<T, T, Ts...> {
    static constexpr bool value = is_same_pack<T, Ts...>::value;
};

template<typename T>
struct is_same_pack<T, T> {
    static constexpr bool value = true;
};
