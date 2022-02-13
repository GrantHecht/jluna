// 
// Copyright 2022 Clemens Cords
// Created on 31.01.22 by clem (mail@clemens-cords.com)
//

namespace jluna
{
    template<Unboxable T>
    T Proxy::operator[](size_t i)
    {
        static jl_function_t* getindex = jl_get_function(jl_base_module, "getindex");
        return unbox<T>(jluna::safe_call(getindex, _content->value(), box(i + 1)));
    }

    template<Unboxable T, std::enable_if_t<not std::is_same_v<T, std::string>, bool>>
    Proxy::operator T() const
    {
        return unbox<T>(_content->value());
    }

    template<typename T, std::enable_if_t<std::is_base_of_v<Proxy, T>, bool>>
    Proxy::operator T()
    {
        return T(_content->value(), _content->_owner, (jl_sym_t*) _content->symbol());
    }

    template<typename T, std::enable_if_t<std::is_base_of_v<Proxy, T>, bool>>
    T Proxy::as()
    {
        return T(_content->value(), _content->_owner, (jl_sym_t*) _content->symbol());
    }

    template<Boxable T>
    Proxy & Proxy::operator=(T value)
    {
        return this->operator=(box(value));
    }

    template<Unboxable T>
    T Proxy::operator[](const std::string& field)
    {
        jl_sym_t* symbol = jl_symbol(field.c_str());
        return unbox<T>(_content.get()->get_field(symbol));
    }

    template<Boxable... Args_t>
    Proxy Proxy::call(Args_t&&... args)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return jluna");
        static jl_function_t* invoke = jl_get_function(jluna_module, "invoke");

        return Proxy(jluna::call(invoke, _content->value(), box(args)...), nullptr);
    }

    template<Boxable... Args_t>
    Proxy Proxy::safe_call(Args_t&&... args)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return jluna");
        static jl_function_t* invoke = jl_get_function(jluna_module, "invoke");

        return Proxy(jluna::safe_call(invoke, _content->value(), box(args)...), nullptr);
    }

    template<Boxable... Args_t>
    Proxy Proxy::operator()(Args_t&&... args)
    {
        return this->safe_call(args...);
    }
}