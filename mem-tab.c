/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         memory tables
* ©overcq                on ‟Gentoo Linux 13.0” “x86_64”             2015‒4‒28 *
*******************************************************************************/
struct E_mem_Q_tab_Z
{ Pc *index; // Tablica mapowania indeksów do adresów w “data”.
  Pc data; // Dane ciągłe.
  N u; // Rozmiar elementu tablicy.
  struct E_mem_Q_tab_Z *iterator;
  I index_n;
  I data_n;
};
struct E_mem_Q_tab_S_iterator_Z
{ I *index; // Dowolna sekwencja ‹identyfikatorów› (indeksów) danych, ale bez powtórzeń.
  I n;
};
//==============================================================================
_export
struct E_mem_Q_tab_Z *
E_mem_Q_tab_M(
  N u
, I n
){  struct E_mem_Q_tab_Z *tab_ = E_mem_Q_blk_M( sizeof( struct E_mem_Q_tab_Z ));
    if( !tab_ )
        return 0;
    Mt_( tab_->index, n );
    if( !tab_->index )
    {   W(tab_);
        return 0;
    }
    tab_->data = Mt( u, n );
    if( !tab_->data )
    {   W( tab_->index );
        W(tab_);
        return 0;
    }
    tab_->index_n = n;
    tab_->data_n = n;
    tab_->u = u;
    for_i( i, n )
        tab_->index[i] = tab_->data + i * tab_->u;
    tab_->iterator = 0;
    return tab_;
}
_export
void
E_mem_Q_tab_W( struct E_mem_Q_tab_Z *tab_
){  if( tab_->iterator )
    {   for_each( i, tab_->iterator, E_mem_Q_tab )
        {   struct E_mem_Q_tab_S_iterator_Z *iterator = (P)tab_->iterator->index[i];
            W( iterator->index );
        }
        W( tab_->iterator->data );
        W( tab_->iterator->index );
        W( tab_->iterator );
    }
    W( tab_->data );
    W( tab_->index );
    W(tab_);
}
//------------------------------------------------------------------------------
_export
I
E_mem_Q_tab_I_add( struct E_mem_Q_tab_Z *tab_
){  for_i( id, tab_->index_n )
        if( !tab_->index[id] )
            break;
    if( !~id )
        return id;
    Pc data = tab_->data;
    N n_prepended;
    if( !E_mem_Q_blk_I_add( &tab_->data, 1, &n_prepended, 0 ))
        return ~0;
    if( tab_->data != data )
    {   for_i( i, id )
            tab_->index[i] += tab_->data + ( n_prepended ? tab_->u : 0 ) - data;
        if( i != tab_->index_n )
            while( ++i != tab_->index_n )
                if( tab_->index[i] )
                    tab_->index[i] += tab_->data + ( n_prepended ? tab_->u : 0 ) - data;
    }
    if( id == tab_->index_n )
    {   if( !E_mem_Q_blk_I_append( &tab_->index, 1 ))
        {   if( !E_mem_Q_blk_I_remove( &tab_->data, n_prepended ? 0 : tab_->data_n, 1 ))
                E_main_I_error_fatal();
            return ~0;
        }
        tab_->index_n++;
    }
    tab_->index[id] = tab_->data + ( n_prepended ? 0 : tab_->data_n * tab_->u );
    tab_->data_n++;
    if( tab_->iterator )
    {   for_i( i, tab_->iterator->index_n )
        {   struct E_mem_Q_tab_S_iterator_Z *iterator = (P)tab_->iterator->index[i];
            if(iterator)
            {   I *p = E_mem_Q_blk_I_append( &iterator->index, 1 );
                if( !p )
                    E_main_I_error_fatal();
                *p = id;
                iterator->n++;
            }
        }
    }
    return id;
}
_export
I
E_mem_Q_tab_I_add_i( struct E_mem_Q_tab_Z *tab_
, I id
){  Pc data = tab_->data;
    N n_prepended;
    if( !E_mem_Q_blk_I_add( &tab_->data, 1, &n_prepended, 0 ))
        return ~0;
    if( tab_->data != data )
    {   if( id < tab_->index_n )
        {   for_i( i, id )
                if( tab_->index[i] )
                    tab_->index[i] += tab_->data + ( n_prepended ? tab_->u : 0 ) - data;
            if( i != tab_->index_n )
                while( ++i != tab_->index_n )
                    if( tab_->index[i] )
                        tab_->index[i] += tab_->data + ( n_prepended ? tab_->u : 0 ) - data;
        }else
        {   for_i( i, tab_->index_n )
                if( tab_->index[i] )
                    tab_->index[i] += tab_->data + ( n_prepended ? tab_->u : 0 ) - data;
        }
    }
    if( id >= tab_->index_n )
    {   if( !E_mem_Q_blk_I_append( &tab_->index, id + 1 - tab_->index_n ))
        {   if( !E_mem_Q_blk_I_remove( &tab_->data, n_prepended ? 0 : tab_->data_n, id + 1 - tab_->index_n ))
                E_main_I_error_fatal();
            return ~0;
        }
        for( I i = tab_->index_n; (In)i < (In)id; i++ )
            tab_->index[i] = 0;
        tab_->index_n = id + 1;
    }
    tab_->index[id] = tab_->data + ( n_prepended ? 0 : tab_->data_n * tab_->u );
    tab_->data_n++;
    if( tab_->iterator )
    {   for_i( i, tab_->iterator->index_n )
        {   struct E_mem_Q_tab_S_iterator_Z *iterator = (P)tab_->iterator->index[i];
            if(iterator)
            {   I *p = E_mem_Q_blk_I_append( &iterator->index, 1 );
                if( !p )
                    E_main_I_error_fatal();
                *p = id;
                iterator->n++;
            }
        }
    }
    return id;
}
_export
N
E_mem_Q_tab_I_remove( struct E_mem_Q_tab_Z *tab_
, I id
){  if( tab_->iterator )
    {   for_i( i, tab_->iterator->index_n )
            if( tab_->iterator->index[i] )
            {   struct E_mem_Q_tab_S_iterator_Z *iterator = (P)tab_->iterator->index[i];
                for_i( j, iterator->n )
                    if( iterator->index[j] == id )
                    {   iterator->n--;
                        if( !E_mem_Q_blk_I_remove( &iterator->index, j, 1 ))
                            return ~0;
                        break;
                    }
            }
    }
    Pc p = tab_->index[id];
    if( id == tab_->index_n - 1 )
    {   I i = id;
        while(i)
            if( tab_->index[ --i ])
            {   i++;
                break;
            }
        if( E_mem_Q_blk_I_remove( &tab_->index, i, id - i + 1 ))
            tab_->index_n = i;
        else
            tab_->index[id] = 0;
    }else
        tab_->index[id] = 0;
    Pc data = tab_->data;
    tab_->data_n--;
    if( !E_mem_Q_blk_I_remove( &tab_->data, ( p - tab_->data ) / tab_->u, 1 ))
        return ~0;
    if( tab_->data != data )
    {   for_i( i, tab_->index_n )
            if( tab_->index[i] )
            {   if( tab_->index[i] > p )
                    tab_->index[i] -= tab_->u;
                tab_->index[i] += tab_->data - data;
            }
    }else
    {   for_i( i, tab_->index_n )
            if( tab_->index[i] > p )
                tab_->index[i] -= tab_->u;
    }
    return 0;
}
#if 0
_export
N
E_mem_Q_tab_I_copy( //NDFN kolejność do odwrócenia.
  struct E_mem_Q_tab_Z *tab_src
, I src_id
, struct E_mem_Q_tab_Z *tab_dest
){  I dest_id = E_mem_Q_tab_I_add( tab_dest );
    E_mem_Q_blk_P_copy( tab_dest->index[ dest_id ], tab_src->index[ src_id ], tab_src->u );
    return dest_id;
}
_export
N
E_mem_Q_tab_I_move(
  struct E_mem_Q_tab_Z *tab_src
, I src_id
, struct E_mem_Q_tab_Z *tab_dest
){  I dest_id = E_mem_Q_tab_I_copy( tab_src, src_id, tab_dest );
    E_mem_Q_tab_I_remove( tab_src, src_id );
    return dest_id;
}
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Iteratory statyczne. Gdy wiadomo, że nie nastąpi przełączenie ‹zadania›, ponieważ wyliczenie wartości musi być wykonane w jednym ‹atomie›, i nie nastąpi zmiana kolekcji.
_export
I
E_mem_Q_tab_Z_iter_R_next( struct E_mem_Q_tab_Z *tab_
, I id
, I out_id // Pomijany— i od następnego do poprzedniego elementu.
){  if( (In)id + 1 > (In)out_id )
    {   while( ++id != tab_->index_n )
            if( tab_->index[id] )
                return id;
        id = ~0;
    }
    if( !~out_id )
        out_id = tab_->index_n;
    while( ++id != out_id )
        if( tab_->index[id] )
            return id;
    return ~0;
}
_export
I
E_mem_Q_tab_Z_iter_R_prev( struct E_mem_Q_tab_Z *tab_
, I id
, I out_id
){  if( !~id )
        id = tab_->index_n;
    if( (In)id - 1 < (In)out_id )
    {   while( ~--id )
            if( tab_->index[id] )
                return id;
        if( !~out_id )
            return ~0;
        id = tab_->index_n;
    }
    while( --id != out_id )
        if( tab_->index[id] )
            return id;
    return ~0;
}
//------------------------------------------------------------------------------
_export
I
E_mem_Q_tab_Q_iter_M( struct E_mem_Q_tab_Z *tab_
, I out_id
){  I n = tab_->data_n;
    if( ~out_id
    && n
    )
        n--;
    I iterator_id;
    if( tab_->iterator )
    {   iterator_id = E_mem_Q_tab_I_add( tab_->iterator );
        if( !~iterator_id )
            return ~0;
    }else
    {   tab_->iterator = E_mem_Q_tab_M( sizeof( struct E_mem_Q_tab_S_iterator_Z ), 1 );
        if( !tab_->iterator )
            return ~0;
        iterator_id = 0;
    }
    struct E_mem_Q_tab_S_iterator_Z *iterator = (P)tab_->iterator->index[ iterator_id ];
    Mt_( iterator->index, n );
    if( !iterator->index )
    {   if( iterator_id )
        {   if( !E_mem_Q_tab_I_remove( tab_->iterator, iterator_id ))
                E_main_I_error_fatal();
        }else
            W_tab_( tab_->iterator );
        return ~0;
    }
    iterator->n = n;
    I i = 0;
    I id;
    for( id = out_id + 1; id != tab_->index_n; id++ )
        if( tab_->index[id] )
            iterator->index[ i++ ] = id;
    if( ~out_id )
    {   for_i_( id, out_id )
            if( tab_->index[id] )
                iterator->index[ i++ ] = id;
    }
    return iterator_id;
}
// Do zwalniania ‘iteratora’ tylko wtedy, gdy nie zakończy się wartością “empty” (czyli gdy pętla zostanie przerwana).
_export
void
E_mem_Q_tab_Q_iter_W( struct E_mem_Q_tab_Z *tab_
, I iterator_id
){  struct E_mem_Q_tab_S_iterator_Z *iterator = (P)tab_->iterator->index[ iterator_id ];
    W( iterator->index );
    if( tab_->iterator->data_n != 1 )
    {   if( !E_mem_Q_tab_I_remove( tab_->iterator, iterator_id ))
            E_main_I_error_fatal();
    }else
        W_tab_( tab_->iterator );
}
    #if 0 // Brak koncepcji pętli usuwających.
_export
I
E_mem_Q_tab_Q_iter_R_shift( struct E_mem_Q_tab_Z *tab_
, I iterator_id
){  struct E_mem_Q_tab_S_iterator_Z *iterator = (P)tab_->iterator->index[ iterator_id ];
    if( !iterator->n )
    {   E_mem_Q_tab_Q_iter_W( tab_, iterator_id );
        return ~0;
    }
    I id = iterator->index[0];
    iterator->n--;
    if( !E_mem_Q_blk_I_remove( &iterator->index, 0, 1 ))
        E_main_I_error_fatal();
    return id;
}
    #endif
_export
I
E_mem_Q_tab_Q_iter_R_next( struct E_mem_Q_tab_Z *tab_
, I iterator_id
, I id
){  struct E_mem_Q_tab_S_iterator_Z *iterator = (P)tab_->iterator->index[ iterator_id ];
    I ret = (In)id < (In)iterator->n ? iterator->index[id] : ~0;
    if( !~ret )
    {   W( iterator->index );
        if( tab_->iterator->data_n != 1 )
        {   if( !E_mem_Q_tab_I_remove( tab_->iterator, iterator_id ))
                E_main_I_error_fatal();
        }else
            W_tab_( tab_->iterator );
    }
    return ret;
}
/******************************************************************************/
