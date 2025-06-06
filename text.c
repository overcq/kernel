/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         text
* ©overcq                on ‟Gentoo Linux 13.0” “x86_64”             2015‒4‒28 *
*******************************************************************************/
_export
B
E_text_Z_c_T_alpha( C c
){  return ( c >= '0' && c <= '9' )
    || ( c >= 'A' && c <= 'Z' )
    || ( c >= 'a' && c <= 'z' );
}
_export
B
E_text_Z_c_T_quote( C c
){  return c < 43 || c > 122
    || c == ';'
    || c == '<'
    || c == '>'
    || c == '?'
    || c == '\\'
    || c == '`';
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
C
E_text_Z_c_I_lower( C c
){  if( c >= 'A'
    && c <= 'Z'
    )
        c += 32;
    return c;
}
_export
C
E_text_Z_c_I_upper( C c
){  if( c >= 'a'
    && c <= 'z'
    )
        c -= 32;
    return c;
}
//==============================================================================
// Szkic standardu:
// Adres wynikowy procedur obiektów “s”:
// •na początku znalezionych albo
// •po wszystkich danych.
//------------------------------------------------------------------------------
_export
Pc
E_text_Z_s0_I_lower( Pc s
){  while( *s )
    {   *s = E_text_Z_c_I_lower( *s );
        s++;
    }
    return s;
}
_export
Pc
E_text_Z_s0_I_upper( Pc s
){  while( *s )
    {   *s = E_text_Z_c_I_upper( *s );
        s++;
    }
    return s;
}
_export
Pc
E_text_Z_s_I_lower( Pc s
, Pc s_end
){  while( s != s_end )
    {   *s = E_text_Z_c_I_lower( *s );
        s++;
    }
    return s;
}
_export
Pc
E_text_Z_s_I_upper( Pc s
, Pc s_end
){  while( s != s_end )
    {   *s = E_text_Z_c_I_upper( *s );
        s++;
    }
    return s;
}
_export
B
E_text_Z_s0_T_eq_s0( Pc s
, Pc t
){  while( *s
    && *t
    && *s == *t
    )
    {   s++;
        t++;
    }
    return !*s && !*t;
}
_export
B
E_text_Z_s0_T_eq_case_s0( Pc s
, Pc t
){  while( *s
    && *t
    && E_text_Z_c_I_upper( *s ) == E_text_Z_c_I_upper( *t )
    )
    {   s++;
        t++;
    }
    return !*s && !*t;
}
_export
N
E_text_Z_s0_T_starts_s0( Pc s
, Pc t
){  N l = 0;
    while( *s
    && *t
    && *s == *t
    )
    {   s++;
        t++;
        l++;
    }
    return !*t ? l : 0;
}
_export
N
E_text_Z_s0_T_starts_case_s0( Pc s
, Pc t
){  N l = 0;
    while( *s
    && *t
    && E_text_Z_c_I_upper( *s ) == E_text_Z_c_I_upper( *t )
    )
    {   s++;
        t++;
        l++;
    }
    return !*t ? l : 0;
}
_export
B
E_text_Z_s0_T_ends_s0( Pc s
, Pc t
){  Pc s_end = E_text_Z_s0_R_end(s);
    N t_l = E_text_Z_s0_R_l(t);
    if( (N)s_end - (N)s < t_l )
        return no;
    s = s_end - t_l;
    while( *t
    && *s == *t
    )
    {   s++;
        t++;
    }
    return !*t;
}
_export
B
E_text_Z_s_T_ends_s0( Pc s
, Pc s_end
, Pc t
){  N t_l = E_text_Z_s0_R_l(t);
    if( (N)s_end - (N)s < t_l )
        return no;
    s = s_end - t_l;
    while( *t
    && *s == *t
    )
    {   s++;
        t++;
    }
    return !*t;
}
_export
S
E_text_Z_s0_I_cmp_s0( Pc s
, Pc t
){  while( *s
    && *t
    && *s == *t
    )
    {   s++;
        t++;
    }
    return *s - *t;
}
_export
S
E_text_Z_sl_T_cmp( Pc s
, Pc t
, N l
){  while( l
    && *s == *t
    )
    {   s++;
        t++;
        l--;
    }
    return l ? *s - *t : 0;
}
_export
N
E_text_Z_s_T_starts_s0( Pc s
, Pc s_end
, Pc t
){  N l = 0;
    while( s != s_end
    && *t
    && *s == *t
    )
    {   s++;
        t++;
        l++;
    }
    return !*t ? l : 0;
}
_export
N
E_text_Z_s_T_starts_case_s0( Pc s
, Pc s_end
, Pc t
){  N l = 0;
    while( s != s_end
    && *t
    && *s == *t
    )
    {   s++;
        t++;
        l++;
    }
    return !*t ? l : 0;
}
_export
B
E_text_Z_s_T_eq_s0( Pc s
, Pc s_end
, Pc t
){  while( s != s_end
    && *t
    && *s == *t
    )
    {   s++;
        t++;
    }
    return s == s_end
    && !*t;
}
_export
B
E_text_Z_s_T_eq_case_s0( Pc s
, Pc s_end
, Pc t
){  while( s != s_end
    && *t
    && E_text_Z_c_I_upper( *s ) == E_text_Z_c_I_upper( *t )
    )
    {   s++;
        t++;
    }
    return s == s_end
    && !*t;
}
_export
B
E_text_Z_s_T_eq_s( Pc s
, Pc s_end
, Pc t
, Pc t_end
){  if( s_end - s != t_end - t )
        return no;
    while( s != s_end
    && *s == *t
    )
    {   s++;
        t++;
    }
    return s == s_end;
}
_export
B
E_text_Z_sl_T_eq( Pc s
, Pc t
, N l
){  while( l
    && *s == *t
    )
    {   s++;
        t++;
        l--;
    }
    return !l;
}
_export
B
E_text_Z_s_T_eq_case_s( Pc s
, Pc s_end
, Pc t
, Pc t_end
){  if( s_end - s != t_end - t )
        return no;
    while( s != s_end
    && E_text_Z_c_I_upper( *s ) == E_text_Z_c_I_upper( *t )
    )
    {   s++;
        t++;
    }
    return s == s_end;
}
_export
B
E_text_Z_sl_T_eq_case( Pc s
, Pc t
, N l
){  while( l
    && E_text_Z_c_I_upper( *s ) == E_text_Z_c_I_upper( *t )
    )
    {   s++;
        t++;
        l--;
    }
    return !l;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Less or equal end.
_export
Pc
E_text_Z_s0_R_end_0_le( Pc s
, N l
){  while( --l
    && *s
    )
        s++;
    return s;
}
_export
B
E_text_Z_s0_T_l_0_le( Pc s
, N l
){  while( --l
    && *s
    )
        s++;
    return !*s;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
Pc
E_text_Z_s_R_search_0( Pc s
, Pc s_end
){  while( s != s_end )
    {   if( !*s )
            break;
        s++;
    }
    return s;
}
_export
Pc
E_text_Z_sl_R_search_0( Pc s
, N l
){  while(l)
    {   if( !*s )
            break;
        s++;
        l--;
    }
    return s;
}
_export
Pc
E_text_Z_s_R_search_c( Pc s
, Pc s_end
, C c
){  while( s != s_end )
    {   if( *s == c )
            break;
        s++;
    }
    return s;
}
_export
Pc
E_text_Z_s_R_search_c_( Pc s
, C c
){  while( *s != c )
        s++;
    return s;
}
_export
Pc
E_text_Z_s0_R_search_c( Pc s
, C c
){  while( *s )
    {   if( *s == c )
            break;
        s++;
    }
    return s;
}
_export
Pc
E_text_Z_s_R_search_last_c( Pc s
, Pc s_end
, C c
){  while( s_end != s )
    {   if( *--s_end == c )
            break;
    }
    return s_end;
}
_export
Pc
E_text_Z_s0_R_search_last_c_( Pc s_end
, C c
){  while( *s_end != c )
        s_end--;
    return s_end;
}
_export
Pc
E_text_Z_sl_R_search_last_c( Pc s_end
, N l
, C c
){  while(l)
    {   if( *--s_end == c )
            break;
        l--;
    }
    return s_end;
}
//------------------------------------------------------------------------------
_export
Pc
E_text_Z_s_R_search_s( Pc s
, Pc s_end
, Pc t
, Pc t_end
){  if( t == t_end )
        return s;
    if( t_end - t <= s_end - s )
    {   Pc r = s, t_ = t;
        while( s != s_end )
        {   if( *s == *t_ )
            {   if( t_ == t )
                    r = s;
                if( ++t_ == t_end )
                    return r;
            }else
            {   if( s >= s_end - ( t_end - t ))
                    break;
                if( t_ != t )
                    t_ = t;
            }
            s++;
        }
    }
    return s_end;
}
_export
Pc
E_text_Z_s_R_search_s0( Pc s
, Pc s_end
, Pc t
){  if( !*t )
        return s;
    Pc r = s, t_ = t;
    while( s != s_end )
    {   if( *s == *t_ )
        {   if( t_ == t )
                r = s;
            if( !*++t_ )
                return r;
        }else if( t_ != t )
            t_ = t;
        s++;
    }
    return s;
}
_export
Pc
E_text_Z_s0_R_search_s0( Pc s
, Pc t
){  if( !*t )
        return s;
    Pc r = s, t_ = t;
    while( *s )
    {   if( *s == *t_ )
        {   if( t_ == t )
                r = s;
            if( !*++t_ )
                return r;
        }else if( t_ != t )
            t_ = t;
        s++;
    }
    return s;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
N
E_text_Z_s_Z_10_N_n( Pc s
, Pc s_end
, Pc *ret_s
){  N n = 0;
    while( s != s_end )
    {   n *= 10;
        if( *s >= '0' && *s <= '9' )
            n += *s - '0';
        else
            break;
        s++;
    }
    *ret_s = s;
    return n;
}
_export
N
E_text_Z_s0_Z_10_N_n( Pc s
, Pc *ret_s
){  N n = 0;
    while( *s )
    {   n *= 10;
        if( *s >= '0' && *s <= '9' )
            n += *s - '0';
        else
            break;
        s++;
    }
    *ret_s = s;
    return n;
}
//------------------------------------------------------------------------------
_export
N
E_text_Z_s_N_n( Pc s
, Pc s_end
, Pc *ret_s
, N base
){  N n = 0, n_prev = 0;
    while( s != s_end )
    {   n *= base;
        if( *s >= '0' && *s <= '9' )
            n += *s - '0';
        else if( *s >= 'A' && *s < 'A' + base - 10 )
            n += 10 + *s - 'A';
        else if( *s >= 'a' && *s < 'a' + base - 10 )
            n += 10 + *s - 'a';
        else
            break;
        if( n < n_prev )
        {   n = n_prev;
            break;
        }
        n_prev = n;
        s++;
    }
    *ret_s = s;
    return n;
}
_export
N
E_text_Z_s0_N_n( Pc s
, Pc *ret_s
, N base
){  N n = 0, n_prev = 0;
    while( *s )
    {   n *= base;
        if( *s >= '0' && *s <= '9' )
            n += *s - '0';
        else if( *s >= 'A' && *s < 'A' + base - 10 )
            n += 10 + *s - 'A';
        else if( *s >= 'a' && *s < 'a' + base - 10 )
            n += 10 + *s - 'a';
        else
            break;
        if( n < n_prev )
        {   n = n_prev;
            break;
        }
        n_prev = n;
        s++;
    }
    *ret_s = s;
    return n;
}
//------------------------------------------------------------------------------
_export
S
E_text_Z_s_N_sn( Pc s
, Pc s_end
, Pc *ret_s
, N base
){  B sign = no;
    if( *s == '-' )
    {   sign = yes;
        s++;
    }
    S n = 0, n_prev = 0;
    while( s != s_end )
    {   n *= base;
        if( *s >= '0' && *s <= '9' )
            n += *s - '0';
        else if( *s >= 'A' && *s < 'A' + base - 10 )
            n += 10 + *s - 'A';
        else if( *s >= 'a' && *s < 'a' + base - 10 )
            n += 10 + *s - 'a';
        else
            break;
        if( n < n_prev
        || !~n
        )
        {   n = n_prev;
            break;
        }
        n_prev = n;
        s++;
    }
    if(sign)
        n = -n;
    *ret_s = s;
    return n;
}
_export
S
E_text_Z_s0_N_sn( Pc s
, Pc *ret_s
, N base
){  B sign = no;
    if( *s == '-' )
    {   sign = yes;
        s++;
    }
    S n = 0, n_prev = 0;
    while( *s )
    {   n *= base;
        if( *s >= '0' && *s <= '9' )
            n += *s - '0';
        else if( *s >= 'A' && *s < 'A' + base - 10 )
            n += 10 + *s - 'A';
        else if( *s >= 'a' && *s < 'a' + base - 10 )
            n += 10 + *s - 'a';
        else
            break;
        if( n < n_prev
        || !~n
        )
        {   n = n_prev;
            break;
        }
        n_prev = n;
        s++;
    }
    if(sign)
        n = -n;
    *ret_s = s;
    return n;
}
//------------------------------------------------------------------------------
/*_export
F
E_text_Z_s_N_f( Pc s
, Pc s_end
, Pc *ret_s
, N base
){  B sign = no;
    if( *s == '-' )
    {   sign = yes;
        s++;
    }
    F n = 0, n_prev = 0;
    N floating_point = ~0;
    while( s != s_end )
    {   n *= base;
        if( *s >= '0' && *s <= '9' )
            n += *s - '0';
        else if( *s >= 'A' && *s < 'A' + base - 10 )
            n += 10 + *s - 'A';
        else if( *s >= 'a' && *s < 'a' + base - 10 )
            n += 10 + *s - 'a';
        else if( *s == '.'
        && !~floating_point
        )
            floating_point = 0;
        else
        {   if( ~floating_point )
                floating_point--;
            break;
        }
        if( isless( n, n_prev ))
        {   n = n_prev;
            if( ~floating_point )
                floating_point--;
            break;
        }
        n_prev = n;
        if( ~floating_point )
            floating_point++;
        s++;
    }
    if( ~floating_point )
        n /= pow( base, floating_point );
    if(sign)
        n = -n;
    *ret_s = s;
    return n;
}
_export
F
E_text_Z_s0_N_f( Pc s
, Pc *ret_s
, N base
){  B sign = no;
    if( *s == '-' )
    {   sign = yes;
        s++;
    }
    F n = 0, n_prev = 0;
    N floating_point = ~0;
    while( *s )
    {   n *= base;
        if( *s >= '0' && *s <= '9' )
            n += *s - '0';
        else if( *s >= 'A' && *s < 'A' + base - 10 )
            n += 10 + *s - 'A';
        else if( *s >= 'a' && *s < 'a' + base - 10 )
            n += 10 + *s - 'a';
        else if( *s == '.'
        && !~floating_point
        )
            floating_point = 0;
        else
        {   if( ~floating_point )
                floating_point--;
            break;
        }
        if( isless( n, n_prev ))
        {   n = n_prev;
            if( ~floating_point )
                floating_point--;
            break;
        }
        n_prev = n;
        if( ~floating_point )
            floating_point++;
        s++;
    }
    if( ~floating_point )
        n /= pow( base, floating_point );
    if(sign)
        n = -n;
    *ret_s = s;
    return n;
}
//------------------------------------------------------------------------------
_export
struct E_math_Z_bignum *
E_text_Z_s_N_bignum( Pc s
, Pc s_end
, Pc *ret_s
, N base
, N min_prec
){  B sign = no;
    if( *s == '-' )
    {   sign = yes;
        s++;
    }
    struct E_math_Z_bignum *n = E_math_Q_bignum_M(), *n_prev = E_math_Q_bignum_M(), *add = E_math_Q_bignum_M();
    N floating_point = ~0;
    while( s != s_end )
    {   E_math_Q_bignum_I_multiply_n( n, base );
        if( *s >= '0' && *s <= '9' )
        {   add->digits[0] = *s - '0';
            E_math_Q_bignum_I_add( n, add );
        }else if( *s >= 'A' && *s < 'A' + base - 10 )
        {   add->digits[0] = 10 + *s - 'A';
            E_math_Q_bignum_I_add( n, add );
        }else if( *s >= 'a' && *s < 'a' + base - 10 )
        {   add->digits[0] = 10 + *s - 'a';
            E_math_Q_bignum_I_add( n, add );
        }else if( *s == '.'
        && !~floating_point
        )
            floating_point = 0;
        else
            break;
        if( E_math_Q_bignum_I_compare( n, n_prev ) == -2 )
        {   J_swap( struct E_math_Z_bignum *, n, n_prev );
            break;
        }
        E_math_Q_bignum_W( n_prev );
        n_prev = E_math_Q_bignum_M_copy(n);
        if( ~floating_point )
            floating_point++;
        s++;
    }
    E_math_Q_bignum_W(add);
    E_math_Q_bignum_W( n_prev );
    if( ~floating_point )
        while( --floating_point )
        {   struct E_math_Z_bignum *tmp = E_math_Q_bignum_M();
            tmp->digits[0] = base;
            E_math_Q_bignum_I_divide_modulo_min_prec( n, tmp, min_prec + floating_point - 1 );
            E_math_Q_bignum_W(tmp);
        }
    if(sign)
        E_math_Q_bignum_I_negate(n);
    *ret_s = s;
    return n;
}
_export
struct E_math_Z_bignum *
E_text_Z_s0_N_bignum( Pc s
, Pc *ret_s
, N base
, N min_prec
){  J_assert( base >= 2 && base <= 36 );
    B sign = no;
    if( *s == '-' )
    {   sign = yes;
        s++;
    }
    struct E_math_Z_bignum *n = E_math_Q_bignum_M(), *n_prev = E_math_Q_bignum_M(), *add = E_math_Q_bignum_M();
    N floating_point = ~0;
    while( *s )
    {   E_math_Q_bignum_I_multiply_n( n, base );
        if( *s >= '0' && *s <= '9' )
        {   add->digits[0] = *s - '0';
            E_math_Q_bignum_I_add( n, add );
        }else if( *s >= 'A' && *s < 'A' + base - 10 )
        {   add->digits[0] = 10 + *s - 'A';
            E_math_Q_bignum_I_add( n, add );
        }else if( *s >= 'a' && *s < 'a' + base - 10 )
        {   add->digits[0] = 10 + *s - 'a';
            E_math_Q_bignum_I_add( n, add );
        }else if( *s == '.'
        && !~floating_point
        )
            floating_point = 0;
        else
            break;
        if( E_math_Q_bignum_I_compare( n, n_prev ) == -2 )
        {   J_swap( struct E_math_Z_bignum *, n, n_prev );
            break;
        }
        E_math_Q_bignum_W( n_prev );
        n_prev = E_math_Q_bignum_M_copy(n);
        if( ~floating_point )
            floating_point++;
        s++;
    }
    E_math_Q_bignum_W(add);
    E_math_Q_bignum_W( n_prev );
    if( ~floating_point )
        while( --floating_point )
        {   struct E_math_Z_bignum *tmp = E_math_Q_bignum_M();
            tmp->digits[0] = base;
            E_math_Q_bignum_I_divide_modulo_min_prec( n, tmp, min_prec + floating_point - 1 );
            E_math_Q_bignum_W(tmp);
        }
    if(sign)
        E_math_Q_bignum_I_negate(n);
    *ret_s = s;
    return n;
}*/
//------------------------------------------------------------------------------
_export
struct E_datetime_Z
E_text_Z_s_N_datetime( Pc s
, Pc s_end
, Pc *ret_s
){  struct E_datetime_Z datetime = {0};
    datetime.year = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( *ret_s != s_end )
        return datetime;
    s = *ret_s + 1;
    datetime.month = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( datetime.month < 1
    || datetime.month > 12
    )
    {   *ret_s = s;
        datetime.month = 0;
        return datetime;
    }
    if( *ret_s != s_end )
        return datetime;
    s = *ret_s + 1;
    datetime.day = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( datetime.day < 1
    || datetime.day > 31
    )
    {   *ret_s = s;
        datetime.month = 0;
        return datetime;
    }else
        switch(datetime.month)
        { case 2:
          {     N days = datetime.year % 100 == 0 && datetime.year % 400 != 0 ? 29 : 28;
                if( datetime.day > days )
                {   *ret_s = s;
                    datetime.day = 0;
                    return datetime;
                }
                break;
          }
          case 4:
          case 6:
          case 9:
          case 11:
                if( datetime.day == 31 )
                {   *ret_s = s;
                    datetime.day = 0;
                    return datetime;
                }
                break;
        }
    if( *ret_s != s_end )
        return datetime;
    s = *ret_s + 1;
    N v = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( v > 23 )
    {   *ret_s = s;
        return datetime;
    }
    datetime.millisecond = 1000 * 60 * 60 * v;
    if( *ret_s != s_end )
        return datetime;
    s = *ret_s + 1;
    v = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( v > 59 )
    {   *ret_s = s;
        return datetime;
    }
    datetime.millisecond += 1000 * 60 * v;
    if( *ret_s != s_end )
        return datetime;
    s = *ret_s + 1;
    v = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( v > 59 )
    {   *ret_s = s;
        return datetime;
    }
    datetime.millisecond += 1000 * v;
    if( *ret_s != s_end )
        return datetime;
    s = *ret_s + 1;
    v = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( v > 999 )
    {   *ret_s = s;
        return datetime;
    }
    datetime.millisecond += v;
    return datetime;
}
_export
struct E_datetime_Z
E_text_Z_s0_N_datetime( Pc s
, Pc *ret_s
){  struct E_datetime_Z datetime = {0};
    datetime.year = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( !**ret_s )
        return datetime;
    s = *ret_s + 1;
    datetime.month = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( datetime.month < 1
    || datetime.month > 12
    )
    {   *ret_s = s;
        datetime.month = 0;
        return datetime;
    }
    if( !**ret_s )
        return datetime;
    s = *ret_s + 1;
    datetime.day = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( datetime.day < 1
    || datetime.day > 31
    )
    {   *ret_s = s;
        datetime.month = 0;
        return datetime;
    }else
        switch(datetime.month)
        { case 2:
          {     N days = datetime.year % 100 == 0 && datetime.year % 400 != 0 ? 29 : 28;
                if( datetime.day > days )
                {   *ret_s = s;
                    datetime.day = 0;
                    return datetime;
                }
                break;
          }
          case 4:
          case 6:
          case 9:
          case 11:
                if( datetime.day == 31 )
                {   *ret_s = s;
                    datetime.day = 0;
                    return datetime;
                }
                break;
        }
    if( !**ret_s )
        return datetime;
    s = *ret_s + 1;
    N v = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( v > 23 )
    {   *ret_s = s;
        return datetime;
    }
    datetime.millisecond = 1000 * 60 * 60 * v;
    if( !**ret_s )
        return datetime;
    s = *ret_s + 1;
    v = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( v > 59 )
    {   *ret_s = s;
        return datetime;
    }
    datetime.millisecond += 1000 * 60 * v;
    if( !**ret_s )
        return datetime;
    s = *ret_s + 1;
    v = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( v > 59 )
    {   *ret_s = s;
        return datetime;
    }
    datetime.millisecond += 1000 * v;
    if( !**ret_s )
        return datetime;
    s = *ret_s + 1;
    v = E_text_Z_s0_Z_10_N_n( s, ret_s );
    if( v > 999 )
    {   *ret_s = s;
        return datetime;
    }
    datetime.millisecond += v;
    return datetime;
}
//------------------------------------------------------------------------------
_export
Pc
E_text_Z_n_N_s( Pc s_end
, N n
, N l
, N base
){  n = E_simple_Z_n_I_mod_i2( n, l * 8 );
    do
    {   N k = n % base;
        *--s_end = k < 10
        ? '0' + k
        : 'A' + k - 10;
    }while( n /= base );
    return s_end;
}
_export
N
E_text_Z_n_N_s_G( N n
, N l
, N base
){  N i = 0;
    n = E_simple_Z_n_I_mod_i2( n, l * 8 );
    do
    {   i++;
    }while( n /= base );
    return i;
}
_export
Pc
E_text_Z_n_N_bcd( Pc s_end
, N n
, N l
, N size
){  n = E_simple_Z_n_I_mod_i2( n, l * 8 );
    B odd = size % 2;
    if(odd)
        *--s_end = 0;
    do
    {   N k = n % 10;
        if( !odd )
            *--s_end = k << 4;
        else
            *s_end |= k;
        odd = !odd;
    }while( n /= 10 );
    return s_end;
}
_export
N
E_text_Z_n_N_bcd_G( N n
, N l
){  N i = 0;
    n = E_simple_Z_n_I_mod_i2( n, l * 8 );
    do
    {   i++;
    }while( n /= 10 );
    return i / 2 + ( i % 2 ? 1 : 0 );
}
//------------------------------------------------------------------------------
_export
Pc
E_text_Z_ns_N_s( Pc s_end
, S sn
, N l
, N base
){  s_end = E_text_Z_n_N_s( s_end, J_abs(sn), l, base );
    if( sn < 0 )
        *--s_end = '-';
    return s_end;
}
_export
N
E_text_Z_ns_N_s_G( S sn
, N l
, N base
){  return E_text_Z_n_N_s_G( J_abs(sn), l, base ) + ( sn < 0 ? 1 : 0 );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
Pc
E_text_Z_s_P_c_fill( Pc s
, Pc s_end
, C c
){  while( s != s_end )
        *s++ = c;
    return s;
}
_export
void
E_text_Z_s0_P_lower( Pc s
){  while( *s )
    {   *s = E_text_Z_c_I_lower( *s );
        s++;
    }
}
_export
void
E_text_Z_s_P_lower( Pc s
, Pc s_end
){  while( s != s_end )
    {   *s = E_text_Z_c_I_lower( *s );
        s++;
    }
}
_export
void
E_text_Z_s0_P_upper( Pc s
){  while( *s )
    {   *s = E_text_Z_c_I_upper( *s );
        s++;
    }
}
_export
void
E_text_Z_s_P_upper( Pc s
, Pc s_end
){  while( s != s_end )
    {   *s = E_text_Z_c_I_upper( *s );
        s++;
    }
}
//------------------------------------------------------------------------------
_export
Pc
E_text_Z_s_P_copy_s( Pc s
, Pc t
, Pc t_end
){  while( t != t_end )
    {   *s = *t;
        s++;
        t++;
    }
    return s;
}
_export
Pc
E_text_Z_s_P_copy_s_0( Pc s
, Pc t
, Pc t_end
){  return E_text_Z_s_P_0( (Pc)E_text_Z_s_P_copy_s( s, t, t_end ));
}
//------------------------------------------------------------------------------
_export
Pc
E_text_Z_s_P_copy_sl_0( Pc s
, Pc t
, N l
){  E_mem_Q_blk_I_copy( s, t, l );
    return E_text_Z_s_P_0( s + l );
}
//------------------------------------------------------------------------------
_export
Pc
E_text_Z_s_P_copy_s0( Pc s
, Pc t
){  while( *t )
    {   *s = *t;
        s++;
        t++;
    }
    return s;
}
_export
Pc
E_text_Z_s_P_copy_s0_0( Pc s
, Pc t
){  return E_text_Z_s_P_0( E_text_Z_s_P_copy_s0( s, t ));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
void
E_text_Z_sl_P_rev( Pc s
, N l
){  N i = 0;
    N l_ = l / 2;
    while( l_-- )
    {   J_swap( C, s[i], s[ l - 1 - i ] );
        i++;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
Pc
E_text_Z_s0_M_duplicate( Pc s
){  N l = E_text_Z_s0_R_l_0(s);
    Pc t = M(l);
    if(t)
        E_text_Z_s_P_copy_s0_0( t, s );
    return t;
}
_export
Pc
E_text_Z_sl_M_duplicate( Pc s
, N l
){  Pc t = M(l);
    if(t)
        E_mem_Q_blk_I_copy( t, s, l );
    return t;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
N
E_text_Z_s_I_append_s( Pc *s
, Pc t
, Pc t_end
){  Pc p;
    if( !( p = E_mem_Q_blk_I_append( s, t_end - t )))
        return 0;
    E_text_Z_s_P_copy_s( p, t, t_end );
    return t_end - t;
}
_export
N
E_text_Z_s_I_s_append_0( Pc *s
, Pc t
, Pc t_end
){  Pc p;
    if( !( p = E_mem_Q_blk_I_append( s, t_end - t + 1 )))
        return 0;
    E_text_Z_s_P_copy_s_0( p, t, t_end );
    return t_end - t + 1;
}
_export
N
E_text_Z_s_I_append_s0( Pc *s
, Pc t
){  N l = E_text_Z_s0_R_l(t);
    Pc p;
    if( !( p = E_mem_Q_blk_I_append( s, l )))
        return 0;
    E_mem_Q_blk_I_copy( p, t, l );
    return l;
}
_export
N
E_text_Z_s_I_append_s0_0( Pc *s
, Pc t
){  N l = E_text_Z_s0_R_l(t) + 1;
    Pc p;
    if( !( p = E_mem_Q_blk_I_append( s, l )))
        return 0;
    E_text_Z_s_P_copy_sl_0( p, t, l );
    return l;
}
//------------------------------------------------------------------------------
_export
Pc
E_text_Z_s_I_prepend_s( Pc *s
, Pc t
, Pc t_end
){  Pc p;
    if( !( p = E_mem_Q_blk_I_prepend( s, t_end - t )))
        return 0;
    E_text_Z_s_P_copy_s( *s, t, t_end );
    return *s + ( t_end - t );
}
_export
Pc
E_text_Z_s_I_prepend_s0( Pc *s
, Pc t
){  N l = E_text_Z_s0_R_l(t);
    Pc p;
    if( !( p = E_mem_Q_blk_I_prepend( s, l )))
        return 0;
    E_mem_Q_blk_I_copy( *s, t, l );
    return p;
}
//------------------------------------------------------------------------------
_export
N
E_text_Z_s0_I_clear( Pc *s
){  N l = E_text_Z_s0_R_l( *s );
    if( !l )
        return 0;
    if( !E_mem_Q_blk_I_remove( s, 0, l ))
        return ~0;
    return 0;
}
_export
Pc
E_text_Z_s0_I_append_c( Pc *s
, C c
){  Pc p;
    if( !( p = E_mem_Q_blk_I_append( s, 1 )))
        return 0;
    *( p - 1 ) = c;
    *p = '\0';
    return p - 1;
}
_export
Pc
E_text_Z_s0_I_append_s0( Pc *s
, Pc t
){  N l = E_text_Z_s0_R_l(t);
    if( !l )
        return *s + l;
    Pc p;
    if( !( p = E_mem_Q_blk_I_append( s, l )))
        return 0;
    E_text_Z_s_P_copy_sl_0( p - 1, t, l );
    return p - 1;
}
_export
Pc
E_text_Z_s0_I_append_s( Pc *s
, Pc t
, Pc t_end
){  if( !( t_end - t ))
        return *s + ( t_end - t );
    Pc p;
    if( !( p = E_mem_Q_blk_I_append( s, t_end - t )))
        return 0;
    E_text_Z_s_P_copy_s_0( p - 1, t, t_end );
    return p - 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
Pc
E_text_Z_s_Z_cmdline_T_quote_n( Pc s
, Pc s_end
){  while( s != s_end )
        if( E_text_Z_c_T_quote( *s++ ))
            break;
    return s;
}
_export
Pc
E_text_Z_s0_Z_cmdline_T_quote( Pc s
){  while( *s )
    {   if( E_text_Z_c_T_quote( *s ))
            break;
        s++;
    }
    return s;
}
//------------------------------------------------------------------------------
_internal
void
E_text_Z_s_Z_cmdline_Z_arg_I_mode( S *cmd_mode
, Pc arg
){  switch( *cmd_mode )
    { case 0:
          if( E_text_Z_s0_T_eq_s0( arg, "gnome-terminal" ))
              *cmd_mode = 1;
          else if( E_text_Z_s0_T_eq_s0( arg, "su" ))
              *cmd_mode = 2;
          else if( E_text_Z_s0_T_eq_s0( arg, "sg" ))
              *cmd_mode = 3;
          else
              *cmd_mode = 0;
          break;
      case 1:
          if( E_text_Z_s0_T_eq_s0( arg, "-e" ))
              *cmd_mode = -*cmd_mode;
          break;
      case 2:
          if( E_text_Z_s0_T_eq_s0( arg, "-c" ))
              *cmd_mode = -*cmd_mode;
          break;
      case 3:
          if( !E_text_Z_s0_T_eq_s0( arg, "-" ))
              *cmd_mode = -*cmd_mode;
          break;
    }
}
//------------------------------------------------------------------------------
_internal
void
E_text_Z_s_Z_cmdline_N_quote_I_open( Pc cmdline
, N *l
, B *inside
, N level
){  for_n_rev( i, level )
    {   E_mem_Q_blk_P_fill_c( cmdline + *l, ( 1 << i ) - 1, '\\' );
        *l += ( 1 << i ) - 1;
        *( cmdline + *l ) = '\'';
        ++*l;
    }
    *inside = yes;
}
_internal
void
E_text_Z_s_Z_cmdline_N_quote_I_close( Pc cmdline
, N *l
, B *inside
, N level
){  for_n( i, level )
    {   E_mem_Q_blk_P_fill_c( cmdline + *l, ( 1 << i ) - 1, '\\' );
        *l += ( 1 << i ) - 1;
        *( cmdline + *l ) = '\'';
        ++*l;
    }
    *inside = no;
}
_export
N
E_text_Z_s_Z_cmdline_N_quote( Pc cmdline
, N level
, struct E_flow_Z_args *args
, N args_n
){  N l = 0;
    S cmd_mode = 0;
    B inside = no;
    for_n( args_i, args_n )
    {   for_n( i, args->argc )
        {   Pc arg = args->argv[i];
            if( !*arg )
                continue;
            if(l)
            {   if( !inside )
                    E_text_Z_s_Z_cmdline_N_quote_I_open( cmdline, &l, &inside, level );
                *( cmdline + l ) = ' ';
                l++;
            }
            if( cmd_mode < 0 )
            {   if(inside)
                    E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level );
                cmd_mode = 0;
                level++;
            }
            E_text_Z_s_Z_cmdline_Z_arg_I_mode( &cmd_mode, arg );
            Pc arg_;
            if( *( arg_ = E_text_Z_s0_R_search_c( arg, '\'' )))
            {   if( arg_ != arg )
                {   N arg_quote = 0;
                    if( *E_text_Z_s_Z_cmdline_T_quote_n( arg, arg_ ) != '\'' )
                    {   if(inside)
                            E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level );
                        arg_quote = 1;
                        E_text_Z_s_Z_cmdline_N_quote_I_open( cmdline, &l, &inside, level + arg_quote );
                    }else if( !inside )
                        E_text_Z_s_Z_cmdline_N_quote_I_open( cmdline, &l, &inside, level );
                    E_mem_Q_blk_I_copy( cmdline + l, arg, arg_ - arg );
                    l += arg_ - arg;
                    E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level + arg_quote );
                }else if( inside )
                    E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level );
                E_mem_Q_blk_P_fill_c( cmdline + l, ( 2 << level ) - 1, '\\' );
                l += ( 2 << level ) - 1;
                *( cmdline + l ) = '\'';
                l++;
                arg = arg_ + 1;
            }
            if( *arg_ )
                for( ; *( arg_ = E_text_Z_s0_R_search_c( arg, '\'' )); arg = arg_ + 1 )
                {   if( arg_ != arg )
                    {   N arg_quote = 0;
                        if( *E_text_Z_s_Z_cmdline_T_quote_n( arg, arg_ ) != '\'' )
                        {   arg_quote = 1;
                            E_text_Z_s_Z_cmdline_N_quote_I_open( cmdline, &l, &inside, level + arg_quote );
                        }
                        E_mem_Q_blk_I_copy( cmdline + l, arg, arg_ - arg );
                        l += arg_ - arg;
                        if( arg_quote )
                            E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level + arg_quote );
                    }
                    E_mem_Q_blk_P_fill_c( cmdline + l, ( 2 << level ) - 1, '\\' );
                    l += ( 2 << level ) - 1;
                    *( cmdline + l ) = '\'';
                    l++;
                }
            if( arg == args->argv[i] ) // Nie znalazł żadnego cudzysłowu.
            {   if( arg == args->argv[i] )
                    arg_ = E_text_Z_s0_R_end( args->argv[i] );
                N arg_quote = 0;
                if( *E_text_Z_s_Z_cmdline_T_quote_n( arg, arg_ ))
                {   if(inside)
                        E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level );
                    arg_quote = 1;
                    E_text_Z_s_Z_cmdline_N_quote_I_open( cmdline, &l, &inside, level + arg_quote );
                }else if( !inside )
                    E_text_Z_s_Z_cmdline_N_quote_I_open( cmdline, &l, &inside, level );
                E_mem_Q_blk_I_copy( cmdline + l, arg, arg_ - arg );
                l += arg_ - arg;
                if( arg_quote )
                    E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level + arg_quote );
            }else if( *arg ) // Znalazł i na końcu jest jeszcze coś oprócz zastąpionego cudzysłowu.
            {   if( arg == args->argv[i] )
                    arg_ = E_text_Z_s0_R_end( args->argv[i] );
                N arg_quote = 0;
                if( *E_text_Z_s_Z_cmdline_T_quote_n( arg, arg_ ))
                {   arg_quote = 1;
                    E_text_Z_s_Z_cmdline_N_quote_I_open( cmdline, &l, &inside, level + arg_quote );
                }
                E_mem_Q_blk_I_copy( cmdline + l, arg, arg_ - arg );
                l += arg_ - arg;
                if( arg_quote )
                    E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level + arg_quote );
            }
        }
        args++;
    }
    if(inside)
        E_text_Z_s_Z_cmdline_N_quote_I_close( cmdline, &l, &inside, level );
    return l;
}
//------------------------------------------------------------------------------
_internal
void
E_text_Z_s_Z_cmdline_N_quote_G_open(
  N *l
, B *inside
, N level
){  for_n_rev( i, level )
        *l += 1 << i;
    *inside = yes;
}
_internal
void
E_text_Z_s_Z_cmdline_N_quote_G_close(
  N *l
, B *inside
, N level
){  for_n( i, level )
        *l += 1 << i;
    *inside = no;
}
_export
N
E_text_Z_s_Z_cmdline_N_quote_G(
  N level
, struct E_flow_Z_args *args
, N args_n
){  N l = 0;
    S cmd_mode = 0;
    B inside = no;
    for_n( args_i, args_n )
    {   for_n( i, args->argc )
        {   Pc arg = args->argv[i];
            if( !*arg )
                continue;
            if(l)
            {   if( !inside )
                    E_text_Z_s_Z_cmdline_N_quote_G_open( &l, &inside, level );
                l++;
            }
            if( cmd_mode < 0 )
            {   if(inside)
                    E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level );
                cmd_mode = 0;
                level++;
            }
            E_text_Z_s_Z_cmdline_Z_arg_I_mode( &cmd_mode, arg );
            Pc arg_;
            if( *( arg_ = E_text_Z_s0_R_search_c( arg, '\'' )))
            {   if( arg_ != arg )
                {   N arg_quote = 0;
                    if( *E_text_Z_s_Z_cmdline_T_quote_n( arg, arg_ ) != '\'' )
                    {   if(inside)
                            E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level );
                        arg_quote = 1;
                        E_text_Z_s_Z_cmdline_N_quote_G_open( &l, &inside, level + arg_quote );
                    }else if( !inside )
                        E_text_Z_s_Z_cmdline_N_quote_G_open( &l, &inside, level );
                    l += arg_ - arg;
                    E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level + arg_quote );
                }else if( inside )
                    E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level );
                l += 2 << level;
                arg = arg_ + 1;
            }
            if( *arg_ )
                for( ; *( arg_ = E_text_Z_s0_R_search_c( arg, '\'' )); arg = arg_ + 1 )
                {   if( arg_ != arg )
                    {   N arg_quote = 0;
                        if( *E_text_Z_s_Z_cmdline_T_quote_n( arg, arg_ ) != '\'' )
                        {   arg_quote = 1;
                            E_text_Z_s_Z_cmdline_N_quote_G_open( &l, &inside, level + arg_quote );
                        }
                        l += arg_ - arg;
                        if( arg_quote )
                            E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level + arg_quote );
                    }
                    l += 2 << level;
                }
            if( arg == args->argv[i] )
            {   if( arg == args->argv[i] )
                    arg_ = E_text_Z_s0_R_end( args->argv[i] );
                N arg_quote = 0;
                if( *E_text_Z_s_Z_cmdline_T_quote_n( arg, arg_ ))
                {   if(inside)
                        E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level );
                    arg_quote = 1;
                    E_text_Z_s_Z_cmdline_N_quote_G_open( &l, &inside, level + arg_quote );
                }else if( !inside )
                    E_text_Z_s_Z_cmdline_N_quote_G_open( &l, &inside, level );
                l += arg_ - arg;
                if( arg_quote )
                    E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level + arg_quote );
            }else if( *arg )
            {   if( arg == args->argv[i] )
                    arg_ = E_text_Z_s0_R_end( args->argv[i] );
                N arg_quote = 0;
                if( *E_text_Z_s_Z_cmdline_T_quote_n( arg, arg_ ))
                {   arg_quote = 1;
                    E_text_Z_s_Z_cmdline_N_quote_G_open( &l, &inside, level + arg_quote );
                }
                l += arg_ - arg;
                if( arg_quote )
                    E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level + arg_quote );
            }
        }
        args++;
    }
    if(inside)
        E_text_Z_s_Z_cmdline_N_quote_G_close( &l, &inside, level );
    return l;
}
//==============================================================================
_export
N
E_text_Z_su0_R_n( Pc s
){  N n = 0;
    while( *s )
    {   U u;
        Pc s_ = E_text_Z_su_R_u( s, &u );
        if( s_ == s )
            break;
        if( ~u )
            n++;
        s = s_;
    }
    return n;
}
_export
N // Ile bajtów zapisano lub błąd.
E_text_Z_u_R_su( U u
, Pc s
){  if( u > 0x10ffff
    || ( u >= 0xd800
      && u <= 0xdfff
    ))
        return ~0;
    if( u <= 0x7f )
    {   s[0] = (C)u;
        return 1;
    }
    N first = 0x1f;
    N i = 0;
    while( u > first )
    {   s[i] = (C)(( u & 0x3f ) | 0x80 );
        first >>= 1;
        if( !first )
            return ~0;
        u >>= 6;
        i++;
    }
    s[i] = (C)(( ~first << 2 ) | u );
    E_text_Z_sl_P_rev( s, i + 1 );
    return i + 1;
}
_export
N
E_text_Z_u_R_su_G( U u
){  if( u > 0x10ffff
    || ( u >= 0xd800
      && u <= 0xdfff
    ))
        return ~0;
    if( u <= 0x7f )
        return 1;
    N first = 0x1f;
    N i = 0;
    while( u > first )
    {   first >>= 1;
        if( !first )
            return ~0;
        u >>= 6;
        i++;
    }
    return i + 1;
}
_export
Pc
E_text_Z_su_R_u( Pc s
, U *u
){  if( (S8)*s >= 0 )
    {   *u = *s;
        return s + 1;
    }
    U v = ~(S8)*s;
    N n = E_asm_I_bsr(v);
    if( n == ~0
    || n == 6
    || n < 3
    )
    {   *u = ~0;
        return s;
    }
    v = *s & (( 1 << n ) - 1 );
    n = 6 - n;
    for_n( i, n )
    {   if(( *++s & 0xc0 ) != 0x80 )
        {   *u = ~0;
            return s;
        }
        v <<= 6;
        v |= *s & 0x3f;
    }
    *u = (( n == 1 && v > 0x7f )
      || ( n == 2 && v > 0x7ff )
      || ( n == 3 && v > 0xffff )
    )
    && v <= 0x10ffff
    && ( v < 0xd800
    || v > 0xdfff
    )
    ? v
    : ~0;
    return s + 1;
}
_export
Pc
E_text_Z_su_R_u_rev( Pc s
, U *u
){  if( (S8)*( s - 1 ) >= 0 )
    {   *u = *( s - 1 );
        return s - 1;
    }
    U v = 0;
    for_n( i, 3 )
    {   if(( *--s & 0xc0 ) != 0x80 )
            break;
        v |= ( (U)*s & 0x3f ) << ( i * 6 );
    }
    N n = E_asm_I_bsr( ~(S8)*s );
    if( n == ~0
    || n != 6 - i
    )
    {   *u = ~0;
        return s + 1;
    }
    v |= ( *s & (( _v( v, 1 ) << n ) - 1 )) << (( i + 1 ) * 6 );
    *u = (( i == 1 && v > 0x7f )
      || ( i == 2 && v > 0x7ff )
      || ( i == 3 && v > 0xffff )
    )
    && v <= 0x10ffff
    && ( v < 0xd800
    || v > 0xdfff
    )
    ? v
    : ~0;
    return s;
}
_export
N // Ile bajtów przeczytano lub błąd odczytu.
E_text_Z_getter_Z_c_R_u( N (*getter)(void)
, U *u
){  N c = getter();
    if( !~c
    || c == S_eof
    )
        return c;
    if( (S8)c >= 0 )
    {   *u = c;
        return 1;
    }
    N n = E_asm_I_bsr( ~(S8)c );
    if( n == ~0
    || n == 6
    || n < 3
    )
        return ~0;
    U v = c & (( _v( v, 1 ) << n ) - 1 );
    n = 6 - n;
    for_n( i, n )
    {   c = getter();
        if( !~c
        || c == S_eof
        )
            return c;
        if(( c & 0xc0 ) != 0x80 )
            return ~0;
        v <<= 6;
        v |= c & 0x3f;
    }
    *u = (( n == 1 && v > 0x7f )
      || ( n == 2 && v > 0x7ff )
      || ( n == 3 && v > 0xffff )
    )
    && v <= 0x10ffff
    && ( v < 0xd800
    || v > 0xdfff
    )
    ? v
    : ~0;
    return 1 + n;
}
_export
N
E_text_Z_getter_Z_c_R_u_rev( N (*getter)(void)
, U *u
){  N c = getter();
    if( !~c
    || c == S_eof
    )
        return c;
    if( (S8)c >= 0 )
    {   *u = c;
        return 1;
    }
    U v = 0;
    for_n( i, 3 )
    {   if(( c & 0xc0 ) != 0x80 )
            break;
        v |= ( c & 0x3f ) << ( i * 6 );
        c = getter();
        if( !~c
        || c == S_eof
        )
            return c;
    }
    N n = E_asm_I_bsr( ~(S8)c );
    if( n == ~0
    || n != 6 - i
    )
    {   *u = ~0;
        return 1 + i;
    }
    v |= ( c & (( _v( v, 1 ) << n ) - 1 )) << ( i * 6 );
    *u = (( i == 1 && v > 0x7f )
      || ( i == 2 && v > 0x7ff )
      || ( i == 3 && v > 0xffff )
    )
    && v <= 0x10ffff
    && ( v < 0xd800
    || v > 0xdfff
    )
    ? v
    : ~0;
    return 1 + i;
}
/******************************************************************************/
