//
// Created by just do it on 2023/12/8.
//

#ifndef TINYRPC_TRAIT_H
#define TINYRPC_TRAIT_H
#include <tuple>
#include <type_traits>

namespace tinyRPC {

    template<typename Method>
    struct service_method_traits {};

    template<typename R, typename S, typename... Args>
    struct service_method_traits<R(S::*)(Args...)> {
        using result_type = R;
        using service_type = S;
        static constexpr size_t args_count = sizeof...(Args);
        using args_type = std::tuple<typename std::decay<Args>::type...>;

        template<size_t I>
        struct typeof_arg_n {
            using type = std::remove_reference_t<
                    decltype(std::get<I - 1>(std::declval<args_type>()))>;
        };
    };

    template<typename M, size_t Idx>
    using method_arg_type = typename service_method_traits<M>::template typeof_arg_n<Idx>::type;

    template<typename M>
    using query_type = std::remove_const_t<std::remove_pointer_t<method_arg_type<M, 2>>>;

    template<typename M>
    using response_type = std::remove_pointer_t<method_arg_type<M, 3>>;

    template<typename M>
    using service_type = typename service_method_traits<M>::service_type;

}

#endif //TINYRPC_TRAIT_H
