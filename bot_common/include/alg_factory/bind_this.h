/*
 ************************************************************************\

                              C O P Y R I G H T

   Copyright © 2024 IRMV lab, Shanghai Jiao Tong University, China.
                         All Rights Reserved.

   Licensed under the Creative Commons Attribution-NonCommercial 4.0
   International License (CC BY-NC 4.0).
   You are free to use, copy, modify, and distribute this software and its
   documentation for educational, research, and other non-commercial purposes,
   provided that appropriate credit is given to the original author(s) and
   copyright holder(s).

   For commercial use or licensing inquiries, please contact:
   IRMV lab, Shanghai Jiao Tong University at: https://irmv.sjtu.edu.cn/

                              D I S C L A I M E R

   IN NO EVENT SHALL TRINITY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR
   DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
   BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGES.

   TRINITY COLLEGE DUBLIN DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE. THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY
   COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
   ENHANCEMENTS, OR MODIFICATIONS.

   The authors may be contacted at the following e-mail addresses:

           YX.E.Z yixuanzhou@sjtu.edu.cn

   Further information about the IRMV and its projects can be found at the ISG web site :

          https://irmv.sjtu.edu.cn/

 \*************************************************************************

 */

#ifndef RRTS_BIND_THIS_H
#define RRTS_BIND_THIS_H
namespace std
{
template<int>  // begin with 0 here!
struct placeholder_template
{};

template<int N>
struct is_placeholder<placeholder_template<N> >
    : integral_constant<int, N+1>  // the one is important
{};
}  // end of namespace std


template<int...>
struct int_sequence
{};

template<int N, int... Is>
struct make_int_sequence
    : make_int_sequence<N-1, N-1, Is...>
{};

template<int... Is>
struct make_int_sequence<0, Is...>
    : int_sequence<Is...>
{};

template<class R, class U, class... Args, int... Is>
auto bind_this_sub(R (U::*p)(Args...), U * pp, int_sequence<Is...>)
-> decltype(std::bind(p, pp, std::placeholder_template<Is>{}...))
{
  return std::bind(p, pp, std::placeholder_template<Is>{}...);
}

// binds a member function only for the this pointer using std::bind
template<class R, class U, class... Args>
auto bind_this(R (U::*p)(Args...), U * pp)
-> decltype(bind_this_sub(p, pp, make_int_sequence< sizeof...(Args) >{}))
{
  return bind_this_sub(p, pp, make_int_sequence< sizeof...(Args) >{});
}

// utility
template<class R, class U, class... Args, int... Is>
auto bind_this_sub(R (U::*p)(Args...) const, U * pp, int_sequence<Is...>)
-> decltype(std::bind(p, pp, std::placeholder_template<Is>{}...))
{
  return std::bind(p, pp, std::placeholder_template<Is>{}...);
}

// binds a member function only for the this pointer using std::bind
template<class R, class U, class... Args>
auto bind_this(R (U::*p)(Args...) const, U * pp)
-> decltype(bind_this_sub(p, pp, make_int_sequence< sizeof...(Args) >{}))
{
  return bind_this_sub(p, pp, make_int_sequence< sizeof...(Args) >{});
}
#endif //RRTS_BIND_THIS_H
