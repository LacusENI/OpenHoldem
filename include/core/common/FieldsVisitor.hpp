#pragma once

/*
 * 由字段名生成 `visit` 方法的预处理器工具。
 *
 * 用法：
 *   struct S {
 *       int a;
 *       bool b;
 *       POKER_FIELDS_VISITOR(a, b)
 *   };
 *
 * 会生成 const / 非 const 两个 `visit(V&&)`：
 *   v("a", a);
 *   v("b", b);
 *
 * 限制与说明：
 *   - 最多 8 个字段（需要更多时补 POKER_PP_FE_9... 并扩充 POKER_PP_NARG）。
 *   - 空字段事件可用 POKER_FIELDS_VISITOR()；参数用 [[maybe_unused]]，
 *     避免在宏体里塞 (void)v 这类额外噪音。
 *   - 不使用 __VA_OPT__ 递归：同名宏在自身展开期间会被预处理器“涂蓝”，
 *     直接递归不会展开；固定元数是可靠做法。
 *   - 成员函数不破坏聚合体，`S{1, true}` 仍然可用。
 */

#define POKER_PP_CAT(a, b) POKER_PP_CAT_I(a, b)
#define POKER_PP_CAT_I(a, b) a##b

#define POKER_PP_NARG(...) POKER_PP_NARG_I(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)
#define POKER_PP_NARG_I(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N

#define POKER_PP_FE_1(M, x) M(x)
#define POKER_PP_FE_2(M, x, ...) M(x) POKER_PP_FE_1(M, __VA_ARGS__)
#define POKER_PP_FE_3(M, x, ...) M(x) POKER_PP_FE_2(M, __VA_ARGS__)
#define POKER_PP_FE_4(M, x, ...) M(x) POKER_PP_FE_3(M, __VA_ARGS__)
#define POKER_PP_FE_5(M, x, ...) M(x) POKER_PP_FE_4(M, __VA_ARGS__)
#define POKER_PP_FE_6(M, x, ...) M(x) POKER_PP_FE_5(M, __VA_ARGS__)
#define POKER_PP_FE_7(M, x, ...) M(x) POKER_PP_FE_6(M, __VA_ARGS__)
#define POKER_PP_FE_8(M, x, ...) M(x) POKER_PP_FE_7(M, __VA_ARGS__)

#define POKER_PP_FOR_EACH(M, ...) \
    POKER_PP_CAT(POKER_PP_FE_, POKER_PP_NARG(__VA_ARGS__))(M, __VA_ARGS__)

#define POKER_VISIT_FIELD(field) v(#field, field);

#define POKER_FIELDS_VISITOR(...)                                      \
    template <class V>                                                 \
    void visit([[maybe_unused]] V&& v) {                               \
        __VA_OPT__(POKER_PP_FOR_EACH(POKER_VISIT_FIELD, __VA_ARGS__))  \
    }                                                                  \
    template <class V>                                                 \
    void visit([[maybe_unused]] V&& v) const {                         \
        __VA_OPT__(POKER_PP_FOR_EACH(POKER_VISIT_FIELD, __VA_ARGS__))  \
    }
