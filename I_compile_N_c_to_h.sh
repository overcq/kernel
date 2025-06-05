#!/bin/sh
################################################################################
#   ___   publicplace
#  ¦OUX¦  ‘unix’ “sh” environment
#  ¦/C+¦  OUX/C+ OS
#   ---   kernel
#         “.c” to “.h” utility for structure fragments from a source file written in conformance to the strict syntax style
# ©overcq                on ‟Gentoo Linux 13.0” “x86_64”             2015‒1‒13 #
################################################################################
case "$1" in
-h1) # type forward declarations
    perl -e '
        use strict;
        use warnings;
        local $\ = $/;
        while(<>)
        {   chomp;
            s`//.*$``;
            if( /^\s*#(?:if|(?:elif|else|endif)\b)/ )
            {   print;
            }elsif( /^(?:struct|union)\s+E_\w+/ ) # typy publiczne
            {   print "$&;";
            }
        }
    ' "$2"
    ;;
-h2) # type definitions, variable and procedure forward declarations
    perl -e '
        use strict;
        use warnings;
        my $extern = 0;
        my $inside_braces = 0;
        my $last_line = '\'\'';
        local $\ = $/;
        while(<>)
        {   chomp;
            s`//.*$``;
            if( $inside_braces == 0 )
            {   if( $last_line =~ /^_(?:export|private)$/ )
                {   $extern = 1;
                }elsif( $last_line =~ /;/ )
                {   $extern = 0;
                }
            }
            if( $inside_braces == 1 )
            {   if( /^{/ )
                {   print $last_line;
                    print;
                    $inside_braces = 2;
                    next;
                }
                $inside_braces = 0;
                $extern = 0;
            }
            if( $inside_braces == 2 )
            {   if( /^}[^=;]*/ )
                {   my $s = $&;
                    if( /^}\s*(\*)*\w/ )
                    {   print '\''}extern'\''. ( !defined( $1 ) ? '\'\ \'' : '\'\'' ) . substr( $s, 1 ) .'\'';'\'';
                    }else
                    {   print $s .'\'';'\'';
                    }
                    $inside_braces = /;/ ? 0 : 3;
                    $extern = 0;
                }else
                {   print;
                }
                next;
            }
            if( $inside_braces == 3 )
            {   $inside_braces = 0 if /^}/;
                next;
            }
            if( $inside_braces == 4 )
            {   if( /^\)/ )
                {   print $& .'\'';'\'';
                    $inside_braces = 0;
                    $extern = 0;
                }else
                {   print;
                }
                next;
            }
            if( /^\s*#(?:if|(?:elif|else|endif)\b)/ )
            {   print;
            }elsif( /^_(?:export|private)\s+(?:\w+\s+)*?((?:(?:enum|struct|union)\s+)?.*?\b\(?\**E_[^=;]*)[=;]/ ) # zmienne publiczne
            {   print "extern $1;";
            }elsif( /^(?:enum|struct|union)\s+E_\w+/ ) # typy publiczne
            {   $inside_braces = 1;
                $last_line = $_;
            }elsif( $extern and /^E_\w+\(/ ) # procedura publiczna
            {   print $last_line;
                print;
                $inside_braces = 4;
            }else
            {   $last_line = $_;
            }
        }
    ' "$2"
    ;;
-c)
    perl -e '
        use strict;
        use warnings;
        my $extern = 0;
        my $inside_comment = 0;
        my $inside_braces = 0;
        my $inside_enum;
        my $last_line = '\'\'';
        local $\ = $/;
        sub print1
        {   print $last_line if defined $last_line;
            $last_line = shift;
            $last_line = $_ unless defined $last_line;
        }
        sub print_blank
        {   print $last_line if defined $last_line;
            $last_line = "";
        }
        while(<>)
        {   chomp;
            if( $inside_comment )
            {   if( /\*\// )
                {   $_ = $'\'';
                    $inside_comment = 0;
                }else
                {   print_blank;
                    next;
                }
            }
            s`//.*$``;
            if( $inside_braces == 0 )
            {   if( $last_line =~ /^_(?:export|private)$/ )
                {   $extern = 1;
                }elsif( $last_line =~ /;/ )
                {   $extern = 0;
                }
            }
            if( $inside_braces == 1 )
            {   if( /^{/ )
                {   print_blank;
                    $inside_braces = 2;
                    next;
                }
                $inside_braces = 0;
                $extern = 0;
            }
            if( $inside_braces == 2 )
            {   if( /^}/ )
                {   unless( $inside_enum )
                    {   $inside_braces = /;/ ? 0 : 3;
                        s`^.``;
                        print1;
                        $extern = 0;
                    }else
                    {   $inside_braces = 0;
                        print_blank;
                    }
                }else
                {   print_blank;
                }
                next;
            }
            if( $inside_braces == 3 )
            {   print1;
                $inside_braces = 0 if /^}/;
                next;
            }
            if( $inside_braces == 4 )
            {   if( /^\)/ )
                {   print1;
                    $inside_braces = 0;
                    $extern = 0;
                }else
                {   print1;
                }
                next;
            }
            if( /^_(?:export|private)\s+(?:\w+\s+)*?(?:(?:enum|struct|union)\s+)?.*?\b\**E_.*;/ ) # zmienne publiczne
            {   print1;
            }elsif( /^(?:enum|struct|union)\s+E_\w/ ) # typy publiczne
            {   $inside_enum = /^enum/;
                unless( $inside_enum )
                {   print1;
                }else
                {   print_blank;
                }
                $inside_braces = 1;
            }elsif( $extern and /^E_\w+\(/ ) # procedura publiczna
            {   print1;
                $inside_braces = 4;
            }elsif( /^\/\*/ ) # komentarz blokowy
            {   if( /\*\// )
                {   $_ = $'\'';
                    redo;
                }
                print_blank;
                $inside_comment = 1
            }else
            {   print1;
            }
        }
        print $last_line if defined $last_line;
    ' "$2"
    ;;
esac
################################################################################
